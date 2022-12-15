#include "State.h"


/**
 * Vrací hodnotu v matici, výsledek
 * @param state Aktuální stav
 * @param event Událost, která se děje
 * @return Vrací hodnotu mez 0-5
 */
State StateMachine::allowed_transition(State state, Event event) {
    map<tuple<State,Event>, State> transition = {
        {tuple<State, Event>(State::ST_INIT,    Event::EV_LOGIN),   State::ST_WAITING},
        {tuple<State, Event>(State::ST_WAITING, Event::EV_PLAY),    State::ST_PLAYING},
        {tuple<State, Event>(State::ST_PLAYING, Event::EV_MOVE),    State::ST_PLAYING}
    };

    return transition[tuple<State,Event>(state, event)];
}
