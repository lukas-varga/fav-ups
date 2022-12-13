#ifndef SERVER_GAME_H
#define SERVER_GAME_H


#include "Player.h"

using namespace std;

class Game{
public:
    static int GAME_COUNTER;
    int game_id;
    Player p1;
    Player p2;

    Game();
    Game(Player p1, Player p2);
};


#endif //SERVER_GAME_H
