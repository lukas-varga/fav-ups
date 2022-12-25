#include "Help.h"
#include "Command.h"

// Codes for delimiter and end

const string Help::SPL = "|";
const string Help::END = "\0";

vector<string> Help::parse(const string& msg, char del) {
    stringstream ss(msg);
    string word;
    vector<string> args{};

    while (!ss.eof()) {
        getline(ss, word, del);
        args.push_back(word);
    }

    // Check validity of message
    try{
        if (args.size() < 2){
            cout << "Command is does not parsable!" << endl;
            throw exception();
        }
        string cmd = args.at(0);
        if (!Command::is_enum(cmd)){
            cout << "Command not found!" << endl;
            throw exception();
        }
    }
    catch (const exception &exc){
        args = {"WRONG_DATA"};
        cout << "Not defined message received from client!" << endl;
        cerr << exc.what();
    }

    // Valid message
    return args;
}

void Help::snd_log(int fd, const string& snd){
    cout << "Sending to fd(" << fd << "): "  << snd << endl;
}

void Help::rcv_log(int fd, const string& snd) {
    cout << "Receiving from fd(" << fd << "): " << snd << endl;
}
