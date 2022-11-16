"""
Game engine with all the info about the game
"""

import numpy as np

from components import ID_TO_CARD, CARDS
from network import Network

"""
Information about current game
"""
class GameState:
    def __init__(self):
        # Network socket
        self.net = Network()

        """
        Board is 5x5 2D list where each element has two chars
        Fst char represents color and snd char represents type (Pawn/King)
        Symbol -- represents empty board
        """
        self.board = [
            ["bP", "bP", "bK", "bP", "bP"],
            ["--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--"],
            ["wP", "wP", "wK", "wP", "wP"]
        ]
        self.cards = CARDS

        # Move to server C++
        # 0 and 1 are white
        # 2 is spare
        # 3 and 4 are black
        self.selected_cards = self.generate_five_cards()

        self.white_to_move = True
        self.move_log = []
        self.is_winner_white = None

    """
    Pick randomly five cards at the beginning
    """
    def generate_five_cards(self):
        random_ids = list(np.random.permutation(len(self.cards))[:5])

        selected_cards = []
        for x in random_ids:
            name = ID_TO_CARD.get(x)
            selected_cards.append(name)

        return selected_cards

    """
    Get selected card by id
    """
    def get_card_by_id(self, card_id):
        for i, c in enumerate(self.selected_cards):
            if card_id == i:
                return c

    """
    Get id by selected card
    """
    def get_id_by_card(self, card_name):
        for i, c in enumerate(self.selected_cards):
            if card_name == c:
                return i

    """
    Takes move as parameter and executes it
    """
    def make_move(self, move, card_picked):
        self.board[move.start_row][move.start_col] = "--"
        self.board[move.end_row][move.end_col] = move.piece_moved
        # Log the move
        self.move_log.append(move)

        # Check check-mate or king on enemy base
        if self.is_game_over(move):
            self.is_winner_white = self.white_to_move
        # Shuffle cards and swap players
        else:
            self.shuffle_cards(card_picked)
            self.white_to_move = not self.white_to_move

    """
    Return true if game over otherwise false
    """
    def is_game_over(self, move):
        # Checkmate
        if move.piece_captured[1] == 'K':
            return True
        # Enemy base for white
        elif self.board[0][2] == "wK":
            return True
        # Enemy base for black
        elif self.board[4][2] == "bK":
            return True
        return False

    """
    Shuffle card, which was played right away and give player spare card
    """
    def shuffle_cards(self, card_picked):
        card_id = self.get_id_by_card(card_picked)
        temp = self.selected_cards[2]
        self.selected_cards[2] = card_picked
        self.selected_cards[card_id] = temp

    """
    Undo last move
    """
    def undo_move(self):
        # There are some moves
        if len(self.move_log) != 0:
            move = self.move_log.pop()
            self.board[move.start_row][move.start_col] = move.piece_moved
            self.board[move.end_row][move.end_col] = move.piece_captured
            self.is_winner_white = None
            self.white_to_move = not self.white_to_move

    """
    Get list of all valid moves for given card.
    White or black on turn - will distinguish
    """
    def get_valid_moves(self, card):
        moves = []
        for r in range(len(self.board)):
            for c in range(len(self.board[r])):
                turn = self.board[r][c][0]
                piece = self.board[r][c][1]

                if piece != "--":
                    # White
                    if turn == 'w' and self.white_to_move:
                        self.one_valid_move(r, c, card, moves, turn)
                    # Black
                    elif turn == 'b' and not self.white_to_move:
                        self.one_valid_move(r, c, card, moves, turn)

        return moves

    """
    Calculate mvoes for one piece
    """
    def one_valid_move(self, r, c, card, moves, turn):
        matrix = self.cards[card]
        for raw_vec in matrix:
            # Invert for black player
            vec = (0 - raw_vec[0], 0 - raw_vec[1]) if not self.white_to_move else raw_vec

            # Transform x,y system to board system
            x = vec[0]
            y = vec[1]
            new_r = r - y
            new_c = c + x

            # In range of board and not capturing our own
            if new_r in range(0, 5) and new_c in range(0, 5):
                new_move = Move((r, c), (new_r, new_c), self.board)
                symbol = new_move.piece_captured[0]
                if symbol != turn:
                    moves.append(new_move)


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
        self.move_id = self.start_row * 1000 + self.start_col * 100 + self.end_row * 10 + self.end_col

    """
    Overriding equal method
    """
    def __eq__(self, other):
        if isinstance(other, Move):
            return self.move_id == other.move_id
        return False

    """
    Method for toString like printing
    """
    def get_chess_like_notation(self):
        source = self.get_rank_file(self.start_row, self.start_col)
        target = self.get_rank_file(self.end_row, self.end_col)
        return source + " -> " + target

    """
    Get rows and cols using chess description
    """
    def get_rank_file(self, r, c):
        return self.cols_to_files[c] + self.rows_to_ranks[r]
