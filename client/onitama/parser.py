from enum import Enum

# Data seperator in one message
SPL = '|'
# Separator of several messages
END = '\0'

"""
Enum of possible valid commands used in communication. Some of them are for client and some for server.
"""
class Cmd(Enum):
    # Client side
    LOGIN = "LOGIN"
    MAKE_MOVE = "MAKE_MOVE"
    MAKE_PASS = "MAKE_PASS"
    # Server side
    WAITING = "WAITING"
    FAILED = "FAILED"
    START = "START"
    MOVE_WAS_MADE = "MOVE_WAS_MADE"
    PASS_WAS_MADE = "PASS_WAS_MADE"
    INVALID_MOVE = "INVALID_MOVE"
    GAME_OVER = "GAME_OVER"
    RECONNECT = "RECONNECT"
    DISCONNECT = "DISCONNECT"
    PING = "PING"


"""
Return true is provided string is valid enum.
"""
def is_enum(provided):
    for e in Cmd:
        if e.value == provided:
            return True
    return False


"""
Parse incoming message and either return it in array or return WRONG_DATA.
"""
def parse(msg: str):
    try:
        res = msg.split(SPL)
        if len(res) < 2:
            raise Exception("Command should have at least one parameter!")
        elif not is_enum(res[0]):
            raise Exception("Not a defined Command!")
    except Exception as e:
        res = ["WRONG_DATA"]
        print(e)
    return res


"""
Prepare LOGIN message to be send
"""
def prepare_login(username):
    return f"{Cmd.LOGIN.value}{SPL}{username}{END}"


"""
Prepare MAKE_MOVE message to be send
"""
def prepare_make_move(card, s_row, s_col, e_row, e_col):
    return f"{Cmd.MAKE_MOVE.value}{SPL}{card}{SPL}{s_row}{SPL}{s_col}{SPL}{e_row}{SPL}{e_col}{END}"


"""
Prepare MAKE_PASS message to be send
"""
def prepare_make_pass(card):
    return f"{Cmd.MAKE_PASS.value}{SPL}{card}{END}"


"""
Prepare PING message to be send. Message always looks like PING|PING
"""
def prepare_ping():
    return f"{Cmd.PING.value}{SPL}{Cmd.PING.value}{END}"

