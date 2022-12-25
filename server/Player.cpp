#include "Player.h"

Player::Player() {
    socket = 0;
    username = "";
    state = State::ST_INIT;
    disconnected = false;
}

Player::Player(int sock, string name) {
    socket = sock;
    username = std::move(name);
    state = State::ST_INIT;
    disconnected = false;
}




