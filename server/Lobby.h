#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include <string>

using namespace std;

class Lobby {
private:
    string send_text;
public:
    Lobby();

    void lobby_full(int fd);
    void forbidden_chars(int fd);
    void name_too_long(int fd, int NAME_LENGTH);
    void empty_login(int fd);
    void reconnect(int fd, string login_name);
    void already_in_use(int fd);
};


#endif //SERVER_LOBBY_H
