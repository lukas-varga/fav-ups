#include "Game.h"

int Game::GAME_COUNTER = 0;

Game::Game() {}
Game::Game(Player p1, Player p2) {
    this->p1 = std::move(p1);
    this->p2 = std::move(p2);
    game_id = ++GAME_COUNTER;
}

