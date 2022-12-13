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

#include "Game.h"
#include "Player.h"
#include "Command.h"

using namespace std;

// A quick way to parse strings separated via any character delimiter
vector<string> parse(const string& msg, char del);

const string SPL = "|";
const string END = "\0";

int main (int argc, char** argv){
    if (argc != 3){
        cout << "Please enter arguments: <ip> <port>" << "\n";
        exit(0);
    }
    char* ip = argv[1];
    int port = atoi(argv[2]);

    const int MAX_CLIENTS = 20;
    const int MAX_BUFF = 1024;
    char buff[MAX_BUFF];
    const char SPL_CHR = '|';

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
    my_addr.sin_addr.s_addr = inet_addr(ip);

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
                    string welcome_msg = "Welcome to Onitama server!";
                    welcome_msg = welcome_msg.append(END);
                    send(client_socket, welcome_msg.data(), welcome_msg.size(), 0) ;
                    printf("Welcome message sent successfully\n");

                    for (int i = 0; i < MAX_CLIENTS; i++)
                    {
                        p = player_arr[i];
                        if(p.socket == 0 )
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
                        vector<string> rcv_arr = parse(rcv, SPL_CHR);
                        string cmd = rcv_arr.at(0);
                        string user_login = rcv_arr.at(1);

                        // login come to server
                        if (cmd == Command::LOGIN){
                            bool name_found = false;
                            for (int i = 0; i < MAX_CLIENTS; i++) {
                                p = player_arr[i];
                                if (p.username == user_login){
                                    name_found = true;
                                    // Reconnect attempt
                                    if(!p.disconnected){
                                        snd = snd.append(Command::FAILED_LOGIN)
                                                .append(SPL)
                                                .append(user_login)
                                                .append(END);
                                        send(fd, snd.data(), snd.size(), 0);
                                        cout << "fd: " << fd << " -> " << Command::FAILED_LOGIN << endl;
                                    }
                                    // Name already in use
                                    else{
                                        snd = snd.append(Command::RECONNECT)
                                                .append(SPL)
                                                .append(user_login)
                                                .append(END);
                                        send(fd, snd.data(), snd.size(), 0);
                                        cout << "fd: " << fd << " -> " << Command::RECONNECT << endl;
                                    }
                                    break;
                                }
                            }
                            if (!name_found){
                                // Name not too long
                                if (user_login.size() > 20 || user_login.empty()){
                                    snd = snd.append(Command::FAILED_LOGIN)
                                            .append(SPL)
                                            .append(user_login)
                                            .append(END);
                                    send(fd, rcv.data(), rcv.size(), 0);
                                    cout << "fd: " << fd << " -> " << Command::FAILED_LOGIN << endl;
                                }
                                // Correct name
                                else {
                                    snd = snd.append(Command::WAITING)
                                            .append(SPL)
                                            .append(user_login)
                                            .append(END);
                                    send(fd, snd.data(), snd.size(), 0);
                                    cout << "fd: " << fd << " -> " << Command::WAITING << " " << user_login << endl;

                                    // TODO Wait for second player
//                                    snd = snd.append(Command::START)
//                                            .append(SPL)
//                                            .append(user_login)
//                                            .append(SPL)
//                                            .append(user_login)
//                                            .append(END);
//                                    send(fd, snd.data(), snd.size(), 0);
//                                    cout << "fd: " << fd << " -> " << Command::START << " " << user_login << " " << user_login << endl;
                                }
                            }
                        }
                        else{
                            cout << "Unknown error" << endl;
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

vector<string> parse(const string& msg, char del) {
    stringstream ss(msg);
    string word;
    vector<string> args {};

    while (!ss.eof()) {
        getline(ss, word, del);
        args.push_back(word);
    }

    return args;
}