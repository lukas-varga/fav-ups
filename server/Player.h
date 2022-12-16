#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string>
#include "State.h"


using namespace std;

class Player{
public:
    int socket;
    string username;
    State state;
    bool disconnected;

    Player();
    Player(int socket, string username);
};


#endif //SERVER_PLAYER_H
