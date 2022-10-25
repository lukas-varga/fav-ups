"""
Information about current game
"""
class game_state:
    def __init__(self):
        # Board is 5x5 2D list where each element has two chars
        # Fst char represents color and snd char represents type (Pawn/King)
        # Symbol -- represents empty board
        self.board = [
            ["bP", "bP", "bK", "bP", "bP"],
            ["--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--"],
            ["wP", "wP", "wK", "wP", "wP"]
        ]

        self.white_to_move = True
        self.move_log = []