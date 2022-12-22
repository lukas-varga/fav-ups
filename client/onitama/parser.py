from enum import Enum

class Cmd(Enum):
    # Client side
    LOGIN = "LOGIN"
    MAKE_MOVE = "MAKE_MOVE"
    STALEMATE = "STALEMATE"
    # Server side
    WRONG_DATA = "WRONG_DATA"
    FAILED_LOGIN = "FAILED_LOGIN"
    WAITING = "WAITING"
    START = "START"
    MOVE_WAS_MADE = "MOVE_WAS_MADE"
    INVALID_MOVE = "INVALID_MOVE"
    STALEMATE_CARD = "STALEMATE_CARD"
    RECONNECT = "RECONNECT"
    LOGOUT = "LOGOUT"
    GAME_OVER = "GAME_OVER"

SPL = '|'
END = '\0'

def parse(msg: str):
    try:
        return msg.split(SPL)
    except:
        print("ERR: Could not parse the server reply!")


def send_login(username):
    return f"{Cmd.LOGIN.value}{SPL}{username}{END}"

def send_make_move(card, start_pos, end_pos):
    return f"{Cmd.MAKE_MOVE.value}{SPL}{card}{SPL}{start_pos}{SPL}{end_pos}{END}"

def send_stalemate(card_to_swap):
    return f"{Cmd.MAKE_MOVE.value}{SPL}{card_to_swap}{END}"

