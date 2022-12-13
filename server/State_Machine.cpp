#include "State_Machine.h"

/**
 * Vrací hodnotu v matici, výsledek
 * @param state Aktuální stav
 * @param event Událost, která se děje
 * @return Vrací hodnotu mez 0-5
 */
State_Machine allowed_transition(State_Machine state, Event event) {
    return transitions[tuple<State_Machine,Event>(state, event)];
};
