#include "Command.h"

map<Cmd,string> Command::enum_map = map<Cmd,string>{
        // Client side
        {Cmd::LOGIN ,          "LOGIN"},
        {Cmd::MAKE_MOVE ,      "MAKE_MOVE"},
        {Cmd::MAKE_PASS ,      "MAKE_PASS"},
        // Server side
        {Cmd::FAILED_LOGIN ,   "FAILED_LOGIN"},
        {Cmd::WAITING ,        "WAITING"},
        {Cmd::START ,          "START"},
        {Cmd::MOVE_WAS_MADE ,  "MOVE_WAS_MADE"},
        {Cmd::INVALID_MOVE ,   "INVALID_MOVE"},
        {Cmd::PASS_WAS_MADE ,  "PASS_WAS_MADE"},
        {Cmd::RECONNECT ,      "RECONNECT"},
        {Cmd::LOGOUT ,         "LOGOUT"},
        {Cmd::GAME_OVER ,      "GAME_OVER"},
};

string Command::name(Cmd cmd) {
    return enum_map[cmd];
}

bool Command::is_enum(const string& name) {
    for (int i = Cmd::LOGIN; i != Cmd::GAME_OVER; i++ ) {
        Cmd cmd = static_cast<Cmd>(i);
        if (Command::name(cmd) == name){
            return true;
        }
    }
    return false;
}
