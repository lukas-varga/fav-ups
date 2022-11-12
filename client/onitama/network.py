"""
Server socket for TCP communication with C server
"""
import socket

"""
Socket operating class for communication with server
"""
class Network:
    def __init__(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # For this to work on your machine this must be equal to the ipv4 address of the machine running the server
        # You can find this address by typing ipconfig in CMD and copying the ipv4 address. Again this must be the
        # servers ipv4 address. This field will be the same for all your clients.
        self.host = "localhost"
        self.port = 10000
        self.addr = (self.host, self.port)
        self.id = self.connect()

    """
    Connect to server
    """
    def connect(self):
        try:
            self.client.connect(self.addr)
            print("Connected!")
            return 0
        except socket.error as e:
            print(e)
            return -1

    """
    Send data to server and wait for reply (FREEZE)
    """
    def send(self, data):
        """
        :param data: str
        :return: str
        """
        try:
            self.client.send(str.encode(data))
            reply = self.client.recv(2048).decode()
            return reply
        except socket.error as e:
            return str(e)
