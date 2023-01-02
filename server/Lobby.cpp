#include "Lobby.h"
#include "Command.h"
#include "Help.h"


Lobby::Lobby() {
    send_text = "";
}

bool Lobby::find_lobby(int GAME_NUM, Game * game_arr[], Player * player) {
    Game * game;

    // Try to connect to room where some people are waiting
    for (int i = 0; i < GAME_NUM; ++i) {
        game = game_arr[i];
        // Enter lobby or/and start game
        if (!game->is_active and (game->black_p != nullptr or game->white_p != nullptr)) {
            // Cmd::WAITING + Cmd::PLAY
            // Start new game
            game->enter_game(player);
            return true;
        }
    }

    // Create new game room
    for (int i = 0; i < GAME_NUM; ++i) {
        game = game_arr[i];
        // Enter lobby or/and start game
        if (!game->is_active) {
            // Cmd::WAITING + Cmd::PLAY
            // Create Room
            game->enter_game(player);
            return true;
        }
    }
    return false;
}

//RECONNECT black_p | white_p | (5x) cards | is_player_white | white_to_move | (25x) "wP" "wK" "--"
void Lobby::reconnect(int GAME_NUM, Game * game_arr[], string username, int socket) {
    int i, row, col, last_piece;
    Game * game = nullptr;

    bool is_white;
    string is_player_white, white_to_move, black_p, white_p;
    vector<string> cards_vec;
    vector<string> board_vec;

    for (i = 0; i < GAME_NUM; ++i){
        if(game_arr[i]->is_active){
            if(game_arr[i]->black_p->username == username){
                game = game_arr[i];
                is_white = false;
                break;
            }
            else if(game_arr[i]->white_p->username == username){
                game = game_arr[i];
                is_white = true;
                break;
            }
        }
    }

    if (game != nullptr){
        is_player_white = (is_white) ? "1" : "0";
        white_to_move = (game->white_to_move) ? "1" : "0";
        black_p = game->black_p->username;
        white_p = game->white_p->username;

        // Pick cards first BLACK SPARE WHITE
        for (i = 0; i < game->five_cards.size(); ++i){
            cards_vec.push_back(game->five_cards[i]);
        }

        for (row = 0; row < game->board.size(); ++row){
            for (col = 0; col < game->board[row].size(); ++col){
                board_vec.push_back(game->board[row][col]);
            }
        }

        send_text = "";
        send_text.append(Command::name(Cmd::RECONNECT))
                .append(1, Help::SPL)
                .append(black_p)
                .append(1, Help::SPL)
                .append(white_p)
                .append(1, Help::SPL);
        for (const string& card : cards_vec){
            send_text.append(card)
                    .append(1, Help::SPL);
        }

        send_text.append(is_player_white)
                .append(1, Help::SPL)
                .append(white_to_move)
                .append(1, Help::SPL);

        last_piece = board_vec.size() - 1;
        for (i = 0; i < last_piece; ++i){
            send_text.append(board_vec[i])
                    .append(1, Help::SPL);
        }
        send_text.append(board_vec[last_piece])
                .append(1, Help::END);

        send(socket, send_text.data(), send_text.size(), 0);
        Help::send_log(socket, send_text);

        cout << "Reconnect attempt was send!" << endl;
    }
    else{
        cout << "ERR: Reconnected player not found!" << endl;
    }
}

void Lobby::inform_disconnecting(int GAME_NUM, Game * game_arr[], string disc_username) {
    Game * game;
    send_text = "";
    send_text.append(Command::name(Cmd::DISCONNECTED))
            .append(1, Help::SPL)
            .append(disc_username)
            .append(1, Help::END);

    for (int i = 0; i< GAME_NUM; ++i){
        game = game_arr[i];
        if(game->is_active){
            // Inform white disconnected_p that black disconnected
            if (game->black_p->username == disc_username){
                send(game->white_p->socket, send_text.data(), send_text.size(), 0);
                Help::send_log(game->white_p->socket, send_text);
                break;
            }
                // Inform black disconnected_p that white disconnected
            else if(game->white_p->username == disc_username){
                send(game->black_p->socket, send_text.data(), send_text.size(), 0);
                Help::send_log(game->black_p->socket, send_text);
                break;
            }
        }
    }
}

void Lobby::inform_reconnecting(int GAME_NUM, Game * game_arr[], string recon_username) {
    Game * game;
    send_text = "";
    send_text.append(Command::name(Cmd::RECONNECT))
            .append(1, Help::SPL)
            .append(recon_username)
            .append(1, Help::END);

    for (int i = 0; i< GAME_NUM; ++i) {
        game = game_arr[i];
        if(game->is_active) {
            // Inform white reconnected_p that black disconnected
            if (game->black_p->username == recon_username) {
                send(game->white_p->socket, send_text.data(), send_text.size(), 0);
                Help::send_log(game->white_p->socket, send_text);
                break;
            }
                // Inform black reconnected_p that white disconnected
            else if (game->white_p->username == recon_username) {
                send(game->black_p->socket, send_text.data(), send_text.size(), 0);
                Help::send_log(game->black_p->socket, send_text);
                break;
            }
        }
    }
}

// Error messages while LOGIN and REMATCH

void Lobby::lobby_full(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("All lobby are full!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::forbidden_chars(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Name include chars | or \\0)")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::name_too_long(int fd, int NAME_LENGTH) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Name is too long (>" + to_string(NAME_LENGTH) + ")!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::empty_login(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Name must no be empty!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::already_in_use(int fd) {
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Name is already in use!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::wrong_num_of_args(int fd) {
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Wrong num of args!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::not_parsable(int fd) {
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append("Message is not parsable!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}