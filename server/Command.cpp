#include "Command.h"

// Client side
const string Command::LOGIN = "LOGIN";
const string Command::MAKE_MOVE = "MAKE_MOVE";
const string Command::STALEMATE = "STALEMATE";
// Server side
const string Command::WAITING = "WAITING";
const string Command::FAILED_LOGIN = "FAILED_LOGIN";
const string Command::START = "START";
const string Command::MOVE_WAS_MADE = "MOVE_WAS_MADE";
const string Command::INVALID_MOVE = "INVALID_MOVE";
const string Command::STALEMATE_CARD = "STALEMATE_CARD";
const string Command::RECONNECT = "RECONNECT";
const string Command::LOGOUT = "LOGOUT";
const string Command::GAME_OVER = "GAME_OVER";
