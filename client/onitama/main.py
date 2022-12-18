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
    net = network.Network(ip, port)

    # Login screen
    code = intro.login(net)
    if code[0] == Cmd.START.value:
        print("Game started!")
        game.play(net)
    elif code[0] == "EXIT":
        print("User quit login!")
    else:
        print("Unexpected behaviour in main!")

    net.close_connection()
    exit()


"""
 Main
"""
if __name__ == "__main__":
    main()