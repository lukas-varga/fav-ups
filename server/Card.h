#ifndef SERVER_CARD_H
#define SERVER_CARD_H

#include <vector>
#include <string>
#include <map>
#include <random>
#include <algorithm>

// Standard namespace
using namespace std;

/*
 * Representing cards using map. Key is string name of card.
 * Value is vector/array of x,y coordinates regarding current position.
 */
class Card {
public:
    // Map that includes cards with coordinates
    static map<string, vector<tuple<int, int>>> card_map;

    // Get all card names
    static vector<string> card_names();
    // Randomly pick 5 cards from the whole deck and return
    static vector<string> pick_five_cards();
    // Get all possible coordinates to selected card. Inverted for black player (180 degree)
    static vector<tuple<int, int>> get_positions(const string& card, bool inverted_for_black);
    // Return true, if given string is valid card name, otherwise false
    static bool is_valid_card(const string& card);

};

#endif //SERVER_CARD_H
