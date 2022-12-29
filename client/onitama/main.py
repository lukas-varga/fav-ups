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

    # Make socket
    net = network.Network(ip, port)
    # Create connection
    net.create_connection()
    # Login screen
    intro.login(net)
    # Close connection
    net.close_connection()
    exit()


"""
 Main
"""
if __name__ == "__main__":
    main()
