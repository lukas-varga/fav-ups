"""
Information about current game
"""


class GameState:
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


"""
Class for handling input moves from user
"""


class Move:
    # Chess-like description of rows
    ranks_to_rows = {"1": 4, "2": 3, "3": 2, "4": 1, "5": 0}
    rows_to_ranks = {v: k for k, v in ranks_to_rows}
    # Chess-like description of columns
    ranks_to_cols = {"a": 0, "b": 1, "c": 2, "d": 3, "e": 4}
    cols_to_ranks = {v: k for k, v in ranks_to_rows}

    def __init__(self, start_sq, end_sq, board):
        self.start_row = start_sq[0]
        self.start_col = start_sq[1]
        self.end_row = end_sq[0]
        self.end_col = end_sq[1]
        self.piece_moved = board[self.start_row][self.start_col]
        self.piece_captured = board[self.end_row][self.end_col]
