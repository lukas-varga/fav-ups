#include "Game.h"
#include "Player.h"
#include "Command.h"
#include "Parser.h"
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
#include <chrono>
#include <algorithm>


using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Please enter server arguments: <port> <num_players>" << endl;
        exit(0);
    }

    int port, num_players;
    try {
        port = stoi(argv[1]);
        num_players = stoi(argv[2]);
        if (num_players < 2) {
            throw exception();
        }
        if (port < 0 or port > 65535) {
            throw exception();
        }
    }
    catch (const exception &exc) {
        cerr << exc.what() << endl;
        cout << "Num of players must be >2 and port must be in (0, 65535) -> Exiting" << endl;
        exit(0);
    }

    // Limits for players
    const int NAME_LENGTH = 10;
    const int CLIENT_NUM = num_players;
    const int GAME_NUM = CLIENT_NUM / 2;

    // Buffer sizes
    const int MAX_INPUT = 255;
    const int MAX_BUFF = 1024;

    // Number of wrong attempts before disconnect
    int MAX_ATTEMPTS = 5;
    // Milliseconds for disconnect 20s
    const double MAX_DISCONNECT = 20000;
    // Milliseconds for removing 100s
    const double MAX_REMOVE = 100000;
    // Timeout for Select
    timeval timeout{};

    //  Socket initials
    int client_socket;
    int server_socket;
    int ret_val, fd;
    socklen_t client_len, server_len;
    sockaddr_in my_addr{}, peer_addr{};
    fd_set client_socks, tests;

    // Pointers
    Game *game_arr[GAME_NUM];
    Player *player_arr[CLIENT_NUM];
    auto *lobby = new Lobby();

    for (int i = 0; i < CLIENT_NUM; i++) {
        player_arr[i] = new Player(0, "");
    }
    for (int i = 0; i < GAME_NUM; i++) {
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
    if (ret_val == 0) {
        printf("Listen - OK\n");
    } else {
        printf("Listen - ERR\n");
        return -1;
    }

    // vyprazdnime sadu deskriptoru a vlozime server sock
    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);

    while (true) {
        char buff[MAX_BUFF];

        string rcv, snd;
        vector<string> data;
        string cmd, login_name;

        Player *player;
        Game *game;

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        auto a2read = 0;
        int i;

        tests = client_socks;
//        printf("Server is waiting... ");

        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
//        ret_val = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
        ret_val = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, &timeout);
        if (ret_val < 0) {
            cout << "Select - ERR" << endl;
            return -1;
        }

        // vynechavame stdin, stdout, stderr
        for (fd = 3; fd < FD_SETSIZE; fd++) {
            // je dany sock v sade fd ze kterych lze cist ?
            if (FD_ISSET(fd, &tests)) {
                // Je to server sock ? prijmeme nove spojeni
                if (fd == server_socket) {
                    client_len = sizeof(peer_addr);

                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &client_len);
                    FD_SET(client_socket, &client_socks);
                    printf("New connection - sock fd is %d\n", client_socket);

                    // send new connection greeting welcome_msg
//                    snd = "";
//                    snd.append("Welcome to Onitama server!");
//                    snd.append(1, Parser::END);
//                    send(client_socket, snd.data(), snd.size(), 0);
//                    Parser::send_log(client_socket, snd);

                    Player *p;
                    // Looking for empty Player slot
                    for (i = 0; i < CLIENT_NUM; i++) {
                        p = player_arr[i];
                        if (p->sock == 0 and p->user.empty()) {
                            p->init();
                            p->sock = client_socket;
                            printf("Adding to list of sockets at index %d\n", i);
                            break;
                        }
                    }
                }

// _____________________________________________________________________________________________________________________

                // Je to klientsky sock ? prijmem rcv
                else {
                    player = nullptr;
                    for (i = 0; i < CLIENT_NUM; i++) {
                        if (fd == player_arr[i]->sock) {
                            player = player_arr[i];
                            break;
                        }
                    }

                    // Player found in arr
                    if (player != nullptr) {
                        // pocet bajtu co je pripraveno ke cteni
                        ioctl(fd, FIONREAD, &a2read);

                        // Mame co cist
                        if (a2read > 0 and a2read < MAX_INPUT) {
                            // Alokace pameti pro cteci buffer
                            memset(buff, 0, MAX_BUFF);

                            if (recv(fd, buff, sizeof(buff), 0) <= 0) {
                                cout << "No data came to server!" << endl;
                            }

                            // Append data to string buffer
                            rcv = "";
                            rcv.append(buff);
                            Parser::recv_log(fd, buff);

                            // Parsing
                            data = Parser::parse(rcv);
                            cmd = data.at(0);

                            // Update last message when PING or other COMMAND
                            if(Command::is_enum(cmd)){
                                player->update_last_message();
                            }

                            // Message handling
                            // PING | name
                            if (cmd == Command::name(Cmd::PING)){
                                // Username can be empty (not logged yet)
                                lobby->ping_back(fd);
                            }
                            // LOGIN | name
                            else if (cmd == Command::name(Cmd::LOGIN)) {
                                if (data.size() == 1 + 1) {
                                    cout << "Entering: " << Command::name(Cmd::LOGIN) << endl;

                                    // Cleaning the username of white spaces before and after
                                    login_name = data.at(1);
                                    login_name.erase(std::remove_if(login_name.begin(), login_name.end(), ::isspace),
                                                     login_name.end());

                                    if (!login_name.empty()) {
                                        bool name_in_use = false;
                                        bool playing = false;

                                        // Check for playing
                                        for (i = 0; i < GAME_NUM; ++i) {
                                            game = game_arr[i];
                                            if (game->is_active) {
                                                // Remove player login but take sock number for game
                                                if (game->black_p->user == login_name and
                                                        game->black_p->disc) {
                                                    game->black_p->sock = player->sock;
                                                    game->black_p->disc = false;
                                                    game->black_p->state = State_Machine::allowed_transition(
                                                            game->black_p->state, Event::EV_RECON);
                                                    game->black_p->update_last_message();

                                                    lobby->reconnect(GAME_NUM, game_arr, login_name, player->sock);
                                                    lobby->inform_reconnecting(GAME_NUM, game_arr, login_name);
                                                    player->init();
                                                    playing = true;
                                                    break;
                                                }
                                                    // Remove player login but take sock number for game
                                                else if (game->white_p->user == login_name and
                                                         game->white_p->disc) {
                                                    game->white_p->sock = player->sock;
                                                    game->white_p->disc = false;
                                                    game->white_p->state = State_Machine::allowed_transition(
                                                            game->white_p->state, Event::EV_RECON);
                                                    game->white_p->update_last_message();

                                                    lobby->reconnect(GAME_NUM, game_arr, login_name,
                                                                     player->sock);
                                                    lobby->inform_reconnecting(GAME_NUM, game_arr, login_name);
                                                    player->init();
                                                    playing = true;
                                                    break;
                                                }

                                                // Player already in game and not disc so cannot login again
                                                else if (game->white_p->sock == player->sock or
                                                         game->black_p->sock == player->sock) {
                                                    lobby->failed_because(fd, "Socket already in game!");
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                    playing = true;
                                                    break;
                                                }
                                            }
                                        }

                                        // Not playing try to login as new user
                                        if (!playing) {
                                            for (i = 0; i < CLIENT_NUM; ++i) {
                                                Player *p = player_arr[i];
                                                // Name used, try different one
                                                if (p->user == login_name) {
                                                    lobby->failed_because(fd, "Name is already in use!");
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                    name_in_use = true;
                                                }
                                            }

                                            // Original name detected and not playing
                                            if (!name_in_use) {
                                                // Name not too long
                                                if (login_name.size() > NAME_LENGTH) {
                                                    lobby->failed_because(fd,
                                                                          "Name is too long (>" +
                                                                          to_string(NAME_LENGTH) +
                                                                          ")!");
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                }
                                                    // Forbidden chars
                                                else if (login_name.find(Parser::SPL) != string::npos
                                                         or login_name.find(Parser::END) != string::npos) {
                                                    lobby->failed_because(fd, "Name include chars | or \\0)");
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                }
                                                    // Correct name
                                                else {
                                                    player->user = login_name;
                                                    player->state = State_Machine::allowed_transition(player->state,Event::EV_LOGIN);

                                                    cout << "Name correct and in lobby!" << endl;
                                                    bool found = lobby->find_lobby(GAME_NUM, game_arr, player);
                                                    if (!found) {
                                                        lobby->failed_because(fd, "All lobby are full!");
                                                        lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                    }
                                                }
                                            }
                                        }
                                    } else {
                                        lobby->failed_because(fd, "Name must no be empty!");
                                        lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                    }
                                } else {
                                    lobby->failed_because(fd, "Wrong num of args!");
                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                }
                            } else if (cmd == Command::name(Cmd::MAKE_MOVE)) {
                                if (data.size() == 5 + 1) {
                                    cout << "Entering: " << Command::name(Cmd::MAKE_MOVE) << endl;
                                    for (i = 0; i < GAME_NUM; i++) {
                                        game = game_arr[i];
                                        if (game->is_active and (game->black_p == player or game->white_p == player)) {
                                            if (game->curr_p == player
                                                and !game->black_p->disc
                                                and !game->white_p->disc) {
                                                try {
                                                    string card = data.at(1);
                                                    int st_row = stoi(data.at(2));
                                                    int st_col = stoi(data.at(3));
                                                    int end_row = stoi(data.at(4));
                                                    int end_col = stoi(data.at(5));
                                                    bool valid_move = game->check_move(
                                                            card, st_row, st_col, end_row, end_col);
                                                    if (valid_move) {
                                                        player->state = State_Machine::allowed_transition(player->state,
                                                                                                          Event::EV_MOVE);
                                                        game->move_was_made(card, st_row, st_col, end_row, end_col);
                                                        if (!game->is_end()) {
                                                            game->shuffle_cards(card);
                                                            game->switch_players();
                                                        } else {
                                                            game->game_over();
                                                            cout << "Refresh / Reset the game so anyone can enter again!" << endl;
                                                            game->init();
                                                        }
                                                    } else {
                                                        player->state = State_Machine::allowed_transition(player->state,
                                                                                                          Event::EV_INVALID);
                                                        game->invalid_move("Move is not allowed!", fd);
                                                        lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                    }
                                                }
                                                catch (const exception &exc) {
                                                    cerr << exc.what() << endl;
                                                    game->invalid_move("Move is not parsable!", fd);
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                }
                                                break;
                                            } else {
                                                game->invalid_move("Not your turn!", fd);
                                            }
                                        }
                                    }
                                } else {
                                    lobby->failed_because(fd, "Wrong num of args!");
                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                }
                            } else if (cmd == Command::name(Cmd::MAKE_PASS)) {
                                if (data.size() == 1 + 1) {
                                    cout << "Entering: " << Command::name(Cmd::MAKE_PASS) << endl;
                                    for (i = 0; i < GAME_NUM; i++) {
                                        game = game_arr[i];
                                        if (game->is_active and (game->black_p == player or game->white_p == player)) {
                                            if (game->curr_p == player
                                                and !game->black_p->disc
                                                and !game->white_p->disc) {
                                                try {
                                                    string card = data.at(1);
                                                    bool valid_swap = game->valid_pass();
                                                    if (valid_swap) {
                                                        player->state = State_Machine::allowed_transition(player->state,
                                                                                                          Event::EV_MOVE);
                                                        game->pass_was_made(card);
                                                        game->shuffle_cards(card);
                                                        game->switch_players();
                                                    } else {
                                                        player->state = State_Machine::allowed_transition(player->state,
                                                                                                          Event::EV_INVALID);
                                                        game->invalid_move("Pass is not allowed!", fd);
                                                        lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                    }
                                                }
                                                catch (const exception &exc) {
                                                    cerr << exc.what() << endl;
                                                    game->invalid_move("Pass is not parsable!", fd);
                                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                                }
                                                break;
                                            } else {
                                                game->invalid_move("Not your turn!", fd);
                                            }
                                        }
                                    }
                                } else {
                                    lobby->failed_because(fd, "Wrong num of args!");
                                    lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                                }
                            } else if (cmd == "WRONG_DATA") {
                                cout << "ERR: Wrong data -> server do not know how to respond!" << endl;
                                lobby->failed_because(fd, "Message is not parsable!");
                                lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                            } else {
                                cout << "ERR: Unknown message in Server!" << endl;
                                lobby->failed_because(fd, "Message is not parsable!");
                                lobby->wrong_attempt(GAME_NUM, game_arr, player, client_socks, MAX_ATTEMPTS);
                            }
                        }

// _____________________________________________________________________________________________________________________


                        // Message is too long -> disconnect completely
                        else if (a2read > MAX_INPUT) {
                            lobby->disconnect_completely(GAME_NUM, game_arr, player, client_socks);
                        }
                        // Na socketu se stalo neco spatneho -> disconnect partly with reconnect
                        else {
                            lobby->disconnect_partly(GAME_NUM, game_arr, player, client_socks);
                        }
                    }
                }
            }
        } // FD for cycle

        //odpojeni vsech klientu, kteri porusili protokol nebo jsou timed out
        lobby->keep_alive(GAME_NUM, game_arr, CLIENT_NUM, player_arr, client_socks, MAX_DISCONNECT, MAX_REMOVE);
    }
}
