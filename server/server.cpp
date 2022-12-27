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
        Player * p;
        Game * game;
        Player * player;
        bool lobby_entered, name_in_use;

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
                    Help::send_log(fd, snd);

                    for (i = 0; i < CLIENT_NUM; i++){
                        p = player_arr[i];
                        if(p->socket == 0 && p->username.empty())
                        {
                            p->socket = client_socket;
                            printf("Adding to list of sockets at index %d\n" , i);
                            break;
                        }
                    }
                }
                // je to klientsky socket ? prijmem rcv
                else {
                    rcv = "";
                    val_read = recv(fd, buff, sizeof(buff), 0);
                    rcv.append(buff);
                    Help::recv_log(fd, buff);

                    // na socketu se stalo neco spatneho
                    if (val_read == 0){
                        for (i = 0; i < CLIENT_NUM; i++){
                            p = player_arr[i];
                            if(p->socket == fd){
                                // TODO make reconnect logic
                                p->socket = 0;
                                p->username = "";
//                                p->state = State_Machine::allowed_transition(p->state, Event::EV_DISC);
//                                p->disconnected = true;
                                break;
                            }
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
                                    name_in_use = false;
                                    for (i = 0; i < CLIENT_NUM; i++) {
                                        p = player_arr[i];
                                        if (login_name.empty()){
                                            // Empty login
                                            lobby->empty_login(fd);
                                            name_in_use = true;
                                            break;
                                        }
                                        else if (p->username == login_name) {
                                            if (p->disconnected) {
                                                // Reconnect attempt
//                                                p->state = State_Machine::allowed_transition(p->state, Event::EV_RECON);
//                                                p->disconnected = false;
                                                // TODO make reconnect logic
                                                lobby->reconnect(fd, login_name);
                                            }
                                            else {
                                                // Name already in use
                                                lobby->already_in_use(fd);
                                            }
                                            name_in_use = true;
                                            break;
                                        }
                                    }
                                    if (!name_in_use) {
                                        // Original name detected
                                        if (login_name.size() > NAME_LENGTH) {
                                            // Name not too long
                                            lobby->name_too_long(fd, NAME_LENGTH);
                                        }
                                        else if (login_name.find(Help::SPL) != string::npos
                                                or login_name.find(Help::END) != string::npos){
                                            // Forbidden chars
                                            lobby->forbidden_chars(fd);
                                        }
                                        else {
                                            // Correct name
                                            // Enter lobby or/and start game
                                            lobby_entered = false;
                                            for (i = 0; i < GAME_NUM; i++) {
                                                game = game_arr[i];
                                                if (!game->is_active) {
                                                    // Create player finally and login
                                                    player->username = login_name;
                                                    player->state = State_Machine::allowed_transition(player->state,Event::EV_LOGIN);
                                                    player->disconnected = false;
                                                    // Cmd::WAITING + Cmd::PLAY
                                                    game->enter_lobby(player);
                                                    lobby_entered = true;
                                                    break;
                                                }
                                            }
                                            if (!lobby_entered) {
                                                // No more available lobby
                                                lobby->lobby_full(fd);
                                            }
                                        }
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
                                        if (game->is_active and game->curr_p == player) {
                                            try {
                                                string card = data.at(1);
                                                int st_row = stoi(data.at(2));
                                                int st_col = stoi(data.at(3));
                                                int end_row = stoi(data.at(4));
                                                int end_col = stoi(data.at(5));
                                                bool valid_move = game->check_move(
                                                        card,st_row, st_col, end_row, end_col);
                                                if (valid_move) {
                                                    game->move_was_made(card, st_row, st_col, end_row, end_col);
                                                    if (!game->is_end()) {
                                                        game->shuffle_cards(card);
                                                        game->switch_players();
                                                    }
                                                    else {
                                                        game->game_over();
                                                    }
                                                }
                                                else {
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
                                        if (game->is_active and game->curr_p == player) {
                                            try {
                                                string card = data.at(1);
                                                bool valid_swap = game->check_pass();
                                                if (valid_swap) {
                                                    game->pass_was_made(card);
                                                    game->shuffle_cards(card);
                                                    game->switch_players();
                                                }
                                                else {
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
                            else if (cmd == "WRONG_DATA") {
                                cout << "Wrong data -> server do not know how to respond!" << endl;
                            }
                            else{
                                cout << "ERR: Unknown message in Server!"<< endl;
                            }
                        }
                        else{
                            cout << "ERR: Player connected but not found in array of players!" << endl;
                        }
                    }
                }
            }
        }
    }
}
