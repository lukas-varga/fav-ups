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

    """
    Connect to server
    """
    def create_connection(self):
        try:
            self.client.connect(self.addr)
            welcome_rcv = self.recv_data()
            # print(welcome_rcv)
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
            print("Disconnecting from server!")
            return 0
        except socket.error as e:
            print(e)
            return -1

    """
    Send data to server
    """
    def send_data(self, data):
        try:
            print(f"Sending: {data}")
            byte_mess = data.encode('utf-8')
            self.client.send(byte_mess)
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
            byte_mess = self.client.recv(self.MAX_BUFF)
            data = byte_mess.decode("utf-8")
            print(f"Receiving: {data}")
            return data
        except socket.error as e:
            print(e)
            return -1
