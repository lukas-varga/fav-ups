#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include <string>
#include "Game.h"

// Standard namespace
using namespace std;
// Namespace for time
using namespace std::chrono;

/**
 * Represent entity which handles incoming messages from clients in logical way.
 */
class Lobby {
public:
    /**
     * If username is accepted and original, he can enter empty game (or join some waiting opponent).
     * Then he waits there till this game room is full (2 people).
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param player reference of entering player
     * @return Return tree if game lobby was found, false otherwise (no lobby available).
     */
    bool find_lobby(int GAME_NUM, Game * game_arr[], Player * player);
    /**
     * Send information that general operation (mainly in LOGIN) was not correct for given reason.
     * @param fd player's socket
     * @param message detailed message why operation failed
     */
    void failed_because(int fd, string message);


    /**
     * After server got ping message from client, he immediately responds with ping back.
     * @param fd player's socket
     */
    void ping_back(int fd);

    /**
     * After every iteration of file descriptor set, there is a control whether some clients exceeded timeout limits.
     * This timeout is linked to last ping information from particular client.
     * When the client did not respond for certain amount of time which equals first timeout, he is disconnected
     * and active game is pause (in waiting room or without login client is removed completely).
     * When the client is no responding for the time greater than second timeout,
     * he is completely removed from the game and paused opponent won the game.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param CLIENT_NUM total number of client (2 * GAME_NUM)
     * @param player_arr array with references to all player (even empty slots)
     * @param client_socks set of client sockets for communication
     * @param MAX_DISCONNECT first timeout for disconnecting player
     * @param MAX_REMOVE second timeout for removing the player
     */
    void keep_alive(int GAME_NUM, Game * game_arr[], int CLIENT_NUM, Player * player_arr[], fd_set & client_socks, double MAX_DISCONNECT, double MAX_REMOVE);

    /**
     * If player send not valid data to server several times, he is considered hacker and is disconnected.
     * The same applies when he send message which is too large for the protocol.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param player reference of entering player
     * @param client_socks set of client sockets for communication
     * @param MAX_ATTEMPTS maximal number of attempts before client is disconnected
     */
    void wrong_attempt(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks, int MAX_ATTEMPTS);


    /**
     * Player is disconnected with possibility to reconnect either after reaching first timeout
     * or by accidental closing the game window.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param player reference of entering player
     * @param client_socks set of client sockets for communication
     */
    void disconnect_partly(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks);

    /**
     * Player is disconnected and removed from the current game completely after he reaches the second timeout.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param player reference of entering player
     * @param client_socks set of client sockets for communication
     */
    void disconnect_completely(int GAME_NUM, Game * game_arr[], Player * player, fd_set & client_socks);

    /**
     * Closing connection with particular client using his file descriptor socket.
     * @param fd client socket
     * @param client_socks set of all client sockets
     */
    void close_connection(int fd, fd_set &client_socks);

    /**
     * After the player is removed completely, game ends and opponent wins.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param player reference of entering player
     */
    void finish_paused_game(int GAME_NUM, Game * game_arr[], Player * player);


    /**
     * After player disconnects, he has certain amount of time for reconnecting to the paused game.
     * If he does so and log in with the same username, he is sent data for recover the game state
     * and he is able to continue playing.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param username
     * @param socket
     */
    void reconnect(int GAME_NUM, Game * game_arr[], string username, int socket);

    /**
     * Opponent is informed about other player disconnecting. The game is paused and can be resumed,
     * when disconnected player reconnect before he reached second timeout.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param disc_username
     */
    void inform_disconnecting(int GAME_NUM, Game * game_arr[], string disc_username);

    /**
     * After disconnected player reconnects to the paused game,
     * opponent is informed about this event, so game can continue.
     * @param GAME_NUM total number of games available
     * @param game_arr array with references to all games (including empty games)
     * @param recon_username
     */
    void inform_reconnecting(int GAME_NUM, Game * game_arr[], string recon_username);
};


#endif //SERVER_LOBBY_H
