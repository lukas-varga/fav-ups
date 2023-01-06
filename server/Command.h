#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include <string>
#include <map>

// Standard namespace
using namespace std;

/**
 * Includes all commands used in TCP protocol.
 * Some of them are send by client, others by server.
 */
enum Cmd {
    /**
     * Client side
     */
    // Try login with username
    LOGIN,
    // Send move to validating (piece + card)
    MAKE_MOVE,
    // Pass turn if possible (only card)
    MAKE_PASS,

    /**
     * Server side
     */
    WAITING,
    FAILED,
    START,
    MOVE_WAS_MADE,
    PASS_WAS_MADE,
    INVALID_MOVE,
    GAME_OVER,
    RECONNECT,
    DISCONNECT,
    PING,
    // Used only for method is is_enum (iterating though enums).
    LAST,
};

class Command{
public:
    static map<Cmd,string> enum_map;

    static string name(Cmd cmd);
    static bool is_enum(const string& name);
};


#endif //SERVER_COMMAND_H
