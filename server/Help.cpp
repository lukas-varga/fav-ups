#include "Help.h"

// Codes for delimiter and end
const string Help::SPL = "|";
const string Help::END = "\0";

vector<string> Help::parse(const string& msg, char del) {
    stringstream ss(msg);
    string word;
    vector<string> args {};

    while (!ss.eof()) {
        getline(ss, word, del);
        args.push_back(word);
    }

    return args;
}

void Help::send_data(int fd, const string& snd){
    send(fd, snd.data(), snd.size(), 0);
    cout << "Sending to fd " << fd << ": "  << snd << endl;
}