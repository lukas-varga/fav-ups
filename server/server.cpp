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

#include "Game.h"
#include "Player.h"
#include "Command.h"
#include "Help.h"
#include "State.h"


using namespace std;

static int server_socket;
void close_connections();

int main (int argc, char** argv){
    if (argc != 2){
        cout << "Please enter arguments: <port>" << endl;
        exit(0);
    }
    int port = atoi(argv[1]);
    atexit(close_connections);

    const int CLIENT_NUM = 20;
    const int GAME_NUM = CLIENT_NUM / 2;
    const int MAX_BUFF = 2048;
    char buff[MAX_BUFF];
    const char SPL_CHR = '|';

    int client_socket;
    int ret_val, val_read;
    int fd, sd;
    socklen_t client_len, server_len;
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    Game * game_arr[GAME_NUM];
    Player * player_arr[CLIENT_NUM];

    for (int i = 0; i < CLIENT_NUM; i++){
        player_arr[i] = new Player(0, "");
    }
    for (int i = 0; i < GAME_NUM; i++){
//        Player * p1 = player_arr[i * 2];
//        Player * p2 = player_arr[(i * 2)+ 1];
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
        Game * game;
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
                    snd = snd.append("Welcome to Onitama server!");
                    snd = snd.append(Help::END);
                    send(client_socket, snd.data(), snd.size(), 0);
                    Help::send_log(fd, snd);

                    for (int i = 0; i < CLIENT_NUM; i++){
                        Player * p = player_arr[i];
                        if(p->socket == 0 && p->username.empty())
                        {
                            p->socket = client_socket;
                            printf("Adding to list of sockets as %d\n" , i);
                            break;
                        }
                    }
                }
                // je to klientsky socket ? prijmem rcv
                else {
                    rcv = "";
                    val_read = recv(fd, buff, sizeof(buff), 0);
                    rcv.append(buff);
                    Help::recv_log(fd,rcv);

                    // na socketu se stalo neco spatneho
                    if (val_read == 0){
                        for (int i = 0; i < CLIENT_NUM; i++){
                            Player * p = player_arr[i];
                            if(p->socket == fd){
                                // TODO make reconnect logic
//                                p->socket = 0;
//                                p->username = "";

                                p->state = StateMachine::allowed_transition(p->state, Event::EV_DISC);
                                p->disconnected = true;

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
                        Player * player = nullptr;
                        for (int i = 0; i < CLIENT_NUM; i++) {
                            if (fd == player_arr[i]->socket){
                                player = player_arr[i];
                                break;
                            }
                        }

                        // Player found in arr
                        if (player != nullptr){
                            // Parsing
                            vector<string> rcv_arr = Help::parse(rcv, SPL_CHR);
                            string cmd = rcv_arr.at(0);

                            // Cmd::WRONG_DATE
                            if (cmd == Command::name(Cmd::WRONG_DATA)){
                                snd = "";
                                snd = snd.append(Command::name(Cmd::WRONG_DATA))
                                        .append(Help::END);
                                send(fd, snd.data(), snd.size(), 0);
                                Help::send_log(fd, snd);
                            }
                            // login come to server
                            else if (cmd == Command::name(Cmd::LOGIN)){
                                string client_username = rcv_arr.at(1);
                                name_in_use = false;
                                for (int i = 0; i < CLIENT_NUM; i++) {
                                    Player * p = player_arr[i];
                                    if (p->username == client_username){
                                        name_in_use = true;
                                        // Reconnect attempt
                                        if(p->disconnected){
                                            p->state = StateMachine::allowed_transition(p->state, Event::EV_RECON);
                                            p->disconnected = false;
                                            // TODO make reconnect logic

                                            snd = "";
                                            snd = snd.append(Command::name(Cmd::RECONNECT))
                                                    .append(Help::SPL)
                                                    .append("Reconnecting: "+client_username)
                                                    .append(Help::END);
                                            send(fd, snd.data(), snd.size(), 0);
                                            Help::send_log(fd, snd);
                                            cout << "Reconnect attempt!" << endl;
                                        }
                                        // Name already in use
                                        else{
                                            snd = "";
                                            snd = snd.append(Command::name(Cmd::FAILED_LOGIN))
                                                    .append(Help::SPL)
                                                    .append("Name already in use!")
                                                    .append(Help::END);
                                            send(fd, snd.data(), snd.size(), 0);
                                            Help::send_log(fd, snd);
                                        }
                                        break;
                                    }
                                }
                                // Original name detected
                                if (!name_in_use){
                                    // Name not too long
                                    if (client_username.size() > 20 || client_username.empty()){
                                        snd = "";
                                        snd = snd.append(Command::name(Cmd::FAILED_LOGIN))
                                                .append(Help::SPL)
                                                .append("Name is too long!")
                                                .append(Help::END);
                                        send(fd, snd.data(), snd.size(), 0);
                                        Help::send_log(fd, snd);
                                    }
                                    // Correct name --> Enter lobby
                                    else {
                                        // Enter lobby or/and start game
                                        lobby_entered = false;
                                        for (int i = 0; i < GAME_NUM; i++){
                                            game = game_arr[i];
                                            if(!game->is_active){
                                                // Create player finally and login
                                                player->username = client_username;
                                                player->state = StateMachine::allowed_transition(player->state, Event::EV_LOGIN);
                                                player->disconnected = false;

                                                // Cmd::WAITING
                                                // Cmd::PLAY
                                                game->enter_lobby(player);
                                                lobby_entered = true;
                                                cout << "Player logged correctly!" << endl;
                                                break;
                                            }
                                        }
                                        // No more available lobby
                                        if (!lobby_entered){
                                            snd = "";
                                            snd = snd.append(Command::name(Cmd::FAILED_LOGIN))
                                                    .append(Help::SPL)
                                                    .append("All lobby are full!")
                                                    .append(Help::END);
                                            send(fd, snd.data(), snd.size(), 0);
                                            Help::send_log(fd, snd);
                                        }
                                    }
                                }
                            }
                            else if (cmd == Command::name(Cmd::MAKE_MOVE)){

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

void close_connections(){
    close(server_socket);
}
