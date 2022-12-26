import engine
import parser
from parser import Cmd
from network import Network

from tkinter import messagebox
import pygame


# COLORS
# Board
BACKGROUND_COLOR = "bisque2"
LIGHT_COLOR = "bisque1"
DARK_COLOR = "bisque3"
KING_COLOR = "bisque4"
# Text and frames
BLACK = "black"
WHITE = "white"
# Highlight
SQ_HIGHLIGHT_ACTIVE = "yellow"
SQ_HIGHLIGHT_PASSIVE = "blue"
CARD_HIGHLIGHT_ACTIVE = "yellow2"
CARD_HIGHLIGHT_PASSIVE = "steelblue3"


# Font
FONT = "consolas"

# Resolution
WIDTH = 1024
HEIGHT = 512
# Dimension of board
DIMENSION = 5
# Square size
# (x, y)
SQ_SIZE = HEIGHT // DIMENSION

# Card size
COEFFICIENT = 1 / 125
# (x, y)
CARD_SIZE = (COEFFICIENT * (300 * SQ_SIZE), COEFFICIENT * (174 * SQ_SIZE))

# Card positions
# (x, y)
CARD_POS = [
    # Black player's cards
    (WIDTH // 2, 1),
    (WIDTH - CARD_SIZE[0], 1),
    # Spare card
    (0.55 * CARD_SIZE[0] + WIDTH // 2, HEIGHT // 2 - CARD_SIZE[1] // 2),
    # White player's cards
    (WIDTH // 2, HEIGHT - CARD_SIZE[1]),
    (WIDTH - CARD_SIZE[0], HEIGHT - CARD_SIZE[1]),
]

# Label positions
# (x, y)
# Position relative to B1
BLACK_LABEL_POS = (CARD_POS[0][0], CARD_POS[0][1] + CARD_SIZE[1])
# Position relative to W1
WHITE_LABEL_POS = (CARD_POS[3][0], CARD_POS[3][1] - CARD_SIZE[1] * 0.13)
# Relative to spare card
SPARE_LABEL_POS = (CARD_POS[2][0] + CARD_SIZE[0] * 0.25, CARD_POS[2][1] - CARD_SIZE[1] * 0.13)

# Others
MAX_FPS = 15
PIECE_IMAGES = {}
CARD_IMAGES = {}


"""
Handle user input and update graphics
"""
def play(net: Network, start_arr, username):
    # Init pygame library
    pygame.init()
    my_font = pygame.font.SysFont(FONT, 18)
    clock = pygame.time.Clock()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))

    # Change icon and title
    program_icon = pygame.image.load('pieces/bN.png')
    pygame.display.set_icon(program_icon)
    pygame.display.set_caption('Onitama')

    # Game state
    gs = engine.GameState(net, start_arr, username)

    # Only once before loop
    load_images(gs)

    # Nullable variables
    global card_picked, valid_moves, sq_selected, player_clicks
    # No square selected, last click of user (row, col)
    sq_selected = ()
    # All the player clicks two tuple [(4,5), (2,2)]
    player_clicks = []
    # Valid moves
    valid_moves = []
    # If user selected card
    card_picked = None

    running = True
    while running:
        # Event in pygame
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                # TODO make a lot of when closing window
                running = False

            # Mouse handler Only current player can play
            if e.type == pygame.MOUSEBUTTONDOWN and gs.is_winner_white is None and gs.curr_p == gs.player_name:
                # x,y loc of mouse
                loc = pygame.mouse.get_pos()

                # Clicking on the board
                if card_picked is not None and (loc[0] <= WIDTH // 2 and loc[1] <= HEIGHT):
                    col = loc[0] // SQ_SIZE
                    row = loc[1] // SQ_SIZE

                    # User clicked at the same square twice
                    if sq_selected == (row, col):
                        # Reset user clicks
                        sq_selected = ()
                        player_clicks = []
                    else:
                        # Append fst and snd click
                        sq_selected = (row, col)
                        player_clicks.append(sq_selected)

                    # Second click
                    if len(player_clicks) == 2:
                        move = engine.Move(player_clicks[0], player_clicks[1], gs.board)
                        if move in valid_moves:
                            # Send MAKE_MOVE to server -> Reply will be collected in select in next iteration
                            make_move = parser.prepade_make_move(
                                str(card_picked),
                                move.start_row,
                                move.start_col,
                                move.end_row,
                                move.end_col)
                            net.send_data(make_move)
                        clear_selection()
                # First clik -> Clicking cards on the right side
                else:
                    clear_selection()
                    for i, card in enumerate(gs.selected_cards):
                        if gs.white_to_move and (i == 0 or i == 1):
                            continue
                        elif i == 2:
                            continue
                        elif not gs.white_to_move and (i == 3 or i == 4):
                            continue

                        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
                        if x < loc[0] < x + w and y < loc[1] < y + h:
                            valid_moves = gs.get_valid_moves(card)
                            card_picked = card
                            print("Selected: " + card)
                            break

        # Incoming message from server
        server_rx_buffer = []
        if net.network_data_arrived(server_rx_buffer):
            for record in server_rx_buffer:
                data = parser.parse(record)

                if data[0] == Cmd.MOVE_WAS_MADE.value:
                    print(f"Move was accepted by server!")
                    try:
                        # Move OK -> switch cards and move piece
                        the_card = data[1]
                        start_tup = (int(data[2]), int(data[3]))
                        end_tup = (int(data[4]), int(data[5]))
                        the_move = engine.Move(start_tup, end_tup, gs.board)

                        # Make the move on the board
                        gs.make_move(the_move)
                        if gs.move_log:
                            animate_move(gs.move_log[-1], screen, gs.board, clock)

                        # Shuffle cards and swap players
                        gs.shuffle_cards(the_card)
                        gs.switch_players()

                    except Exception as e:
                        print(e)
                elif data[0] == Cmd.INVALID_MOVE.value:
                    print("Inform current player, that it move was invalid!")
                    messagebox.showinfo(f"Invalid Move", data[1])
                elif data[0] == Cmd.GAME_OVER.value:
                    print(f"Game over, one player won")
                    messagebox.showinfo(f"Game Over", "One player has won!")
                elif data[0] == "WRONG_DATA":
                    print(f"WRONG_DATA -> opponent do not know how to respond -> {data}")
                else:
                    print(f"ERR: What the heck happened at client -> {data}")

        draw_game_state(screen, gs, valid_moves, sq_selected, card_picked, my_font)
        clock.tick(MAX_FPS)
        pygame.display.flip()

    print("Game finished - quiting pygame!")
    pygame.quit()
    return


"""
Global dictionary of pieces
"""
def load_images(gs):
    # Assigning pieces using dictionary PIECE_IMAGES["wP"] = pieces/wP.png
    pieces = ["wP", "wK", "bP", "bK"]
    for piece in pieces:
        # Scale image using resolution
        PIECE_IMAGES[piece] = pygame.transform.scale(pygame.image.load("pieces/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))

    # Load all playing card images using dictionary CARD_IMAGES["wP"] = pieces/wP.png
    card_names = list(gs.cards.keys())
    for card in card_names:
        CARD_IMAGES[card] = pygame.transform.scale(pygame.image.load("cards/" + card + ".jpg"), CARD_SIZE)


"""
Responsible for all graphics within current game state
"""
def draw_game_state(screen, gs, valid_moves, sq_selected, which_card, my_font):
    # Reset screen
    screen.fill(pygame.Color(BACKGROUND_COLOR))
    # Draw squares of board
    draw_board(screen)
    # Highlight
    highlight_square(screen, gs, valid_moves, sq_selected)
    # Draw pieces on the top of board
    draw_pieces(screen, gs.board)
    # Draw holders for cards
    draw_card_holders(screen, gs)
    # Selected card highlight
    draw_card_highlight(screen, gs, which_card)
    # Draw playing label
    draw_player_label(screen, gs, my_font)


"""
Clear player selection
"""
def clear_selection():
    global card_picked, valid_moves, sq_selected, player_clicks
    # Reset card
    card_picked = None
    # Reset valid moves
    valid_moves = []
    # Reset user clicks
    sq_selected = ()
    player_clicks = []


"""
Draw squares of board
"""
def draw_board(screen):
    global colors
    colors = [pygame.Color(LIGHT_COLOR), pygame.Color(DARK_COLOR)]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            if (r, c) == (0, 2) or (r, c) == (4, 2):
                pygame.draw.rect(screen, KING_COLOR, pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))
                continue
            color = colors[(r + c) % 2]
            pygame.draw.rect(screen, color, pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))



"""
Highlight square and moves for piece
"""
def highlight_square(screen, gs, valid_moves, sq_selected):
    if sq_selected != ():
        r, c = sq_selected
        # sq_selected is piece than can be moved
        if gs.board[r][c][0] == ('w' if gs.white_to_move else "b"):
            # Highlight
            s = pygame.Surface((SQ_SIZE, SQ_SIZE))
            # Transparency value (0 -> transparent, 255 -> opaque)
            s.set_alpha(100)
            s.fill(pygame.Color(SQ_HIGHLIGHT_ACTIVE))
            screen.blit(s, (c * SQ_SIZE, r * SQ_SIZE))

            # Highlight moves
            s.fill(pygame.Color(SQ_HIGHLIGHT_PASSIVE))
            for move in valid_moves:
                if move.start_row == r and move.start_col == c:
                    screen.blit(s, (move.end_col * SQ_SIZE, move.end_row * SQ_SIZE))


"""
Draw pieces on the top of board
"""
def draw_pieces(screen, board):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            # Not empty
            if piece != "--":
                screen.blit(PIECE_IMAGES[piece], pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


"""
Draw card holders
"""
def draw_card_holders(screen, gs):
    for i, card in enumerate(gs.selected_cards):
        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
        # Rotate 180 degrees to face player
        #image = pygame.transform.rotate(CARD_IMAGES[card], 180) if not gs.white_to_move else CARD_IMAGES[card]
        image = CARD_IMAGES[card]
        screen.blit(image, pygame.Rect(x, y, w, h))
        pygame.draw.rect(screen, pygame.Color(BLACK), pygame.Rect(x, y, w, h), width=1)


"""
Possible highlight for selected card
"""
def draw_card_highlight(screen, gs, which_card):
    # Blue highlight
    if gs.curr_p == gs.player_name:
        if gs.player_name == gs.black_name:
            x, y, w, h = get_rect_tuple(0)
            pygame.draw.rect(screen, pygame.Color(CARD_HIGHLIGHT_PASSIVE), pygame.Rect(x, y, w, h), width=3)
            x, y, w, h = get_rect_tuple(1)
            pygame.draw.rect(screen, pygame.Color(CARD_HIGHLIGHT_PASSIVE), pygame.Rect(x, y, w, h), width=3)
        elif gs.player_name == gs.white_name:
            x, y, w, h = get_rect_tuple(3)
            pygame.draw.rect(screen, pygame.Color(CARD_HIGHLIGHT_PASSIVE), pygame.Rect(x, y, w, h), width=3)
            x, y, w, h = get_rect_tuple(4)
            pygame.draw.rect(screen, pygame.Color(CARD_HIGHLIGHT_PASSIVE), pygame.Rect(x, y, w, h), width=3)

    if which_card is not None:
        # Actively selected card
        i = gs.get_id_by_card(which_card)
        x, y, w, h = get_rect_tuple(i)
        pygame.draw.rect(screen, pygame.Color(CARD_HIGHLIGHT_ACTIVE), pygame.Rect(x, y, w, h), width=5)


"""
Return selected card for highlighting
"""
def get_rect_tuple(i):
    return CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1]


"""
Animate move    
"""
def animate_move(move, screen, board, clock):
    global colors
    d_r = move.end_row - move.start_row
    d_c = move.end_col - move.start_col
    frames_per_square = 10
    frame_count = (abs(d_r) + abs(d_c)) * frames_per_square

    for frame in range(frame_count + 1):
        r, c = (move.start_row + d_r*frame/frame_count, move.start_col + d_c*frame/frame_count)
        draw_board(screen)
        draw_pieces(screen, board)

        # Erase
        color = colors[(move.end_row + move.end_col) % 2]
        end_square = pygame.Rect(move.end_col*SQ_SIZE, move.end_row*SQ_SIZE, SQ_SIZE, SQ_SIZE)
        pygame.draw.rect(screen, color, end_square)

        # Draw captured pieces
        if move.piece_captured != "--":
            screen.blit(PIECE_IMAGES[move.piece_captured], end_square)

        # Draw moving piece
        screen.blit(PIECE_IMAGES[move.piece_moved], pygame.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))
        pygame.display.flip()
        clock.tick(60)


"""
Draw label to inform which player is playing
"""
def draw_player_label(screen, gs, my_font):
    b_name = "YOU" if gs.player_name == gs.black_name else "OPPONENT"
    w_name = "YOU" if gs.player_name == gs.white_name else "OPPONENT"
    b_playing = " (PLAYING)" if not gs.white_to_move else ""
    w_playing = " (PLAYING)" if gs.white_to_move else ""

    black_label = my_font.render(f"{b_name}: {gs.black_name}{b_playing}", True, BLACK)
    screen.blit(black_label, BLACK_LABEL_POS)

    white_label = my_font.render(f"{w_name}: {gs.white_name}{w_playing}", True, BLACK)
    screen.blit(white_label, WHITE_LABEL_POS)

    spare_label = my_font.render(f"SPARE CARD", True, BLACK)
    screen.blit(spare_label, SPARE_LABEL_POS)
