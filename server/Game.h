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
    string send_text;

    Game();
    void init();

    void enter_game(Player * player);
    void start_game();

    bool check_move(const string& card, int st_row, int st_col, int end_row, int end_col);
    void move_was_made(const string& card, int st_row, int st_col, int end_row, int end_col);

    bool check_pass();
    void pass_was_made(const string& card);

    void invalid_move();
    void move_not_parsable();

    void shuffle_cards(const string& card);
    void switch_players();

    bool is_end();
    void game_over();




};


#endif //SERVER_GAME_H
