#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include "Player.h"


using namespace std;

class Game{
public:
    static int GAME_COUNTER;
    int game_id;
    Player * p1;
    Player * p2;
    bool is_active;

    Game();
    Game(Player * p1, Player * p2);

    void enter_lobby(Player * player);
    void start_game();

};


#endif //SERVER_GAME_H
