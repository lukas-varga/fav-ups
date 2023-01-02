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
        print("Please enter client arguments: <ip> <port>")
        exit()

    try:
        ip = str(sys.argv[1])
        port = int(sys.argv[2])
        if port < 0 or port > 65535:
            raise Exception("Port out of bounds!")
    except Exception as e:
        print(e)
        print("Not valid IP and port were provided!")
        exit()

    # Make socket
    net = network.Network(ip, port)
    # Create connection
    net.create_connection()
    # Login screen
    intro.login(net)
    # Close connection
    net.close_connection()
    # Exit the program itself
    exit()


"""
 Main
"""
if __name__ == "__main__":
    main()
