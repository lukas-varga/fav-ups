#include "State.h"


/**
 * Vrací hodnotu v matici, výsledek
 * @param state Aktuální stav
 * @param event Událost, která se děje
 * @return Vrací hodnotu mez 0-5
 */
State StateMachine::allowed_transition(State state, Event event) {
    map<tuple<State,Event>, State> transition = {
        // Login and play
        {tuple<State, Event>(State::ST_INIT,Event::EV_LOGIN),     State::ST_WAITING},
        {tuple<State, Event>(State::ST_WAITING,Event::EV_PLAY),   State::ST_PLAYING},
        {tuple<State, Event>(State::ST_PLAYING,Event::EV_MOVE),   State::ST_PLAYING},

        // Wrong
        {tuple<State, Event>(State::ST_INIT,Event::EV_WRONG),     State::ST_INIT},
        {tuple<State, Event>(State::ST_WAITING,Event::EV_WRONG),  State::ST_WAITING},
        {tuple<State, Event>(State::ST_PLAYING,Event::EV_WRONG),  State::ST_PLAYING},

        // Disconnect
        {tuple<State, Event>(State::ST_INIT,Event::EV_DISC),      State::ST_INIT},
        {tuple<State, Event>(State::ST_WAITING,Event::EV_DISC),   State::ST_WAITING},
        {tuple<State, Event>(State::ST_PLAYING, Event::EV_DISC),  State::ST_PLAYING},

        // Reconnect
        {tuple<State, Event>(State::ST_INIT,Event::EV_RECON),     State::ST_INIT},
        {tuple<State, Event>(State::ST_WAITING,Event::EV_RECON),  State::ST_WAITING},
        {tuple<State, Event>(State::ST_PLAYING,Event::EV_RECON),  State::ST_PLAYING},
    };

    return transition[tuple<State,Event>(state, event)];
}
