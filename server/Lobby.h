#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include <string>
#include "Game.h"

using namespace std;

class Lobby {
private:
    string send_text;
public:
    Lobby();

    bool find_lobby(int GAME_NUM, Game * game_arr[], Player * player);
    void failed_because(int fd, string message);
    void attempt_disconnect(int GAME_NUM, Game * game_arr[], Player * player, int fd, fd_set & client_socks);

    void reconnect(int GAME_NUM, Game * game_arr[], string username, int socket);
    void inform_disconnecting(int GAME_NUM, Game * game_arr[], string disc_username);
    void inform_reconnecting(int GAME_NUM, Game * game_arr[], string recon_username);
};


#endif //SERVER_LOBBY_H
