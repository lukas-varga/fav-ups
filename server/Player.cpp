#include "Player.h"

Player::Player() {
    this->socket = 0;
    this->username = "";
}
Player::Player(int socket, std::string username) {
    this->socket = socket;
    this->username = std::move(username);
}




