#include "Help.h"
#include "Command.h"

// Codes for delimiter and end

const char Help::SPL = '|';
const char Help::END = '\0';

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
        cerr << exc.what();
        cout << "Not defined message received from client!" << endl;
    }

    // Valid message
    return args;
}

void Help::send_log(int fd, const string& snd){
    cout << "Sending to fd(" << fd << "): "  << snd << endl;
}

void Help::recv_log(int fd, char buff[]) {
    cout << "Receiving from fd(" << fd << "): " << buff << endl;
}
