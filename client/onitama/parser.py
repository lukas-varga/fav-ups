from enum import Enum

# Data seperator in one message
SPL = '|'
# Separator of several messages
END = '\0'

"""
Includes all commands used in TCP protocol as enum. Some of them are for client and some for server.
"""
class Cmd(Enum):
    """
     Client side
    """
    # Try login with username or reconnect to open game
    LOGIN = "LOGIN"
    # Send move for validation to server (piece + card)
    MAKE_MOVE = "MAKE_MOVE"
    # Pass turn if there is no other option left (only card)
    MAKE_PASS = "MAKE_PASS"

    """
    Server side
    """
    # Waiting for other players to join in lobby/waiting room
    WAITING = "WAITING"
    # Failed login or reconnect. As second parameter there is error message
    FAILED = "FAILED"
    # Game starts. First player is black, second is white. Five cards are distributed (everyone sees them).
    START = "START"
    # Provided move was correct (checked by server). Both players receive confirmation about new game state.
    MOVE_WAS_MADE = "MOVE_WAS_MADE"
    # User could not move in any way, so he passed the turn and switch the card. The other players is on turn.
    PASS_WAS_MADE = "PASS_WAS_MADE"
    # Provided move was incorrect for some reason. The reason is showed in second parameter. Player plays again.
    INVALID_MOVE = "INVALID_MOVE"
    # Game ended. Either by taking opponent king or moving my own king to opponent king's start square.
    GAME_OVER = "GAME_OVER"
    # Reconnect to game occurred. Reconnected player get all the game state info. Paused player get only notification.
    RECONNECT = "RECONNECT"
    # Player was notified that opponent disconnected from the game. He can reconnect in certain amount of time.
    DISCONNECT = "DISCONNECT"
    # Ping message which is sent by clients and server respond by pinging back. Occur every few seconds.
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
        msg = msg.strip()
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

