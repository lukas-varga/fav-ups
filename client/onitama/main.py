import sys

import intro
import game
import network
from parser import Cmd

"""
Implementation of main method
"""
def main():
    if len(sys.argv) != 3:
        print("Please enter arguments: <ip> <port>")
        exit()
    ip = str(sys.argv[1])
    port = int(sys.argv[2])

    # Handshake
    net = network.Network(ip, port)
    code = intro.login(net)

    if code[0] == Cmd.START:
        print("Game started!")
        game.play(net)
    elif code[0] == "EXIT":
        print("User quit login!")
        exit()
    else:
        print("Unexpected behaviour in main!")
        exit()


"""
 Main
"""
if __name__ == "__main__":
    main()