#ifndef SERVER_CMD_H
#define SERVER_CMD_H

#include <string>


using namespace std;

class Cmd{
public:
    // Client side
    static const string LOGIN;
    static const string MAKE_MOVE;
    static const string STALEMATE;
    // Server side
    static const string WAITING;
    static const string FAILED_LOGIN;
    static const string START;
    static const string MOVE_WAS_MADE;
    static const string INVALID_MOVE;
    static const string STALEMATE_CARD;
    static const string RECONNECT;
    static const string LOGOUT;
    static const string GAME_OVER;
};



#endif //SERVER_CMD_H
