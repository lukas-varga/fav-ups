#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include "Player.h"


using namespace std;

class Game{
public:
    static int GAME_COUNTER;
    int game_id;
    Player * white_p;
    Player * black_p;
    bool is_active;

    Player * curr_p;
    bool white_to_move;
    Player * winner_p;
    vector<vector<string>> board;
    vector<string> five_cards;
    string piece_moved;
    string piece_captured;

    Game();

    void enter_lobby(Player * player);
    void start_game();

    bool check_move(const string& card, int st_row, int st_col, int end_row, int end_col);
    void move_was_made(const string& card, int st_row, int st_col, int end_row, int end_col);
    void invalid_move();

    void shuffle_cards(string card);
    void switch_players();

    bool is_game_over();
    void end_game();

};


#endif //SERVER_GAME_H
