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


// Standard namespace
using namespace std;

/**
 * It handles parsing incoming message from socket using TCP enums.
 * It can tell whether incoming message can be processed by lobby class.
 */
class Parser {
public:
    // Char symbol of delimiter between parameters
    static const char SPL;
    // Char symbol of end of the whole message
    static const char END;

    /**
     * A quick way to parse strings separated by delimiter.
     * All the whitespace and newline symbols are removed for the sake of safety.
     * @param msg message received in socket communication from clients
     * @return Return vector of parsed string data if data seems valid. Otherwise it returns vector of {"WRONG_DATA"}.
     */
    static vector<string> parse(const string& msg);

    /**
     * For debugging purpose, all the messages sent to clients are printed to standard output.
     * @param fd client socket
     * @param snd sent message
     */
    static void send_log(int fd, const string& snd);

    /**
     * For debugging purpose, all the messages received by server are printed to standard output.
     * @param fd client socket
     * @param buff received message
     */
    static void recv_log(int fd, char buff[]);
};


#endif //SERVER_PARSER_H
