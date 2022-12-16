#ifndef SERVER_HELP_H
#define SERVER_HELP_H

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

class Help {
public:
    // Codes for delimiter and end
    static const string SPL;
    static const string END;

    // A quick way to parse strings separated via any character delimiter
    static vector<string> parse(const string& msg, char del);
    static void send_log(int fd, const string& snd);
    static void recv_log(int fd, const string& snd);
};


#endif //SERVER_HELP_H
