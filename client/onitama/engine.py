from network import Network
from components import *

"""
Information about current game
"""


class GameState:
    def __init__(self, net: Network, start_arr, username):
        self.net = net
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
        self.move_log = []

        # START | P1 (black) | P2 (white) | 5x cards
        self.black_name = start_arr[1]
        self.white_name = start_arr[2]

        self.white_to_move = True
        self.curr_p = self.white_name

        if self.black_name == username:
            self.player_name = self.black_name
            self.enemy_name = self.white_name
        else:
            self.player_name = self.white_name
            self.enemy_name = self.black_name

        self.win_white = None
        self.black_disconnected = False
        self.white_disconnected = False
        # Generating in sever
        # 0 and 1 are black
        # 2 is spare
        # 3 and 4 are white
        self.all_cards = DEF_CARDS
        self.selected_cards = start_arr[3:]

    """
    Takes move as parameter and executes it
    """

    def make_move(self, move):
        # Make move
        self.board[move.start_row][move.start_col] = "--"
        self.board[move.end_row][move.end_col] = move.piece_moved
        # Log the move
        self.move_log.append(move)
        print("Move made: " + move.get_chess_like_notation())

    """
    Shuffle card, which was played right away and give player spare card
    """

    def shuffle_cards(self, card_picked):
        card_id = self.get_id_by_card(card_picked)
        temp = self.selected_cards[2]
        self.selected_cards[2] = card_picked
        self.selected_cards[card_id] = temp
        print("Cards was shuffled!")

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
    Switch player
    """

    def switch_players(self):
        self.curr_p = self.black_name if self.white_to_move else self.white_name
        self.white_to_move = not self.white_to_move
        print("Players were switched!")

    """
    Server encountered game over so switching accordingly
    """

    def game_over(self, winner_name):
        if winner_name == self.black_name:
            self.win_white = False
        elif winner_name == self.white_name:
            self.win_white = True
        else:
            print(f"ERR: Neither Black nor White player has won!")

    """
    Player has nowhere to go with both his card so he choose card to discard
    """

    def nowhere_to_go(self):
        if self.curr_p == self.black_name:
            list_1 = self.get_valid_moves(self.selected_cards[0])
            list_2 = self.get_valid_moves(self.selected_cards[1])
            if not list_1 and not list_2:
                return True
        elif self.curr_p == self.white_name:
            list_1 = self.get_valid_moves(self.selected_cards[3])
            list_2 = self.get_valid_moves(self.selected_cards[4])
            if not list_1 and not list_2:
                return True
        return False

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

                if piece != "-":
                    # White
                    if turn == 'w' and self.white_to_move:
                        self.one_valid_move(r, c, card, moves, turn)
                    # Black
                    elif turn == 'b' and not self.white_to_move:
                        self.one_valid_move(r, c, card, moves, turn)

        return moves

    """
    Calculate moves for one piece
    """

    def one_valid_move(self, r, c, card, moves, turn):
        matrix = self.all_cards[card]
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
    Fill onitama engine with reconnected data
    """

    def reconnect(self, rec_data):
        # is_player_white | white_to_move | (25x) "wP" "wK" "--"
        is_player_white_rec = None
        if rec_data[0] == "1":
            is_player_white_rec = True
        elif rec_data[0] == "0":
            is_player_white_rec = False

        white_to_move_rec = None
        if rec_data[1] == "1":
            white_to_move_rec = True
        elif rec_data[1] == "0":
            white_to_move_rec = False

        cards_rec = rec_data[2:]
        if (
                (is_player_white_rec and self.white_name == self.player_name) or
                (not is_player_white_rec and self.black_name == self.player_name)
        ) and len(cards_rec) == 25:
            # Right player to play BOOL + CURR_P
            self.curr_p = self.white_name if white_to_move_rec else self.black_name
            self.white_to_move = white_to_move_rec

            # Reset all the board to actual state on the server
            i = 0
            for r in range(len(self.board)):
                for c in range(len(self.board[r])):
                    self.board[r][c] = cards_rec[i]
                    i += 1

        else:
            print("ERR: Some problem when processing reconnect data!")


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
