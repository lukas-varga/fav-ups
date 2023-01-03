#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "State.h"

#include <chrono>
#include <string>


using namespace std;
using namespace std::chrono;

class Player{
public:
    int sock;
    string user;

    bool disc;
    State state;

    int wrong_ct;
    time_point<chrono::system_clock> last_mess;

    Player(int sock, string name);
    void init();
    void inc_wrong_attempt();
    void update_last_message();

};


#endif //SERVER_PLAYER_H
