import intro
import game
import network
from parser import Cmd

import sys


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
    start_arr, username = intro.login(net)
    if start_arr is None:
        print("User pressed Close button!")
    elif start_arr[0] == Cmd.START.value:
        print("Both players connected -> Game will start!")
        game.play(net, start_arr, username)
    else:
        print("Unexpected behaviour in main!")

    net.close_connection()
    exit()


"""
 Main
"""
if __name__ == "__main__":
    main()