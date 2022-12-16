#include "Command.h"


string Command::name(Cmd cmd) {
    map<Cmd,string> enum_map =  map<Cmd,string>{
            // Client side
            {Cmd::LOGIN ,          "LOGIN"},
            {Cmd::MAKE_MOVE ,      "MAKE_MOVE"},
            {Cmd::STALEMATE ,      "STALEMATE"},
            // Server side
            {Cmd::WRONG_DATA ,     "WRONG_DATA"},
            {Cmd::FAILED_LOGIN ,   "FAILED_LOGIN"},
            {Cmd::WAITING ,        "WAITING"},
            {Cmd::START ,          "START"},
            {Cmd::MOVE_WAS_MADE ,  "MOVE_WAS_MADE"},
            {Cmd::INVALID_MOVE ,   "INVALID_MOVE"},
            {Cmd::STALEMATE_CARD , "STALEMATE_CARD"},
            {Cmd::RECONNECT ,      "RECONNECT"},
            {Cmd::LOGOUT ,         "LOGOUT"},
            {Cmd::GAME_OVER ,      "GAME_OVER"},
    };

    return enum_map[cmd];
}

bool Command::is_enum(const string& name) {
    for (int fooInt = Cmd::LOGIN; fooInt != Cmd::GAME_OVER; fooInt++ ) {
        Cmd foo = static_cast<Cmd>(fooInt);
        if (Command::name(foo) == name){
            return true;
        }
    }
    return false;
}
