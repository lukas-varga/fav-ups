#include "Lobby.h"
#include "Command.h"
#include "Help.h"


Lobby::Lobby() {
    snd = "";
}

void Lobby::wrong_args(int fd) {
    // Wrong number of arguments
    snd = "";
    snd.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Wrong num of args!")
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);
}

void Lobby::lobby_full(int fd) {
    snd = "";
    snd.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("All lobby are full!")
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);
}

void Lobby::forbidden_chars(int fd) {
    snd = "";
    snd.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name include chars (|, \\0)")
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);

}

void Lobby::name_too_long(int fd, int NAME_LENGTH) {
    snd = "";
    snd.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name is too long (>" + to_string(NAME_LENGTH) + ")!")
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);
}

void Lobby::empty_login(int fd) {
    snd = "";
    snd.append(Command::name(Cmd::FAILED_LOGIN))
            .append(1, Help::SPL)
            .append("Name must no be empty!")
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);
}

void Lobby::reconnect(int fd, string login_name) {
    snd = "";
    snd.append(Command::name(Cmd::RECONNECT))
            .append(1, Help::SPL)
            .append("Reconnecting: " + login_name)
            .append(1, Help::END);
    send(fd, snd.data(), snd.size(), 0);
    Help::snd_log(fd, snd);
    cout << "Reconnect attempt!" << endl;

}

