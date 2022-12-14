from enum import Enum

class Commands(Enum):
    # Client side
    LOGIN = "LOGIN"
    MAKE_MOVE = "MAKE_MOVE"
    STALEMATE = "STALEMATE"
    # Server side
    WAITING = "WAITING"
    FAILED_LOGIN = "FAILED_LOGIN"
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
    return msg.split(SPL)


def login(username):
    return f"{Commands.LOGIN.value}{SPL}{username}{END}"

def make_move(card, start_pos, end_pos):
    return f"{Commands.MAKE_MOVE.value}{SPL}{card}{SPL}{start_pos}{SPL}{end_pos}{END}"

def stalemate(card_to_swap):
    return f"{Commands.MAKE_MOVE.value}{SPL}{card_to_swap}{END}"

