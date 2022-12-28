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
    void reconnect(int GAME_NUM, Game * game_arr[], Player * player, int fd);

    void rematch_name(int fd);
    void lobby_full(int fd);
    void forbidden_chars(int fd);
    void name_too_long(int fd, int NAME_LENGTH);
    void empty_login(int fd);
    void already_in_use(int fd);

};


#endif //SERVER_LOBBY_H
