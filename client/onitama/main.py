import sys

import intro
import game
import network

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
    welcome_rcv = net.recv_data()
    print(welcome_rcv)

    intro.login(net)
    game.play(net)


"""
 Main
"""
if __name__ == "__main__":
    main()