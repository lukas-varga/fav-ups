#include "Command.h"

map<Cmd,string> Command::enum_map = map<Cmd,string>{
        // Client side
        {Cmd::LOGIN ,         "LOGIN"},
        {Cmd::MAKE_MOVE ,     "MAKE_MOVE"},
        {Cmd::MAKE_PASS ,     "MAKE_PASS"},
        // Server side
        {Cmd::WAITING ,       "WAITING"},
        {Cmd::FAILED ,        "FAILED"},
        {Cmd::START ,         "START"},
        {Cmd::MOVE_WAS_MADE , "MOVE_WAS_MADE"},
        {Cmd::PASS_WAS_MADE , "PASS_WAS_MADE"},
        {Cmd::INVALID_MOVE ,  "INVALID_MOVE"},
        {Cmd::DISCONNECT ,    "DISCONNECT"},
        {Cmd::RECONNECT ,     "RECONNECT"},
        {Cmd::GAME_OVER ,     "GAME_OVER"},
        {Cmd::PING,           "PING"},
        {Cmd::LAST,           "LAST"}
};

string Command::name(Cmd cmd) {
    return enum_map[cmd];
}

bool Command::is_enum(const string& name) {
    for (int i = Cmd::LOGIN; i != Cmd::LAST; i++ ) {
        Cmd cmd = static_cast<Cmd>(i);
        if (Command::name(cmd) == name){
            return true;
        }
    }
    return false;
}
