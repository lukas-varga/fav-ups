#include "Player.h"

Player::Player(int sock, string name) {
    socket = sock;
    username = name;

    disconnected = false;
    state = State::ST_INIT;

    wrong_counter = 0;
}

void Player::init() {
    socket = 0;
    username = "";

    disconnected = false;
    state = State::ST_INIT;

    wrong_counter = 0;
}





