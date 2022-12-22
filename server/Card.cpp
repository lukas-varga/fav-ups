#include "Card.h"


map<string, vector<tuple<int, int>>> Card::card_map = map<string, vector<tuple<int,int>>> {
        {"tiger"    ,       {{0, 2}, {0, -1}},                      },
        {"dragon"   ,       {{-2, 1}, {2, 1}, {-1, -1}, {1, -1}},   },
        {"frog"     ,       {{-1, 1}, {-2, 0}, {1, -1}},            },
        {"rabbit"   ,       {{1, 1}, {2, 0}, {-1, -1}},             },
        {"crab"     ,       {{0, 1}, {-2, 0}, {2, 0}},              },
        {"elephant" ,       {{-1, 1}, {1, 1}, {-1, 0}, {1, 0}},     },
        {"goose"    ,       {{-1, 1}, {-1, 0}, {1, 0}, {1, -1}},    },
        {"rooster"  ,       {{1, 1}, {-1, 0}, {1, 0}, {-1, -1}},    },
        {"monkey"   ,       {{-1, 1}, {1, 1}, {-1, -1}, {1, -1}},   },
        {"mantis"   ,       {{-1, 1}, {1, 1}, {0, -1}},             },
        {"horse"    ,       {{0, 1}, {-1, 0}, {0, -1}},             },
        {"ox"       ,       {{0, 1}, {1, 0}, {0, -1}},              },
        {"crane"    ,       {{0, 1}, {-1, -1}, {1, -1}},            },
        {"boar"     ,       {{0, 1}, {-1, 0}, {1, 0}},              },
        {"eel"      ,       {{-1, 1}, {1, 0}, {-1, -1}},            },
        {"cobra"    ,       {{1, 1}, {-1, 0}, {1, -1}},             },
};

vector<string> Card::card_names() {
    vector<string> res;
    for (auto const& element : card_map) {
        res.push_back(element.first);
    }
    return res;
}

vector<string> Card::pick_five_cards() {
    vector<string> five_cards;
    vector<string> all_names = card_names();

    auto rd = random_device{};
    auto rng = default_random_engine{rd()};
    shuffle(begin(all_names), end(all_names), rng);

    for (int i = 0; i < 5; i++) {
        string val = all_names.at(i);
        five_cards.push_back(val);
    }

    return five_cards;
}

vector<tuple<int, int>> Card::get_positions(const string& card){
    return card_map.at(card);
}

bool Card::is_valid_card(const string& card){
    if (card_map.find(card) == card_map.end()){
        return false;
    }
    return true;
}

