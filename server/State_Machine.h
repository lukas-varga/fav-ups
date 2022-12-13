#ifndef SERVER_STATE_MACHINE_H
#define SERVER_STATE_MACHINE_H

#pragma once
#include "map"

using namespace std;

enum class State_Machine {
    ST_NOT_ALLOWED = 0,
    ST_INIT = 1,
    ST_WAITING = 2,
    ST_PLAYING = 3
};

enum class Event {
    EV_LOGIN = 0,
    EV_PLAY = 1,
    EV_MOVE = 2,
    EV_WRONG = 3,
};

map<tuple<State_Machine,Event>, State_Machine> transitions = {
    {tuple<State_Machine,Event>(State_Machine::ST_INIT, Event::EV_LOGIN), State_Machine::ST_INIT},
    {tuple<State_Machine,Event>(State_Machine::ST_WAITING, Event::EV_PLAY), State_Machine::ST_PLAYING},
    {tuple<State_Machine,Event>(State_Machine::ST_PLAYING, Event::EV_MOVE), State_Machine::ST_PLAYING}
};

#endif //SERVER_STATE_MACHINE_H
