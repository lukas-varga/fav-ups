#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H


#include <string>

using namespace std;

class Player{
public:
    int socket;
    string username;
    bool disconnected = false;

    Player();
    Player(int socket, string username);
};


#endif //SERVER_PLAYER_H
