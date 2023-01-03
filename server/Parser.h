#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sstream>
#include <vector>
#include "string"


using namespace std;

class Parser {
public:
    // Codes for delimiter and end
    static const char SPL;
    static const char END;

    // A quick way to parse strings separated via any character delimiter
    static vector<string> parse(const string& msg);
    static void send_log(int fd, const string& snd);
    static void recv_log(int fd, char buff[]);
};


#endif //SERVER_PARSER_H
