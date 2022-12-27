#include "Lobby.h"
#include "Command.h"
#include "Help.h"


Lobby::Lobby() {
    send_text = "";
}

void Lobby::lobby_full(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("All lobby are full!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::forbidden_chars(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name include chars | or \\0)")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);

}

void Lobby::name_too_long(int fd, int NAME_LENGTH) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name is too long (>" + to_string(NAME_LENGTH) + ")!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::empty_login(int fd) {
    send_text = "";
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name must no be empty!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

void Lobby::reconnect(int fd, string login_name) {
    send_text = "";
    send_text.append(Command::name(Cmd::RECONNECT))
            .append(1, Help::SPL)
            .append("Reconnecting: " + login_name)
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
    cout << "Reconnect attempt!" << endl;

}

void Lobby::already_in_use(int fd) {
    send_text.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name is already in use!")
            .append(1, Help::END);
    send(fd, send_text.data(), send_text.size(), 0);
    Help::send_log(fd, send_text);
}

