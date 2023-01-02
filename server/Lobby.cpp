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

void Lobby::attempt_disconnect(int GAME_NUM, Game * game_arr[], Player * player, int fd, fd_set & client_socks) {
    int i;
    Game * game;
    player->wrong_counter++;
    int NUM_OF_ATTEMPTS = 5;

    if(player->wrong_counter >= NUM_OF_ATTEMPTS){
        for (i=0; i<GAME_NUM; ++i){
            game = game_arr[i];
            if(game->is_active){
                if(game->black_p == player) {
                    game->winner_p = game->white_p;
                    game->game_over();
                    game->init();
                    cout << "Game reset!" << endl;
                }
                else if (game->white_p == player){
                    game->winner_p = game->black_p;
                    game->game_over();
                    game->init();
                    cout << "Game reset!" << endl;
                }
            }
        }

        cout << "Disconnect on 3 wrong attempts was made!" << endl;
        player->init();

        // Remove closed fd
        close(fd);
        FD_CLR(fd, &client_socks);
        printf("Removing client (3 wrong attempts) on fd %d\n", fd);
    }
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

void Lobby::failed_because(int fd, string message){
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Help::SPL)
            .append(message)
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}
