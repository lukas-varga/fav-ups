"""
All components
"""

"""
Cards presented in the game using vectors from current location
from point of view of current player (x,y), (x,y), ...
"""
CARDS = {
    "tiger": [(0, 2), (0, -1)],
    "dragon": [(-2, 1), (2, 1), (-1, -1), (1, -1)],
    "frog": [(-1, 1), (-2, 0), (1, -1)],
    "rabbit": [(1, 1), (2, 0), (-1, -1)],
    "crab": [(0, 1), (-2, 0), (2, 0)],
    "elephant": [(-1, 1), (1, 1), (-1, 0), (1, 0)],
    "goose": [(-1, 1), (-1, 0), (1, 0), (1, -1)],
    "rooster": [(1, 1), (-1, 0), (1, 0), (-1, -1)],
    "monkey": [(-1, 1), (1, 1), (-1, -1), (1, -1)],
    "mantis": [(-1, 1), (1, 1), (0, -1)],
    "horse": [(0, 1), (-1, 0), (0, -1)],
    "ox": [(0, 1), (1, 0), (0, -1)],
    "crane": [(0, 1), (-1, -1), (1, -1)],
    "boar": [(0, 1), (-1, 0), (1, 0)],
    "eel": [(-1, 1), (1, 0), (-1, -1)],
    "cobra": [(1, 1), (-1, 0), (1, -1)],
}

CARD_TO_ID = {card: i for i, card in enumerate(CARDS, 0)}
ID_TO_CARD = {i: card for card, i in CARD_TO_ID.items()}

"""
Board is 5x5 2D list where each element has two chars
Fst char represents color and snd char represents type (Pawn/King)
Symbol -- represents empty board
"""

