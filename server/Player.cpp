#include "Player.h"

Player::Player() {
    this->socket = 0;
    this->username = "";
    this->state = State::ST_INIT;
    this->disconnected = false;
}

Player::Player(int socket, string username) {
    this->socket = socket;
    this->username = std::move(username);
    this->state = State::ST_INIT;
    this->disconnected = false;
}




