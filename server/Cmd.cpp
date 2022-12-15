#include "Cmd.h"

// Client side
const string Cmd::LOGIN = "LOGIN";
const string Cmd::MAKE_MOVE = "MAKE_MOVE";
const string Cmd::STALEMATE = "STALEMATE";
// Server side
const string Cmd::WAITING = "WAITING";
const string Cmd::FAILED_LOGIN = "FAILED_LOGIN";
const string Cmd::START = "START";
const string Cmd::MOVE_WAS_MADE = "MOVE_WAS_MADE";
const string Cmd::INVALID_MOVE = "INVALID_MOVE";
const string Cmd::STALEMATE_CARD = "STALEMATE_CARD";
const string Cmd::RECONNECT = "RECONNECT";
const string Cmd::LOGOUT = "LOGOUT";
const string Cmd::GAME_OVER = "GAME_OVER";
