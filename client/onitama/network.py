"""
Network class which encapsulate socket communication
"""
import socket

"""
Socket operating class for communication with server
"""
class Network(object):
    def __init__(self, ip, port):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # For this to work on your machine this must be equal to the ipv4 address of the machine running the server
        # You can find this address by typing ipconfig in CMD and copying the ipv4 address. Again this must be the
        # servers ipv4 address. This field will be the same for all your clients.
        self.MAX_BUFF = 1024
        self.host = ip
        self.port = port
        self.addr = (self.host, self.port)
        self.id = self.connect()

    """
    Connect to server
    """
    def connect(self):
        try:
            self.client.connect(self.addr)
            # hand_shake = self.recv_data()
            # print(hand_shake)
            return 0
        except socket.error as e:
            print(e)
            return -1

    """
    Close the connection to server
    """
    def close_connection(self):
        try:
            self.client.close()
        except socket.error as e:
            print(e)
            return -1

    """
    Send data to server
    """
    def send_data(self, data):
        try:
            message = data.encode('utf-8')
            self.client.send(message)
            return 0
        except socket.error as e:
            print(e)
            return -1

    """
    Receive data from server while waiting
    :return str data
    """
    def recv_data(self):
        try:
            b_mess = self.client.recv(self.MAX_BUFF)
            return b_mess.decode("utf-8")
        except socket.error as e:
            print(e)
            return -1
