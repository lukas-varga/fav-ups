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
        self.id = self.create_connection()

    """
    Connect to server
    """
    def create_connection(self):
        try:
            self.client.connect(self.addr)
            welcome_rcv = self.recv_data()
            print(welcome_rcv)
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
            return 0
        except socket.error as e:
            print(e)
            return -1

    """
    Send data to server
    """
    def send_data(self, data):
        try:
            print(f"Sending to server: {data}")
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
            mess = b_mess.decode("utf-8")
            print(f"Receiving from server: {mess}")
            return mess
        except socket.error as e:
            print(e)
            return -1
