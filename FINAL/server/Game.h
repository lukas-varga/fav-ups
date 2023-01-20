#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include "Player.h"

// Standard namespace
using namespace std;

/**
 * Represent one particular game of two players. Includes several pointers describing game state.
 * Cards are represented by string vector and board 2D string vector
 */
class Game{
public:
    // Keeps track of how many games were created
    static int GAME_COUNTER;
    // ID number of the game
    int game_id;

    // Black player pointer
    Player * black_p;
    // White player pointer
    Player * white_p;
    // True if game is running, false otherwise
    bool is_active;
    // Pointer to current player (his turn)
    Player * curr_p;
    // True if white is playing now, false if black is playing
    bool white_to_move;
    // Pointer to player who has won the game
    Player * winner_p;

    // 2D array representing board with pieces (pawn/king/empty)
    vector<vector<string>> board;
    // Randomly selected cards (0,1 - Black; 2 - Spare; 3,4 - White)
    vector<string> five_cards;

    // Currently moved piece/figure
    string piece_moved;
    // Currently captured piece/figure
    string piece_captured;

    /**
     * Game constructor creates empty uninitialized game
     */
    Game();
    /**
     * Reset game to default state without destroying it
     */
    void init();

    /**
     * Given player enters them game. If there is already another person there, game starts immateriality.
     * @param player player entering game room
     */
    void enter_game(Player * player);

    /**
     * Called when game room is full. Prepare for the game and send start info to both players.
     */
    void start_game();

    /**
     * Check whether player's move is valid and return bool accordingly.
     * @param card given string card
     * @param st_row row of starting position
     * @param st_col col of starting position
     * @param end_row row of ending position
     * @param end_col col of ending position
     * @return Return true if move is correct, false otherwise.
     */
    bool check_move(const string& card, int st_row, int st_col, int end_row, int end_col);

    /**
     * Inform both players that provided move was correct.
     * Client can update game state from the data. Other player can play.
     * @param card given string card
     * @param st_row row of starting position
     * @param st_col col of starting position
     * @param end_row row of ending position
     * @param end_col col of ending position
     */
    void move_was_made(const string& card, int st_row, int st_col, int end_row, int end_col);

    /**
     * Provided pass request is correct.
     * @param card card to swap with spare card
     * @return true if valid, false otherwise
     */
    bool valid_pass(string card);

    /**
     * Inform players about the pass. Other player can play.
     * @param card card to swap with spare card.
     */
    void pass_was_made(const string& card);

    /**
     * Given move (or pass) violated rules or does not include valid data. Player play again without any change.
     * @param message message why player violated game rules
     * @param fd player's socket number
     */
    void invalid_move(string message, int fd);

    /**
     * After valid move/pass spare card needs to be swapped with currently played card
     * @param card card to swap with spare card
     */
    void shuffle_cards(const string& card);

    /**
     *  After valid move/pass the player ends his turn and the other player start his turn.
     */
    void switch_players();

    /**
     * Check win conditions and return accordingly.
     * @return Return true if game is over, false if players still plays.
     */
    bool is_end();

    /**
     * After game over event is encountered, game ends and both players needs to be informed about who won the game.
     */
    void game_over();
};


#endif //SERVER_GAME_H
