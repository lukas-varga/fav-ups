#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "State.h"

#include <chrono>
#include <string>


// Standard namespace
using namespace std;
// Namespace for time
using namespace std::chrono;

/**
 * Entity represents each player as object with socket, name and other states.
 */
class Player{
public:
    // Client socket (0 by default)
    int sock;
    // Client username ("" by default)
    string user;

    // True if player is disconnected from the game, false otherwise
    bool disc;
    // Tells in which states the user is in currently.
    State state;

    // Hacker counter of wrong messages from the client
    int wrong_ct;
    // Last time when server communicated with the client either by TCP message or PING
    time_point<chrono::system_clock> last_mess;


    /**
     * Main constructor for creating player reference
     * @param sock player socket
     * @param name player username
     */
    Player(int sock, string name);

    /**
     * Reset reference to empty slot used by players
     */
    void init();

    /**
     * Called after client sent somehow wrong data. Hacker counter is incremented
     * and user is disconnected if he exceed the limit.
     */
    void inc_wrong_attempt();

    /**
     * Client communicated with server using PING or some TCP message so his last seen time is updated.
     */
    void update_last_message();
};


#endif //SERVER_PLAYER_H
