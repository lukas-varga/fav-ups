#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include "map"


using namespace std;

enum class State {
    ST_NOT_ALLOWED = 0,
    ST_INIT = 1,
    ST_WAITING = 2,
    ST_PLAYING = 3,
};

enum class Event {
    EV_LOGIN = 0,
    EV_PLAY = 1,
    EV_MOVE = 2,
    EV_INVALID = 3,
    EV_DISC = 4,
    EV_RECON = 5,
};

class State_Machine{
public:
    /**
     * Vrací hodnotu v matici, výsledek
     * @param state Aktuální stav
     * @param event Událost, která se děje
     * @return Vrací hodnotu mez 0-5
     */
    static State allowed_transition(State state, Event event);
};

#endif //SERVER_STATE_H
