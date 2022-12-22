#ifndef SERVER_CARD_H
#define SERVER_CARD_H

#include <vector>
#include <string>
#include <map>
#include <random>
#include <algorithm>


using namespace std;

class Card {
public:
    static map<string, vector<tuple<int, int>>> card_map;

    static vector<string> card_names();
    static vector<string> pick_five_cards();
    static vector<tuple<int, int>> get_positions(const string& card);
    static bool is_valid_card(const string& card);

};

#endif //SERVER_CARD_H
