#include <algorithm>
#include "Parser.h"
#include "Command.h"

// Codes for delimiter and end

const char Parser::SPL = '|';
const char Parser::END = '\0';

vector<string> Parser::parse(const string& msg) {
    stringstream ss(msg);
    string word;
    vector<string> args{};

    while (!ss.eof()) {
        getline(ss, word, SPL);

        // Cleaning the username of white spaces before and after
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace),
                   word.end());
        // Cleaning \n
        string new_word;
        for (char ch : word){
            if (ch != '\n'){
                new_word.append(1, ch);
            }
        }

        args.push_back(new_word);
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

        // Valid message
        return args;
    }
    catch (const exception &exc){
        cerr << exc.what() << endl;
        cout << "Not defined message received from client!" << endl;

        args = {"WRONG_DATA"};
        return args;
    }
}

void Parser::send_log(int fd, const string& snd){
    cout << "Sending to fd(" << fd << "): "  << snd << endl;
}

void Parser::recv_log(int fd, char buff[]) {
    cout << "Receiving from fd(" << fd << "): " << buff << endl;
}
