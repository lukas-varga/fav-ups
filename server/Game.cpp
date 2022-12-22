#include "Game.h"
#include "Command.h"
#include "Help.h"
#include "Card.h"

int Game::GAME_COUNTER = -1;

Game::Game() {
    this->p1 = nullptr;
    this->p2 = nullptr;
    this->game_id = ++GAME_COUNTER;
    is_active = false;
}

Game::Game(Player *p1, Player *p2) {
    this->p1 = p1;
    this->p2 = p2;
    this->game_id = ++GAME_COUNTER;
    is_active = false;
}

void Game::enter_lobby(Player * player){
    if (this->p1 == nullptr){
        this->p1 = player;

        string snd = "";
        snd.append(Command::name(Cmd::WAITING))
                .append(Help::SPL)
                .append(player->username)
                .append(Help::END);
        send(player->socket, snd.data(), snd.size(), 0);
        Help::send_log(player->socket, snd);
        cout << "Player1 " << player->username << " has entered lobby!" << endl;
    }
    else if(this->p2 == nullptr){
        this->p2 = player;

        string snd = "";
        snd.append(Command::name(Cmd::WAITING))
                .append(Help::SPL)
                .append(player->username)
                .append(Help::END);
        send(player->socket, snd.data(), snd.size(), 0);
        Help::send_log(player->socket, snd);
        cout << "Player2 " << player->username << " has entered lobby!" << endl;

        // GAME started
        start_game();
    }

}

void Game::start_game(){
    is_active = true;
    // Pick 5 random cards
    vector<string> five_cards = Card::pick_five_cards();
    int last_index = five_cards.size() - 1;

    string snd = "";
    snd.append(Command::name(Cmd::START))
            .append(Help::SPL)
            .append(p1->username)
            .append(Help::SPL)
            .append(p2->username)
            .append(Help::SPL);

    for (int i = 0; i < last_index; ++i){
        snd.append(five_cards[i])
            .append(Help::SPL);
    }
    snd.append(five_cards[last_index])
        .append(Help::END);

    // Send to both TWO players
    send(p1->socket, snd.data(), snd.size(), 0);
    Help::send_log(p1->socket, snd);
    send(p2->socket, snd.data(), snd.size(), 0);
    Help::send_log(p2->socket, snd);

    cout << "Game started with id: " << game_id << endl;
    // TODO start game implement...
}





