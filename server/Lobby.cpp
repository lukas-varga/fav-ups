#include "Lobby.h"
#include "Command.h"
#include "Parser.h"


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


void Lobby::failed_because(int fd, string message){
    string send_text;
    send_text.append(Command::name(Cmd::FAILED))
            .append(1, Parser::SPL)
            .append(message)
            .append(1, Parser::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Parser::send_log(fd, send_text);
}


void Lobby::ping_back(int fd) {
    string send_text;
    send_text.append(Command::name(Cmd::PING))
            .append(1, Parser::SPL)
            .append(Command::name(Cmd::PING))
            .append(1, Parser::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Parser::send_log(fd, send_text);
}

void Lobby::keep_alive(int GAME_NUM, Game * game_arr[], int CLIENT_NUM, Player * player_arr[], fd_set & client_socks, double MAX_DISCONNECT, double MAX_REMOVE) {
    cout << "Keep alive check..." << endl;
    Player * p;
    auto currentTime = system_clock::now();

    // Players with sockets different from 0
    for (int i=0; i < CLIENT_NUM; ++i) {
        p = player_arr[i];
        auto durationMillis = duration<double, milli>(currentTime - p->last_mess).count();

        if(p->sock != 0) {
            if (durationMillis > MAX_DISCONNECT) {
                cerr << "Player " << p->user << " with sock " << p->sock
                     << " reached 1st timeout -> disconnect!" << endl;
                disconnect_partly(GAME_NUM, game_arr, p, client_socks);
            }

            if (durationMillis > MAX_REMOVE) {
                cerr << "Player " << p->user << " reached 2nd timeout -> remove!" << endl;
                disconnect_completely(GAME_NUM, game_arr, p, client_socks);
            }
        }
    }

    // Players with disconnect status and user filled but ohne Socket
    for (int i=0; i < CLIENT_NUM; ++i) {
        p = player_arr[i];
        auto durationMillis = duration<double, milli>(currentTime - p->last_mess).count();

        if(p->disc and !p->user.empty()){
            if (durationMillis > MAX_REMOVE) {
                cerr << "Paused player " << p->user << " reached 2nd timeout -> remove!" << endl;
                finish_paused_game(GAME_NUM, game_arr, p);
            }
        }
    }
}

void Lobby::wrong_attempt(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks, int MAX_ATTEMPTS) {
    // Increment wrong attempt by one
    player->inc_wrong_attempt();

    // If overflow -> disconnect completely
    if(player->wrong_ct >= MAX_ATTEMPTS){
        disconnect_completely(GAME_NUM, game_arr, player, client_socks);
    }
}


void Lobby::disconnect_partly(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks){
    cout << "Disconnecting partly!" << endl;

    int fd = player->sock;
    int i;
    Game * game;

    // Not logged player -> Delete Player Record completely
    if (player->user.empty()) {
        cout << "User with empty login closed!" << endl;
        player->init();
    }
    // Already logged
    else {
        for (i = 0; i < GAME_NUM; ++i) {
            game = game_arr[i];
            // In Game -> Remember NAME for reconnect but forgets sock
            if (game->is_active and (game->white_p == player or game->black_p == player)) {
                // Second player is already disc
                if (game->black_p->disc or game->white_p->disc) {
                    game->black_p->init();
                    game->white_p->init();
                    game->init();
                    cout << "Both players logged out and game finished!" << endl;
                }
                // Remember the Name - Fort Minor
                else {
                    player->sock = 0;
                    // player -> SAME USERNAME
                    player->disc = true;
                    player->state = State_Machine::allowed_transition(player->state,Event::EV_DISC);
                    inform_disconnecting(GAME_NUM, game_arr, player->user);
                    cout << "Player " << player->user << " has unfinished game. Can reconnect with user!" << endl;
                }
            }
            // Waiting room -> Delete Player Record completely and REMOVE the GAME
            else if (!game->is_active and
                     (game->white_p == player or game->black_p == player)) {
                player->init();
                game->init();
                cout << "Player logged out and Room closed!" << endl;
            }
        }
    }
    close_connection(fd,client_socks);
}

void Lobby::disconnect_completely(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks) {
    cout << "Disconnecting completely!" << endl;
    int fd = player->sock;

    finish_paused_game(GAME_NUM, game_arr, player);
    close_connection(fd,client_socks);
}

void Lobby::finish_paused_game(int GAME_NUM, Game * game_arr[], Player * player) {
    int i;
    Game * game;

    // End games which are unfinished
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

    // Clear player in memory
    player->init();
}


void Lobby::close_connection(int fd, fd_set & client_socks){
    // Remove closed fd
    // (from the unmodified fd_set readfds)
    close(fd);
    FD_CLR(fd, &client_socks);
    printf("Removing client on fd %d\n", fd);
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
            if(game_arr[i]->black_p->user == username){
                game = game_arr[i];
                is_white = false;
                break;
            }
            else if(game_arr[i]->white_p->user == username){
                game = game_arr[i];
                is_white = true;
                break;
            }
        }
    }

    if (game != nullptr){
        is_player_white = (is_white) ? "1" : "0";
        white_to_move = (game->white_to_move) ? "1" : "0";
        black_p = game->black_p->user;
        white_p = game->white_p->user;

        // Pick cards first BLACK SPARE WHITE
        for (i = 0; i < game->five_cards.size(); ++i){
            cards_vec.push_back(game->five_cards[i]);
        }

        for (row = 0; row < game->board.size(); ++row){
            for (col = 0; col < game->board[row].size(); ++col){
                board_vec.push_back(game->board[row][col]);
            }
        }

        string send_text;
        send_text.append(Command::name(Cmd::RECONNECT))
                .append(1, Parser::SPL)
                .append(black_p)
                .append(1, Parser::SPL)
                .append(white_p)
                .append(1, Parser::SPL);
        for (const string& card : cards_vec){
            send_text.append(card)
                    .append(1, Parser::SPL);
        }

        send_text.append(is_player_white)
                .append(1, Parser::SPL)
                .append(white_to_move)
                .append(1, Parser::SPL);

        last_piece = board_vec.size() - 1;
        for (i = 0; i < last_piece; ++i){
            send_text.append(board_vec[i])
                    .append(1, Parser::SPL);
        }
        send_text.append(board_vec[last_piece])
                .append(1, Parser::END);

        send(socket, send_text.data(), send_text.size(), 0);
        Parser::send_log(socket, send_text);

        cout << "Reconnect attempt was send!" << endl;
    }
    else{
        cout << "ERR: Reconnected player not found!" << endl;
    }
}

