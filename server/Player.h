#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "State.h"

#include <string>


using namespace std;

class Player{
public:
    int socket;
    string username;

    bool disconnected;
    State state;

    Player(int sock, string name);
    void init();
};


#endif //SERVER_PLAYER_H
