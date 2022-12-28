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
    intro.login(net)
    net.close_connection()
    exit()


"""
 Main
"""
if __name__ == "__main__":
    main()