void Lobby::inform_disconnecting(int GAME_NUM, Game * game_arr[], string disc_username) {
    Game * game;
    string send_text;
    send_text.append(Command::name(Cmd::DISCONNECT))
            .append(1, Parser::SPL)
            .append(disc_username)
            .append(1, Parser::END);

    for (int i = 0; i< GAME_NUM; ++i){
        game = game_arr[i];
        if(game->is_active){
            // Inform white disconnected_p that black disc
            if (game->black_p->user == disc_username){
                send(game->white_p->sock, send_text.data(), send_text.size(), 0);
                Parser::send_log(game->white_p->sock, send_text);
                break;
            }
                // Inform black disconnected_p that white disc
            else if(game->white_p->user == disc_username){
                send(game->black_p->sock, send_text.data(), send_text.size(), 0);
                Parser::send_log(game->black_p->sock, send_text);
                break;
            }
        }
    }
}

void Lobby::inform_reconnecting(int GAME_NUM, Game * game_arr[], string recon_username) {
    Game * game;
    string send_text;
    send_text.append(Command::name(Cmd::RECONNECT))
            .append(1, Parser::SPL)
            .append(recon_username)
            .append(1, Parser::END);

    for (int i = 0; i< GAME_NUM; ++i) {
        game = game_arr[i];
        if(game->is_active) {
            // Inform white reconnected_p that black disc
            if (game->black_p->user == recon_username) {
                send(game->white_p->sock, send_text.data(), send_text.size(), 0);
                Parser::send_log(game->white_p->sock, send_text);
                break;
            }
                // Inform black reconnected_p that white disc
            else if (game->white_p->user == recon_username) {
                send(game->black_p->sock, send_text.data(), send_text.size(), 0);
                Parser::send_log(game->black_p->sock, send_text);
                break;
            }
        }
    }
}





