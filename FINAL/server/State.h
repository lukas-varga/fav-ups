#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include "map"


// Standard namespace
using namespace std;

/**
 * Tells us in which state the client currently is.
 */
enum class State {
    // Not allowed
    ST_NOT_ALLOWED = 0,
    // Default client state
    ST_INIT = 1,
    // Successfully logged in and waiting for game to start
    ST_WAITING = 2,
    // Client is playing in the game
    ST_PLAYING = 3,
};

/**
 * Event occurred by client.
 */
enum class Event {
    // User logged in
    EV_LOGIN = 0,
    // User started playing
    EV_PLAY = 1,
    // User moved with piece and card or by passing turn
    EV_MOVE = 2,
    // Invalid event occurred. User sent incorrect data.
    EV_INVALID = 3,
    // User disconnected with reconnect option
    EV_DISC = 4,
    // User reconnected to the paused game
    EV_RECON = 5,
};

/**
 * Transition between state to state using event. Tells us in which state client is. Used for debugging.
 */
class State_Machine{
public:
    /**
     * Using State-Event matrix, it tells us which state happened after the event.
     * @param state actual state
     * @param event event which occured
     * @return Returns the state after current operation
     */
    static State allowed_transition(State state, Event event);
};

#endif //SERVER_STATE_H
