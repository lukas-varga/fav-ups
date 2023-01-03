import select
import socket
from tkinter import messagebox
from time import time
import parser

"""
Socket operating class for communication with server
"""
class Network(object):
    def __init__(self, ip, port):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # For this to work on your machine this must be equal to the ipv4 address of the machine running the server
        # You can find this address by typing ipconfig in CMD and copying the ipv4 address. Again this must be the
        # servers ipv4 address. This field will be the same for all your clients.
        self.MAX_BUFF = 1024
        self.MAX_INPUT = 255
        self.host = ip
        self.port = port
        self.addr = (self.host, self.port)

        # Wrong messages
        self.NUM_OF_ATTEMPTS = 5
        self.wrong_counter = 0

        # Last time in millis
        self.last_mess = time()
        # Ping sent
        self.can_ping = True
        # Ping in seconds
        self.PING_TIME = 4
        # Ping in seconds
        self.MAX_TIMEOUT = 10

        # Not to block server
        # self.server.setblocking(False)

        # Blocking timeout for socket
        # self.server.settimeout(self.TIMEOUT)

    """
    Connect to server
    """
    def create_connection(self):
        try:
            self.server.connect(self.addr)
            print("Connected!")

            # welcome_rcv = self.recv_data()
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
            self.server.close()
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
            self.server.send(byte_mess)
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
            byte_mess = self.server.recv(self.MAX_BUFF)
            data = byte_mess.decode("utf-8")
            print(f"Receiving: {data}")
            return data
        except socket.error as e:
            print(e)
            return ""

    def allow_ping(self):
        self.last_mess = time()
        self.can_ping = True

    def send_ping(self):
        if self.can_ping and (time() - self.last_mess) > self.PING_TIME:
            ping = parser.prepare_ping()
            self.send_data(ping)
            self.can_ping = False

    def broken_connection(self):
        if (time() - self.last_mess) > self.MAX_TIMEOUT:
            return True
        return False

    """ 
    Read from the socket, stuffing data into the buffer.
    Returns True when a full packet has been read into the buffer
    """
    def network_data_arrived(self, socket_buffer):
        self.send_ping()

        result = False
        socks = [self.server]

        try:
            # tiny read-timeout
            in_, out_, exc_ = select.select(socks, [], [], 0)

            # has any data arrived?
            if in_.count(socks[0]) > 0:
                incoming_message = self.recv_data()

                # Server is disconnected
                if len(incoming_message) == 0:
                    raise Exception("Server not responding -> disconnect!")
                if len(incoming_message) > self.MAX_INPUT:
                    raise Exception("Too big data recv -> disconnect!")

                # Split messages by null character
                records = incoming_message.split("\x00")
                for record in records:
                    if record == "":
                        continue
                    socket_buffer.append(record)
                    print(f"Appended to recv buffer: {record}")
                # if len(socket_buffer) >= self.MAX_BUFF:
                result = True

        except Exception as e:
            # No data gathered
            print(e)
            messagebox.showinfo("Server Error", "Disconnect!")
            result = None

        return result

    def wrong_attempt(self):
        self.wrong_counter += 1
        print("Wrong counter: ", self.wrong_counter)

        if self.wrong_counter >= self.NUM_OF_ATTEMPTS:
            return True

        return False

