from enum import Enum
from network import Network


SPL = '|'
END = '\0'

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
    DISCONNECTED = "DISCONNECTED"
    LOGOUT = "LOGOUT"

def is_enum(provided):
    for e in Cmd:
        if e.value == provided:
            return True
    return False


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


def prepare_login(username):
    return f"{Cmd.LOGIN.value}{SPL}{username}{END}"

def prepare_make_move(card, s_row, s_col, e_row, e_col):
    return f"{Cmd.MAKE_MOVE.value}{SPL}{card}{SPL}{s_row}{SPL}{s_col}{SPL}{e_row}{SPL}{e_col}{END}"

def prepare_make_pass(card):
    return f"{Cmd.MAKE_PASS.value}{SPL}{card}{END}"

def attempt_disconnect(net: Network):
    net.wrong_counter += 1
    print("Wrong counter: ", net.wrong_counter)

    if net.wrong_counter >= net.NUM_OF_ATTEMPTS:
        return True

    return False
