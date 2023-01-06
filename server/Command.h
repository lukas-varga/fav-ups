#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include <string>
#include <map>

// Standard namespace
using namespace std;

/**
 * Includes all commands used in TCP protocol as enum. Some of them are send by client, others by server.
 */
enum Cmd {
    /**
     * Client side
     */
    // Try login with username or reconnect to open game
    LOGIN,
    // Send move for validation to server (piece + card)
    MAKE_MOVE,
    // Pass turn if there is no other option left (only card)
    MAKE_PASS,

    /**
     * Server side
     */
    // Waiting for other players to join in lobby/waiting room
    WAITING,
    // Failed login or reconnect. As second parameter there is error message
    FAILED,
    // Game starts. First player is black, second is white. Five cards are distributed (everyone sees them).
    START,
    // Provided move was correct (checked by server). Both players receive confirmation about new game state.
    MOVE_WAS_MADE,
    // User could not move in any way, so he passed the turn and switch the card. The other players is on turn.
    PASS_WAS_MADE,
    // Provided move was incorrect for some reason. The reason is showed in second parameter. Player plays again.
    INVALID_MOVE,
    // Game ended. Either by taking opponent king or moving my own king to opponent king's start square.
    GAME_OVER,
    // Reconnect to game occurred. Reconnected player get all the game state info. Paused player get only notification.
    RECONNECT,
    // Player was notified that opponent disconnected from the game. He can reconnect in certain amount of time.
    DISCONNECT,
    // Ping message which is sent by clients and server respond by pinging back. Occur every few seconds.
    PING,
    // Used only for method is is_enum (iterating though enums).
    LAST,
};

/**
 * Static shared class for using TCP commands across the server app.
 */
class Command{
public:
    // Map which converts Cmd commands to same string equivalents
    static map<Cmd,string> enum_map;

    /**
     * Get string name of command by providing command enum.
     * @param cmd command enum
     * @return string representation
     */
    static string name(Cmd cmd);

    /**
     * Checks if given command (string) is valid or not.
     * @param name command as string
     * @return true if valid Cmd, false otherwise
     */
    static bool is_enum(const string& name);
};


#endif //SERVER_COMMAND_H
