#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include <string>
#include <map>


using namespace std;

enum Cmd {
    // Client side
    LOGIN,
    MAKE_MOVE,
    MAKE_PASS,
    // Server side
    WAITING,
    FAILED,
    START,
    MOVE_WAS_MADE,
    PASS_WAS_MADE,
    INVALID_MOVE,
    GAME_OVER,
    RECONNECT,
    DISCONNECTED,
};

class Command{
public:
    static map<Cmd,string> enum_map;

    static string name(Cmd cmd);
    static bool is_enum(const string& name);
};


#endif //SERVER_COMMAND_H
