# COLORS
# For board and window elements
BACKGROUND_COLOR = "bisque2"
LIGHT_COLOR = "bisque1"
DARK_COLOR = "bisque3"
KING_COLOR = "bisque4"
# Text and frames
BLACK = "black"
WHITE = "white"
# Highlights
SQ_HIGHLIGHT_ACTIVE = "yellow"
SQ_HIGHLIGHT_PASSIVE = "blue"
CARD_HIGHLIGHT_ACTIVE = "yellow2"
CARD_HIGHLIGHT_PASSIVE = "steelblue3"

# FONT
FONT = "calibri"

"""
Cards presented in the game using map. Key is string name
and value are coordinates from current location (x,y), (x,y), ...
"""
DEF_CARDS = {
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

"""
Map card name to id.
"""
CARD_TO_ID = {card: i for i, card in enumerate(DEF_CARDS, 0)}

"""
Map id name to card name.
"""
ID_TO_CARD = {i: card for card, i in CARD_TO_ID.items()}
