#include "Lobby.h"
#include "Command.h"
#include "Help.h"


Lobby::Lobby() {
    send_text = "";
}

bool Lobby::find_lobby(int GAME_NUM, Game * game_arr[], Player * player) {
    for (int i = 0; i < GAME_NUM; i++) {
        Game * game = game_arr[i];
        // Enter lobby or/and start game
        if (!game->is_active) {
            // Cmd::WAITING + Cmd::PLAY
            game->enter_game(player);
            return true;
        }
    }
    return false;
}

//RECONNECT is_player_white | white_to_move | black_p | white_p | (5x) cards | (25x) "wP" "wK" "--"
void Lobby::reconnect(int GAME_NUM, Game * game_arr[], Player * player, int fd) {
    int i, row, col, last_piece;
    Game * g = nullptr;
    bool is_white;
    string is_player_white, white_to_move, black_p, white_p;
    vector<string> cards_vec;
    vector<string> board_vec;

    for (i = 0; i < GAME_NUM; ++i){
        if(game_arr[i]->is_active){
            if(game_arr[i]->black_p == player){
                g = game_arr[i];
                is_white = false;
                break;
            }
            else if(game_arr[i]->white_p == player){
                is_white = true;
                g = game_arr[i];
                break;
            }
        }
    }

    if (g != nullptr){
        is_player_white = (is_white) ? "1" : "0";
        white_to_move = (g->white_to_move) ? "1" : "0";
        black_p = g->black_p->username;
        white_p = g->white_p->username;

        // Pick cards first BLACK SPARE WHITE
        for (i = 0; i < g->five_cards.size(); ++i){
            cards_vec.push_back(g->five_cards[i]);
        }

        for (row = 0; row < g->board.size(); ++row){
            for (col = 0; col < g->board[row].size(); ++col){
                board_vec.push_back(g->board[row][col]);
            }
        }

        send_text = "";
        send_text.append(Command::name(Cmd::RECONNECT))
                .append(1, Help::SPL)
                .append(is_player_white)
                .append(1, Help::SPL)
                .append(white_to_move)
                .append(1, Help::SPL)
                .append(black_p)
                .append(1, Help::SPL)
                .append(white_p)
                .append(1, Help::SPL);
        for (const string& card : cards_vec){
            send_text.append(card)
                    .append(1, Help::SPL);
        }
        last_piece = board_vec.size() - 1;
        for (i = 0; i < last_piece; ++i){
            send_text.append(board_vec[i])
                    .append(1, Help::SPL);
        }
        send_text.append(board_vec[last_piece])
                .append(1, Help::END);

        send(fd, send_text.data(), send_text.size(), 0);
        Help::send_log(fd, send_text);

        player->disconnected = false;
        player->state = State_Machine::allowed_transition(player->state, Event::EV_RECON);
        cout << "Reconnect attempt was send!" << endl;
    }
    else{
        cout << "ERR: Reconnected player not found!" << endl;
    }
}

// Error messages while LOGIN and REMATCH

void Lobby::lobby_full(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("All lobby are full!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::forbidden_chars(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name include chars | or \\0)")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);

}

void Lobby::name_too_long(int fd, int NAME_LENGTH) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name is too long (>" + to_string(NAME_LENGTH) + ")!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::empty_login(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name must no be empty!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::already_in_use(int fd) {
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name is already in use!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::rematch_name(int fd) {
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Rematch name does not match!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}



