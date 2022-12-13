#include "Player.h"

Player::Player() {}
Player::Player(int socket, std::string username) {
    this->socket = socket;
    this->username = std::move(username);
}




