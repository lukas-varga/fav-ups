#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string>
#include "State.h"


using namespace std;

class Player{
public:
    int socket;
    string username;
    bool disconnected = false;
    State state = State::ST_INIT;

    Player();
    Player(int socket, string username);
};


#endif //SERVER_PLAYER_H
