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
#include "Cmd.h"
#include "Help.h"
#include "State.h"


using namespace std;

int main (int argc, char** argv){
    if (argc != 2){
        cout << "Please enter arguments: <port>" << "\n";
        exit(0);
    }
    int port = atoi(argv[1]);

    const int MAX_CLIENTS = 20;
    const int MAX_BUFF = 1024;
    char buff[MAX_BUFF];
    const char SPL_CHR = '|';

    Game game_arr[MAX_CLIENTS / 2];
    Game game = Game();

    Player player_arr[MAX_CLIENTS];
    int server_socket;
    int client_socket;
    int ret_val, val_read;
    int fd, sd;

    socklen_t client_len, server_len;
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        player_arr[i] = Player(0, "");
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

    // MAX_CLIENTS devices connected to chat
    ret_val = listen(server_socket, MAX_CLIENTS);
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
        Player p;
        State trans;

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
                    Help::send_data(client_socket, snd);

                    for (int i = 0; i < MAX_CLIENTS; i++)
                    {
                        p = player_arr[i];
                        if(p.socket == 0)
                        {
                            p.socket = client_socket;
                            printf("Adding to list of sockets as %d\n" , i);
                            break;
                        }
                    }
                }
                // je to klientsky socket ? prijmem rcv
                else {
                    val_read = recv(fd, buff, sizeof(buff), 0);
                    rcv.append(buff);
                    cout << "Receiving from fd " << fd << ": " << buff << endl;

                    // na socketu se stalo neco spatneho
                    if (val_read == 0){
                        for (int i = 0; i < MAX_CLIENTS; i++){
                            p = player_arr[i];
                            if(p.socket == fd){
                                p.socket = 0;
                                p.username = "";
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
                        vector<string> rcv_arr = Help::parse(rcv, SPL_CHR);
                        string cmd = rcv_arr.at(0);
                        string username = rcv_arr.at(1);

                        // login come to server
                        if (cmd == Cmd::LOGIN){
                            bool name_found = false;
                            for (int i = 0; i < MAX_CLIENTS; i++) {
                                p = player_arr[i];
                                if (p.username == username){
                                    name_found = true;
                                    // Reconnect attempt
                                    if(!p.disconnected){
                                        snd = "";
                                        snd = snd.append(Cmd::FAILED_LOGIN)
                                                .append(Help::SPL)
                                                .append(username)
                                                .append(Help::END);
                                        Help::send_data(fd, snd);
                                    }
                                    // Name already in use
                                    else{
                                        snd = "";
                                        snd = snd.append(Cmd::RECONNECT)
                                                .append(Help::SPL)
                                                .append(username)
                                                .append(Help::END);
                                        Help::send_data(fd, snd);
                                    }
                                    break;
                                }
                            }
                            if (!name_found){
                                // Name not too long
                                if (username.size() > 20 || username.empty()){
                                    snd = "";
                                    snd = snd.append(Cmd::FAILED_LOGIN)
                                            .append(Help::SPL)
                                            .append(username)
                                            .append(Help::END);
                                    Help::send_data(fd, snd);
                                }
                                // Correct name
                                else {
                                    trans = StateMachine::allowed_transition(p.state, Event::EV_LOGIN);
                                    if (trans == State::ST_WAITING){
                                        p.state = trans;

                                        snd = "";
                                        snd = snd.append(Cmd::WAITING)
                                                .append(Help::SPL)
                                                .append(username)
                                                .append(Help::END);
                                        Help::send_data(fd, snd);

                                        if (game.p1.username.empty()){
                                            game.p1 = p;
                                        }
                                        else if (game.p2.username.empty()){
                                            game.p2 = p;
                                            // MAX game_id = 9
                                            if (game.game_id < MAX_CLIENTS / 2){
                                                game_arr[game.game_id] = game;
                                                game.start_game();
                                                // New game
                                                game = Game();
                                            }
                                            else{
                                                cout << "All lobby are full!" << endl;
                                            }
                                        }
                                    }
                                    else{
                                        cout << "Unhandled error while creating lobby!" << endl;
                                    }
                                }
                            }
                        }
                        else{
                            cout << "Not defined message received in server: " << buff << endl;
                        }
                    }
                }
            }
        }

    }

    close(server_socket);
    close(client_socket);
    exit(0);
}
