#include "Game.h"
#include "Cmd.h"
#include "Help.h"

int Game::GAME_COUNTER = -1;

Game::Game() {
    this->p1 = Player();
    this->p2 = Player();
    game_id = ++GAME_COUNTER;
}

void Game::start_game() const {
    string snd = "";
    snd = snd.append(Cmd::START)
            .append(Help::SPL)
            .append(p1.username)
            .append(Help::SPL)
            .append(p2.username)
            .append(Help::END);
    Help::send_data(p1.socket, snd);
    Help::send_data(p2.socket, snd);
}