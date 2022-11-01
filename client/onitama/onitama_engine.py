import random

import numpy as np
from onitama.components import CARDS, BOARD, ID_TO_CARD

"""
Information about current game
"""
class GameState:
    def __init__(self):
        self.board = BOARD
        self.cards = CARDS
        self.selected_cards = self.pick_five_cards()

        self.white_to_move = True
        self.move_log = []

    def pick_five_cards(self):
        selected_ids = list(np.random.permutation(len(self.cards))[:5])

        selected_cards = []
        for x in selected_ids:
            name = ID_TO_CARD.get(x)
            selected_cards.append(name)

        return selected_cards

    """
    Takes move as parameter and executes it
    """
    def make_move(self, move):
        self.board[move.start_row][move.start_col] = "--"
        self.board[move.end_row][move.end_col] = move.piece_moved
        # Log the move
        self.move_log.append(move)
        # Swap players
        self.white_to_move = not self.white_to_move

    """
    Undo last move
    """
    def undo_move(self):
        # There are some moves
        if len(self.move_log) != 0:
            move = self.move_log.pop()
            self.board[move.start_row][move.start_col] = move.piece_moved
            self.board[move.end_row][move.end_col] = move.piece_captured
            self.white_to_move = not self.white_to_move


"""
Class for handling input moves from user
"""
class Move:
    # Chess-like description of rows
    ranks_to_rows = {"1": 4, "2": 3, "3": 2, "4": 1, "5": 0}
    rows_to_ranks = {v: k for k, v in ranks_to_rows.items()}
    # Chess-like description of columns
    files_to_cols = {"a": 0, "b": 1, "c": 2, "d": 3, "e": 4}
    cols_to_files = {v: k for k, v in files_to_cols.items()}

    def __init__(self, start_sq, end_sq, board):
        self.start_row = start_sq[0]
        self.start_col = start_sq[1]
        self.end_row = end_sq[0]
        self.end_col = end_sq[1]
        self.piece_moved = board[self.start_row][self.start_col]
        self.piece_captured = board[self.end_row][self.end_col]

    """
    Method for toString like printing
    """
    def get_chess_like_notation(self):
        return self.get_rank_file(self.start_col, self.start_col) + " -> " + self.get_rank_file(self.end_col,
                                                                                                self.end_row)

    """
    Get rows and cols using chess description
    """
    def get_rank_file(self, r, c):
        return self.cols_to_files[c] + self.rows_to_ranks[r]