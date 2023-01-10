#include "Player.h"
#include <utility>
#include <iostream>

Player::Player(int sock, string name) {
    sock = sock;
    user = std::move(name);

    disc = false;
    state = State::ST_INIT;

    wrong_ct = 0;
    last_mess = system_clock::now();
}

void Player::init() {
    sock = 0;
    user = "";

    disc = false;
    state = State::ST_INIT;

    wrong_ct = 0;
    last_mess = system_clock::now();
}

void Player::inc_wrong_attempt(){
    wrong_ct++;
    cout << "Player " << user << " has wrong counter: " << wrong_ct << endl;
}

void Player::update_last_message() {
    last_mess = system_clock::now();
}
