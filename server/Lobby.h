#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include <string>
#include "Game.h"

using namespace std;
using namespace std::chrono;

class Lobby {
private:
    string send_text;
public:

    bool find_lobby(int GAME_NUM, Game * game_arr[], Player * player);
    void failed_because(int fd, string message);

    void ping_back(int fd);
    void keep_alive(int GAME_NUM, Game * game_arr[], int CLIENT_NUM, Player * player_arr[], fd_set & client_socks, double MAX_DISCONNECT, double MAX_REMOVE);
    void wrong_attempt(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks, int MAX_ATTEMPTS);

    void disconnect_partly(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks);
    void disconnect_completely(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks);
    void close_connection(int fd, fd_set &client_socks);

    void finish_paused_game(int GAME_NUM, Game * game_arr[], Player * player);
    void reconnect(int GAME_NUM, Game * game_arr[], string username, int socket);
    void inform_disconnecting(int GAME_NUM, Game * game_arr[], string disc_username);
    void inform_reconnecting(int GAME_NUM, Game * game_arr[], string recon_username);


};


#endif //SERVER_LOBBY_H
