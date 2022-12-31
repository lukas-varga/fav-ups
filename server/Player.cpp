#include "Player.h"

Player::Player(int sock, string name) {
    socket = sock;
    username = std::move(name);

    disconnected = false;
    state = State::ST_INIT;
}




