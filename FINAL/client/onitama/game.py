import engine
import parser
from parser import Cmd
from components import *
from network import Network

import pygame


# Resolution for main game window
WIDTH_PLAY = 1024
HEIGHT_PLAY = 512
# Dimension of board (how many squares)
DIMENSION = 5
# Square size
# (x, y)
SQ_SIZE = HEIGHT_PLAY // DIMENSION

# Temp variable for Card size
CARD_COEF = 1 / 125
# Card size (x, y)
CARD_SIZE = (CARD_COEF * (300 * SQ_SIZE), CARD_COEF * (174 * SQ_SIZE))

# Card positions
# (x, y)
CARD_POS = [
    # Black player's cards
    (WIDTH_PLAY // 2, 1),
    (WIDTH_PLAY - CARD_SIZE[0], 1),
    # Spare card
    (0.55 * CARD_SIZE[0] + WIDTH_PLAY // 2, HEIGHT_PLAY // 2 - CARD_SIZE[1] // 2),
    # White player's cards
    (WIDTH_PLAY // 2, HEIGHT_PLAY - CARD_SIZE[1]),
    (WIDTH_PLAY - CARD_SIZE[0], HEIGHT_PLAY - CARD_SIZE[1]),
]

# Temp variable for Label size
LABEL_COEF = 0.13
# Label positions (x, y)
# Position relative to B1 card
BLACK_NAME_POS = (CARD_POS[0][0], CARD_POS[0][1] + CARD_SIZE[1])
BLACK_STATUS_POS = (BLACK_NAME_POS[0] , BLACK_NAME_POS[1] + CARD_SIZE[1] * LABEL_COEF)
# Relative to spare card
SPARE_CARD_POS = (CARD_POS[2][0] + CARD_SIZE[0] * 2*LABEL_COEF, CARD_POS[2][1] - CARD_SIZE[1] * LABEL_COEF)
# Position relative to W1 card
WHITE_NAME_POS = (CARD_POS[3][0], CARD_POS[3][1] - CARD_SIZE[1] * LABEL_COEF)
WHITE_STATUS_POS = (WHITE_NAME_POS[0], WHITE_NAME_POS[1] - CARD_SIZE[1] * LABEL_COEF)

# Others
MAX_FPS = 15
PIECE_IMAGES = {}
CARD_IMAGES = {}


"""
Handle user input and update graphics. Also react to messages from server and update game stats accordingly.
"""
def play(net: Network, data, username, rec_data):
    # Init pygame library
    pygame.init()
    my_font = pygame.font.SysFont(FONT, 18)
    clock = pygame.time.Clock()
    screen = pygame.display.set_mode((WIDTH_PLAY, HEIGHT_PLAY))

    # Change icon and title
    program_icon = pygame.image.load('pieces/bN.png')
    pygame.display.set_icon(program_icon)
    pygame.display.set_caption('Onitama')

    # Game state
    gs = engine.GameState(net, data, username)
    if rec_data is not None:
        gs.reconnect(rec_data)

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

    rematch = False
    running = True
    while running:
        # True if player has no option to play, so he gives back one card by clicking twice on it and pass turn
        pass_turn = gs.nowhere_to_go()
        gs.broken_conn = net.broken_connection()

        # Event in pygame
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False

            # Mouse handler Only current player can play
            if e.type == pygame.MOUSEBUTTONDOWN \
                    and gs.win_white is None \
                    and gs.curr_p == gs.player_name \
                    and not gs.black_disconnected \
                    and not gs.white_disconnected \
                    and not gs.broken_conn:
                # x,y loc of mouse
                loc = pygame.mouse.get_pos()

                # Second clock Clicking on the board
                if card_picked is not None and (loc[0] <= WIDTH_PLAY // 2 and loc[1] <= HEIGHT_PLAY):
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
                            make_move = parser.prepare_make_move(
                                str(card_picked),
                                move.start_row,
                                move.start_col,
                                move.end_row,
                                move.end_col)
                            net.send_data(make_move)
                        clear_selection()
                # Second click -> Confirm to exchange that card nad pass turn
                elif card_picked is not None and pass_turn:
                    for i, card in enumerate(gs.selected_cards):
                        if gs.white_to_move and (i == 0 or i == 1):
                            continue
                        elif i == 2:
                            continue
                        elif not gs.white_to_move and (i == 3 or i == 4):
                            continue

                        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
                        if x < loc[0] < x + w and y < loc[1] < y + h:
                            if card_picked == card:
                                make_pass = parser.prepare_make_pass(card)
                                net.send_data(make_pass)
                                print("Pass was made with card: " + card)
                                break
                    clear_selection()
                # First click -> Clicking cards on the right side
                else:
                    clear_selection()
                    for i, card in enumerate(gs.selected_cards):
                        if gs.white_to_move and (i == 0 or i == 1):
                            continue
                        elif i == 2:
                            continue
                        elif not gs.white_to_move and (i == 3 or i == 4):
                            continue

                        # Selecting one card
                        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
                        if x < loc[0] < x + w and y < loc[1] < y + h:
                            valid_moves = gs.get_valid_moves(card)
                            card_picked = card
                            print("Selected: " + card)
                            break

        # Incoming message from server
        server_rx_buffer = []
        buff = net.network_data_arrived(server_rx_buffer)
        if buff is None:
            running = False
            rematch = False
        else:
            for record in server_rx_buffer:
                data = parser.parse(record)
                cmd = data[0]

                if cmd == Cmd.PING.value:
                    if data[1] == Cmd.PING.value:
                        net.allow_ping()
                    else:
                        running = not net.wrong_attempt()

                elif cmd == Cmd.MOVE_WAS_MADE.value:
                    try:
                        print(f"Move was accepted by server and made in client!")
                        if len(data) != 6:
                            raise Exception("Server MOVE_WAS_MADE invalid!")

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
                        running = not net.wrong_attempt()

                elif cmd == Cmd.PASS_WAS_MADE.value:
                    if len(data) == 2:
                        the_card = data[1]
                        print(f"Pass was accepted by server and made in client with card {the_card}!")
                        gs.shuffle_cards(the_card)
                        gs.switch_players()
                    else:
                        running = not net.wrong_attempt()

                elif cmd == Cmd.INVALID_MOVE.value:
                    if len(data) == 2:
                        print(Cmd.INVALID_MOVE.value, f"{data[1]}")
                    else:
                        running = not net.wrong_attempt()

                elif cmd == Cmd.GAME_OVER.value:
                    if len(data) == 2:
                        winner_name = data[1]
                        print(f"Game over encountered, player {winner_name} has won!")
                        gs.game_over(winner_name)
                    else:
                        running = not net.wrong_attempt()
                    rematch = True

                elif cmd == Cmd.RECONNECT.value:
                    if len(data) == 2 and data[1] == gs.enemy_name:
                        if gs.enemy_name == gs.white_name:
                            gs.white_disconnected = False
                        elif gs.enemy_name == gs.black_name:
                            gs.black_disconnected = False
                    else:
                        print("ERR: RECONNECT in Game not possible!")
                        running = not net.wrong_attempt()

                elif cmd == Cmd.DISCONNECT.value:
                    if len(data) == 2 and data[1] == gs.enemy_name:
                        if gs.enemy_name == gs.white_name:
                            gs.white_disconnected = True
                        elif gs.enemy_name == gs.black_name:
                            gs.black_disconnected = True
                    else:
                        print("ERR: DISCONNECT in Game not possible!")
                        running = not net.wrong_attempt()

                elif cmd == "WRONG_DATA":
                    print("ERR: WRONG_DATA", f"Data are not parsable!")
                    running = not net.wrong_attempt()

                else:
                    print(f"ERR: Unknown message in Game!")
                    running = not net.wrong_attempt()

        draw_game_state(screen, gs, valid_moves, sq_selected, card_picked, my_font)
        clock.tick(MAX_FPS)
        pygame.display.flip()

    print("Game finished - quiting pygame!")
    pygame.quit()
    return rematch


"""
Clear player selection so nothing is chosen
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
Global dictionary of pieces and cards. Pictures and loaded only once on program start.
Source of cards: https://www.ultraboardgames.com/onitama/game-rules.php
Source of pieces: https://drive.google.com/drive/folders/1qH7IQj5lj7o3MQIb5TAZhsDr_5f9p8aq
"""
def load_images(gs):
    # Assigning pieces using dictionary PIECE_IMAGES["wP"] = pieces/wP.png
    pieces = ["wP", "wK", "bP", "bK"]
    for piece in pieces:
        # Scale image using resolution
        PIECE_IMAGES[piece] = pygame.transform.scale(pygame.image.load("pieces/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))

    # Load all playing card images using dictionary CARD_IMAGES["wP"] = pieces/wP.png
    card_names = list(gs.all_cards.keys())
    for card in card_names:
        CARD_IMAGES[card] = pygame.transform.scale(pygame.image.load("cards/" + card + ".jpg"), CARD_SIZE)


"""
Responsible for all graphics changes within current game state. Drawing and refresh board, pieces, cards and labels.
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
Draw squares of board on the left side of window
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
Highlight clicked square which piece is selected
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
Draw pieces on the top of board.
"""
def draw_pieces(screen, board):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            # Not empty
            if piece != "--":
                screen.blit(PIECE_IMAGES[piece], pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


"""
Draw card holders with cards inside. Two cards for black, one spare card and two cards for white player.
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
Possible highlight for selected card. Also highlight cards for the current player
"""
def draw_card_highlight(screen, gs, which_card):
    # Blue highlight
    if gs.curr_p == gs.player_name \
            and gs.win_white is None \
            and not gs.black_disconnected \
            and not gs.white_disconnected \
            and not gs.broken_conn:
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

    if which_card is not None \
            and not gs.black_disconnected \
            and not gs.white_disconnected \
            and not gs.broken_conn:
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
Animate move of piece for one second
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
Draw label to inform which player is playing and some other info regarding connection and game over.
"""
def draw_player_label(screen, gs, my_font):
    black_who = "You" if gs.player_name == gs.black_name else "Enemy"
    white_who = "You" if gs.player_name == gs.white_name else "Enemy"

    black_status = "[PLAYING]" if not gs.white_to_move else ""
    white_status = "[PLAYING]" if gs.white_to_move else ""

    # Won / Lost
    if gs.win_white is not None:
        black_status = "[WON]" if not gs.win_white else "[LOST]"
        white_status = "[WON]" if gs.win_white else "[LOST]"

    # Disconnect
    elif gs.black_disconnected:
        black_status = "[DISCONNECT]"
        white_status = "[PAUSE]"
    elif gs.white_disconnected:
        black_status = "[PAUSE]"
        white_status = "[DISCONNECT]"

    # BROKEN CONN
    elif gs.broken_conn:
        black_status = "[BROKEN CONN]"
        white_status = "[BROKEN CONN]"

    black_name_lb = my_font.render(f"{black_who}: {gs.black_name}", True, BLACK)
    screen.blit(black_name_lb, BLACK_NAME_POS)

    black_status_lb = my_font.render(f"{black_status}", True, BLACK)
    screen.blit(black_status_lb, BLACK_STATUS_POS)

    spare_card_lb = my_font.render(f"Spare Card", True, BLACK)
    screen.blit(spare_card_lb, SPARE_CARD_POS)

    white_name_lb = my_font.render(f"{white_who}: {gs.white_name}", True, BLACK)
    screen.blit(white_name_lb, WHITE_NAME_POS)

    white_status_lb = my_font.render(f"{white_status}", True, BLACK)
    screen.blit(white_status_lb, WHITE_STATUS_POS)

