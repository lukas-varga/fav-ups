#include "Game.h"
#include "Player.h"
#include "Command.h"
#include "Help.h"
#include "State.h"
#include "Lobby.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sstream>
#include <vector>
#include "string"


using namespace std;

int main (int argc, char** argv){
    if (argc != 3){
        cout << "Please enter arguments: <port> <num_players>" << endl;
        exit(0);
    }

    int port, num_players;
    try {
        port = stoi(argv[1]);
        num_players = stoi(argv[2]);
        if (num_players < 2){
            throw exception();
        }
    }
    catch (const exception &exc){
        cerr << exc.what() << endl;
        cout << "Arguments are not valid. Num of players must be >2 -> Exiting" << endl;
        exit(0);
    }

    const int NAME_LENGTH = 10;
    const int CLIENT_NUM = num_players;
    const int GAME_NUM = CLIENT_NUM / 2;
    const int MAX_BUFF = 2048;
    char buff[MAX_BUFF];
    const char SPL_CHR = '|';

    int client_socket;
    int server_socket;
    int ret_val, val_read;
    int fd;
    int i;
    socklen_t client_len, server_len;
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    Game * game_arr[GAME_NUM];
    Player * player_arr[CLIENT_NUM];

    for (i = 0; i < CLIENT_NUM; i++){
        player_arr[i] = new Player(0, "");
    }
    for (i = 0; i < GAME_NUM; i++){
//        Player * white_p = player_arr[i * 2];
//        Player * black_p = player_arr[(i * 2)+ 1];
        game_arr[i] = new Game();
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    server_len = sizeof(my_addr);
    ret_val = bind(server_socket, (struct sockaddr *) &my_addr, server_len);
    if (ret_val == 0)
        printf("Bind - OK\n");
    else {
        printf("Bind - ERR\n");
        return -1;
    }

    // CLIENT_NUM devices connected to chat
    ret_val = listen(server_socket, CLIENT_NUM);
    if (ret_val == 0){
        printf("Listen - OK\n");
    } else {
        printf("Listen - ERR\n");
        return -1;
    }

    // vyprazdnime sadu deskriptoru a vlozime server socket
    FD_ZERO( &client_socks );
    FD_SET( server_socket, &client_socks );

    while(true) {
        memset(buff, 0, MAX_BUFF);
        string rcv, snd;
        vector<string> data;
        string cmd, login_name;

        Lobby * lobby;
        Player * player;
        int index;
        Game * game;

        tests = client_socks;
        printf("Server is waiting...\n");

        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        ret_val = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
        if (ret_val < 0) {
            printf("Select - ERR\n");
            return -1;
        }

        // vynechavame stdin, stdout, stderr
        for (fd = 3; fd < FD_SETSIZE; fd++) {
            // je dany socket v sade fd ze kterych lze cist ?
            if (FD_ISSET(fd, &tests)) {
                // je to server socket ? prijmeme nove spojeni
                if (fd == server_socket) {
                    client_len = sizeof(peer_addr);

                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &client_len);
                    FD_SET(client_socket, &client_socks);
                    printf("New connection - socket fd is %d\n", client_socket );

                    // send new connection greeting welcome_msg
                    snd = "";
                    snd.append("Welcome to Onitama server!");
                    snd.append(1, Help::END);
                    send(client_socket, snd.data(), snd.size(), 0);
                    Help::send_log(client_socket, snd);

                    // Check for reconnect
                    Player * p;
                    bool reconnect = false;
                    for (i = 0; i < CLIENT_NUM; i++){
                        p = player_arr[i];
                        if(p->socket == client_socket){
                            printf("Socket already in array at index %d\n" , i);
                            reconnect = true;
                            break;
                        }
                    }
                    // Looking for empty Player slot
                    if (!reconnect){
                        for (i = 0; i < CLIENT_NUM; i++){
                            p = player_arr[i];
                            if(p->socket == 0){
                                p->socket = client_socket;
                                printf("Adding to list of sockets at index %d\n" , i);
                                break;
                            }
                        }
                    }
                }

                // je to klientsky socket ? prijmem rcv
                else {
                    rcv = "";
                    val_read = recv(fd, buff, sizeof(buff), 0);
                    rcv.append(buff);
                    Help::recv_log(fd, buff);

                    // Disconnecting
                    // na socketu se stalo neco spatneho
                    if (val_read == 0){
                        player = nullptr;
                        for (i = 0; i < CLIENT_NUM; i++) {
                            if (fd == player_arr[i]->socket){
                                player = player_arr[i];
                                index = i;
                                break;
                            }
                        }

                        // Player found in arr
                        if (player != nullptr) {
                            // Not logged player -> Delete Player Record completely
                            if (player->username.empty()) {
                                player_arr[index] = new Player(0, "");
                            }
                            else {
                                // Already logged
                                for (i = 0; i < GAME_NUM; ++i) {
                                    game = game_arr[i];
                                    if (game->white_p == player or game->black_p == player) {
                                        // In Game -> Remember NAME for reconnect
                                        if (game->is_active) {
                                            player->socket = 0;
                                            player->disconnected = true;
                                            player->state = State_Machine::allowed_transition(player->state,Event::EV_DISC);
                                            lobby->inform_disconnecting(GAME_NUM, game_arr, player->username);
                                        }
                                        // Waiting room -> Delete Player Record completely and REMOVE the GAME
                                        else {
                                            game_arr[i] = new Game();
                                            player_arr[i] = new Player(0, "");
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                        else{
                            cout << "ERR: Player connected but not found in array of players (CLOSING)!" << endl;
                        }

                        // Remove closed fd
                        // (from the unmodified fd_set readfds)
                        close(fd);
                        FD_CLR(fd, &client_socks);
                        printf("Removing client on fd %d\n", fd);
                    }

                    // mame co cist
                    else {
                        player = nullptr;
                        for (i = 0; i < CLIENT_NUM; i++) {
                            if (fd == player_arr[i]->socket){
                                player = player_arr[i];
                                index = i;
                                break;
                            }
                        }
                        // Player found in arr
                        if (player != nullptr){
                            // Parsing
                            data = Help::parse(rcv, SPL_CHR);
                            cmd = data.at(0);
                            lobby = new Lobby();
                            // LOGIN | name
                            if (cmd == Command::name(Cmd::LOGIN)){
                                if (data.size() == 1 + 1) {
                                    cout << "Entering: " << Command::name(Cmd::LOGIN) << endl;
                                    login_name = data.at(1);
                                    if (!login_name.empty()){
                                        bool name_in_use = false;
                                        bool reconnect = false;
                                        int temp_socket;

                                        // Check for reconnect
                                        for (i = 0; i < GAME_NUM; ++i){
                                            game = game_arr[i];
                                            if (game->is_active){
                                                // Remove player login but take socket number for game
                                                if(game->black_p->disconnected or game->white_p->disconnected) {
                                                    // Reconnect attempt with login
                                                    if (game->black_p->username == login_name
                                                            or game->white_p->username == login_name) {
                                                        for (i = 0; i < CLIENT_NUM; ++i){
                                                            Player * p = player_arr[i];
                                                            if (p->username == login_name and p != player){
                                                                temp_socket = player->socket;
                                                                player_arr[i]->socket = temp_socket;
                                                                player_arr[i]->disconnected = false;
                                                                player_arr[i]->state = State_Machine::allowed_transition(player_arr[i]->state, Event::EV_RECON);

                                                                lobby->reconnect(GAME_NUM, game_arr, login_name, temp_socket);
                                                                lobby->inform_reconnecting(GAME_NUM, game_arr, login_name);

                                                                break;
                                                            }
                                                        }

                                                        player_arr[index] = new Player(0, "");
                                                        reconnect = true;
                                                    }
                                                }
                                            }
                                        }


                                        // Not reconnect try to login as new user
                                        if (!reconnect){
                                            for (i = 0; i < CLIENT_NUM; ++i){
                                                Player * p = player_arr[i];
                                                // Name used, try different one
                                                if (p->username == login_name){
                                                    lobby->already_in_use(fd);
                                                    name_in_use = true;
                                                }
                                            }

                                            // Original name detected and not reconect
                                            if (!name_in_use) {
                                                // Name not too long
                                                if (login_name.size() > NAME_LENGTH) {
                                                    lobby->name_too_long(fd, NAME_LENGTH);
                                                }
                                                // Forbidden chars
                                                else if (login_name.find(Help::SPL) != string::npos
                                                         or login_name.find(Help::END) != string::npos){
                                                    lobby->forbidden_chars(fd);
                                                }
                                                // Correct name
                                                else {
                                                    player->username = login_name;
                                                    player->state = State_Machine::allowed_transition(player->state, Event::EV_LOGIN);

                                                    cout << "Name correct and in lobby!" << endl;
                                                    bool found = lobby->find_lobby(GAME_NUM, game_arr, player);
                                                    if(!found){
                                                        lobby->lobby_full(fd);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else{
                                        lobby->empty_login(fd);
                                    }
                                }
                                else{
                                    cout << "ERR: Wrong num of args!" << endl;
                                }
                            }
                            else if (cmd == Command::name(Cmd::MAKE_MOVE)){
                                if (data.size() == 5 + 1){
                                    cout << "Entering: " << Command::name(Cmd::MAKE_MOVE) << endl;
                                    for (i = 0; i < GAME_NUM; i++) {
                                        game = game_arr[i];
                                        if (game->is_active and game->curr_p == player
                                                and !game->black_p->disconnected
                                                and !game->white_p->disconnected) {
                                            try {
                                                string card = data.at(1);
                                                int st_row = stoi(data.at(2));
                                                int st_col = stoi(data.at(3));
                                                int end_row = stoi(data.at(4));
                                                int end_col = stoi(data.at(5));
                                                bool valid_move = game->check_move(
                                                        card,st_row, st_col, end_row, end_col);
                                                if (valid_move) {
                                                    player->state = State_Machine::allowed_transition(player->state, Event::EV_MOVE);
                                                    game->move_was_made(card, st_row, st_col, end_row, end_col);
                                                    if (!game->is_end()) {
                                                        game->shuffle_cards(card);
                                                        game->switch_players();
                                                    }
                                                    else {
                                                        game->game_over();
                                                        // Refresh / Reset the game so anyone can enter again
                                                        game_arr[i]->init();
                                                    }
                                                }
                                                else {
                                                    player->state = State_Machine::allowed_transition(player->state, Event::EV_INVALID);
                                                    game->invalid_move();
                                                }
                                            }
                                            catch (const exception &exc){
                                                cerr << exc.what() << endl;
                                                game->move_not_parsable();

                                            }
                                            break;
                                        }
                                    }
                                }
                                else{
                                    cout << "ERR: Wrong num of args!" << endl;
                                }
                            }
                            else if (cmd == Command::name(Cmd::MAKE_PASS)){
                                if (data.size() == 1 + 1){
                                    cout << "Entering: " << Command::name(Cmd::MAKE_PASS) << endl;
                                    for (i = 0; i < GAME_NUM; i++) {
                                        game = game_arr[i];
                                        if (game->is_active and game->curr_p == player
                                                and !game->black_p->disconnected
                                                and !game->white_p->disconnected) {
                                            try {
                                                string card = data.at(1);
                                                bool valid_swap = game->check_pass();
                                                if (valid_swap) {
                                                    player->state = State_Machine::allowed_transition(player->state, Event::EV_MOVE);
                                                    game->pass_was_made(card);
                                                    game->shuffle_cards(card);
                                                    game->switch_players();
                                                }
                                                else {
                                                    player->state = State_Machine::allowed_transition(player->state, Event::EV_INVALID);
                                                    game->invalid_move();
                                                }
                                            }
                                            catch (const exception &exc){
                                                cerr << exc.what() << endl;
                                                game->move_not_parsable();
                                            }
                                            break;
                                        }
                                    }
                                }
                                else{
                                    cout << "ERR: Wrong num of args!" << endl;
                                }
                            }
                            else if (cmd == Command::name(Cmd::REMATCH)){
                                if (data.size() == 1 + 1){
                                    if (player->username == data[1]){
                                        bool found = lobby->find_lobby(GAME_NUM, game_arr, player);
                                        if(!found){
                                            lobby->lobby_full(fd);
                                        }
                                    }
                                    else{
                                        lobby->rematch_name(fd);
                                    }
                                }
                                else{
                                    cout << "ERR: Wrong num of args!" << endl;
                                }
                            }
                            else if (cmd == "WRONG_DATA") {
                                cout << "Wrong data -> server do not know how to respond!" << endl;
                            }
                            else{
                                cout << "ERR: Unknown message in Server!"<< endl;
                            }
                        }
                        else{
                            cout << "ERR: Player connected but not found in array of players (READING)!" << endl;
                        }
                    }
                }
            }
        }
    }
}
