#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "State.h"

#include <string>


using namespace std;

class Player{
public:
    int socket;
    string username;
    State state;
    bool disconnected;

    Player();
    Player(int sock, string name);
};


#endif //SERVER_PLAYER_H
