#include "Game.h"
#include "Command.h"
#include "Help.h"
#include "Card.h"

int Game::GAME_COUNTER = -1;

Game::Game() {
    white_p = nullptr;
    black_p = nullptr;
    game_id = ++GAME_COUNTER;
    is_active = false;

    curr_p = nullptr;
    white_to_move = true;
    winner_p = nullptr;
    piece_moved = "";
    piece_captured = "";

    vector<vector<string>> init
    {
        {"bP", "bP", "bK", "bP", "bP"},
        {"--", "--", "--", "--", "--"},
        {"--", "--", "--", "--", "--"},
        {"--", "--", "--", "--", "--"},
        {"wP", "wP", "wK", "wP", "wP"},
    };
    board = init;
    five_cards = Card::pick_five_cards();
}


void Game::enter_lobby(Player * player){
    if (white_p == nullptr){
        white_p = player;

        string snd = "";
        snd.append(Command::name(Cmd::WAITING))
                .append(1, Help::SPL)
                .append(player->username)
                .append(1, Help::END);
        send(player->socket, snd.data(), snd.size(), 0);
        Help::snd_log(player->socket, snd);
        cout << "Player1 " << player->username << " has entered lobby!" << endl;
    }
    else if(black_p == nullptr){
        black_p = player;

        string snd = "";
        snd.append(Command::name(Cmd::WAITING))
                .append(1, Help::SPL)
                .append(player->username)
                .append(1, Help::END);
        send(player->socket, snd.data(), snd.size(), 0);
        Help::snd_log(player->socket, snd);
        cout << "Player2 " << player->username << " has entered lobby!" << endl;

        // GAME started
        start_game();
    }
}

void Game::start_game(){
    is_active = true;
    // Pick 5 random cards first BLACK SPARE WHITE
    int last_index = five_cards.size() - 1;

    string snd = "";
    snd.append(Command::name(Cmd::START))
            .append(1, Help::SPL)
            .append(black_p->username)
            .append(1, Help::SPL)
            .append(white_p->username)
            .append(1, Help::SPL);

    for (int i = 0; i < last_index; ++i){
        snd.append(five_cards[i])
            .append(1, Help::SPL);
    }
    snd.append(five_cards[last_index])
        .append(1, Help::END);

    // Send to both TWO players START
    send(black_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(black_p->socket, snd);
    send(white_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(white_p->socket, snd);

    white_to_move = true;
    curr_p = white_p;
    cout << "Game started with id: " << game_id << endl;
}

bool Game::check_move(const string& card, int st_row, int st_col, int end_row, int end_col){
    vector<tuple<int,int>> positions;
    int new_r, new_c, fst_card, snd_card;
    string piece_color;
    bool inverted;

    // Black player up there
    if (!white_to_move){
        inverted = true;
        fst_card = 0;
        snd_card = 1;
        piece_color = "b";
    }
    // White player down there
    else{
        inverted = false;
        fst_card = 3;
        snd_card = 4;
        piece_color = "w";
    }

    positions = Card::get_positions(card, inverted);
    // Using only black cards
    if(card == five_cards.at(fst_card) or card == five_cards.at(snd_card)){
        // Not outside of playing field
        if(end_row >= 0 and end_row <= 4 and end_col >= 0 and end_col <= 4){
            // Moving with our figures and not to friendly figures
            if (board[st_row][st_col].substr(0,1) == piece_color
            and board[end_row][end_col].substr(0,1) != piece_color){
                for (tuple<int, int> vec : positions) {
                    // Transform x,y system to board system
                    new_r = st_row - get<1>(vec);
                    new_c = st_col + get<0>(vec);
                    // Card and given position is right
                    if (new_r == end_row and new_c == end_col) {
                        piece_moved = board[st_row][st_col];
                        piece_captured = board[end_row][end_col];
                        board[st_row][st_col] = "--";
                        board[end_row][end_col] = piece_moved;
                        cout << "Move was OK!" << endl;
                        return true;
                    }
                }
            }
        }
    }

    cout << "Move was invalid for some reason!" << endl;
    return false;
}

void Game::move_was_made(const string& card, int st_row, int st_col, int end_row, int end_col) {
    string snd = "";
    snd.append(Command::name(Cmd::MOVE_WAS_MADE))
            .append(1, Help::SPL)
            .append(card)
            .append(1, Help::SPL)
            .append(to_string(st_row))
            .append(1, Help::SPL)
            .append(to_string(st_col))
            .append(1, Help::SPL)
            .append(to_string(end_row))
            .append(1, Help::SPL)
            .append(to_string(end_col))
            .append(1, Help::END);

    send(black_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(black_p->socket, snd);
    send(white_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(white_p->socket, snd);
}

void Game::invalid_move() {
    string snd = "";
    snd.append(Command::name(Cmd::INVALID_MOVE))
            .append(1, Help::SPL)
            .append("Move is not possible!")
            .append(1, Help::END);
    send(curr_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(curr_p->socket, snd);
}

void Game::move_not_parsable() {
    string snd = "";
    snd.append(Command::name(Cmd::INVALID_MOVE))
            .append(1, Help::SPL)
            .append("Number of row or col is not integer OR card is to string!")
            .append(1, Help::END);
    send(curr_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(curr_p->socket, snd);
}

void Game::shuffle_cards(string card) {
    // Black player and White Player
    string tmp;
    for (int i = 0; i < five_cards.size(); ++i){
        if (i == 2){
            continue;
        }
        else if (five_cards[i] == card){
            tmp = five_cards[i];
            five_cards[i] = five_cards[2];
            five_cards[2] = tmp;
            cout << "Cards swapped: " << tmp << " -> " << five_cards[i] << endl;
            break;
        }
    }
}

void Game::switch_players() {
    white_to_move = !white_to_move;

    if (curr_p == black_p){
        curr_p = white_p;
    }
    else{
        curr_p = black_p;
    }
    cout << "Players switched!" << endl;
}

bool Game::is_game_over() {
    // Checkmate || Enemy base for white || Enemy base for black
    if (piece_captured.substr(1, 1) == "K" or board[0][2] == "wK" or board[4][2] == "bK"){
        winner_p = curr_p;
        cout << "Game over encountered!" << endl;
        return true;
    }
    cout << "Not game over yet!" << endl;
    return false;
}

void Game::end_game() {
    string snd = "";
    snd.append(Command::name(Cmd::GAME_OVER))
            .append(1, Help::SPL)
            .append(winner_p->username)
            .append(1, Help::END);
    send(white_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(white_p->socket, snd);
    send(black_p->socket, snd.data(), snd.size(), 0);
    Help::snd_log(black_p->socket, snd);

    cout << "End of the game" << endl;
    //TODO send logout
}







