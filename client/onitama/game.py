import pygame
import engine


# Colors
LIGHT_COLOR = "antiquewhite1"
DARK_COLOR = "antiquewhite3"
BACKGROUND_COLOR = "antiquewhite2"
BORDER_COLOR = "black"
HIGHLIGHT_COLOR_1 = "blue"
HIGHLIGHT_COLOR_2 = "yellow"
FONT = 'calibri'

# Resolution
WIDTH = 1024
HEIGHT = 512
# Dimension of board
DIMENSION = 5
# Square size
SQ_SIZE = HEIGHT // DIMENSION

# Card size
COEFFICIENT = 1 / 125
CARD_SIZE = (COEFFICIENT * (300 * SQ_SIZE), COEFFICIENT * (174 * SQ_SIZE))

# Card positions
CARD_POS = [
    # Black player's cards
    (WIDTH // 2, 1),
    (WIDTH - CARD_SIZE[0], 1),
    # Spare card
    (0.5 * CARD_SIZE[0] + WIDTH // 2, HEIGHT // 2 - CARD_SIZE[1] // 2),
    # White player's cards
    (WIDTH // 2, HEIGHT - CARD_SIZE[1]),
    (WIDTH - CARD_SIZE[0], HEIGHT - CARD_SIZE[1]),
]

# Others
MAX_FPS = 15
PIECE_IMAGES = {}
CARD_IMAGES = {}


"""
Handle user input and update graphics
"""
def play(net, start, username):
    # Init pygame library
    pygame.init()
    clock = pygame.time.Clock()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))

    # Change icon and title
    program_icon = pygame.image.load('pieces/bN.png')
    pygame.display.set_icon(program_icon)
    pygame.display.set_caption('Onitama')

    # Game state

    gs = engine.GameState()

    # Only once before loop
    screen.fill(pygame.Color(BACKGROUND_COLOR))
    load_images(gs)

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
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False

            # Mouse handler
            elif e.type == pygame.MOUSEBUTTONDOWN and gs.is_winner_white is None:
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

                    # Snd click
                    if len(player_clicks) == 2:
                        move = engine.Move(player_clicks[0], player_clicks[1], gs.board)
                        if move in valid_moves:
                            # Move
                            gs.make_move(move, card_picked)
                            if gs.move_log:
                                animate_move(gs.move_log[-1], screen, gs.board, clock)
                            print("Move made:" + move.get_chess_like_notation())
                            clear_selection()
                        else:
                            clear_selection()
                # Clicking cards on the right side
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

            # Undo with Z TODO delete on release
            # Keyboard handler
            elif e.type == pygame.KEYDOWN:
                if e.key == pygame.K_z:
                    gs.undo_move()
                    clear_selection()
                if e.key == pygame.K_r:
                    gs = engine.GameState()
                    clear_selection()

        draw_game_state(screen, gs, valid_moves, sq_selected, card_picked)
        clock.tick(MAX_FPS)
        pygame.display.flip()

        if gs.is_winner_white is not None:
            win_text = "White Won" if gs.is_winner_white else "Black Won"
            pygame.display.set_caption(win_text)


    print("Game finished")
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
Responsible for all graphics within current game state
"""
def draw_game_state(screen, gs, valid_moves, sq_selected, which_card):
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
            s.fill(pygame.Color(HIGHLIGHT_COLOR_1))
            screen.blit(s, (c * SQ_SIZE, r * SQ_SIZE))

            # Highlight moves
            s.fill(pygame.Color(HIGHLIGHT_COLOR_2))
            for move in valid_moves:
                if move.start_row == r and move.start_col == c:
                    screen.blit(s, (move.end_col * SQ_SIZE, move.end_row * SQ_SIZE))


"""
Draw squares of board
"""
def draw_board(screen):
    global colors
    colors = [pygame.Color(LIGHT_COLOR), pygame.Color(DARK_COLOR)]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            color = colors[(r + c) % 2]
            pygame.draw.rect(screen, color, pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


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
        pygame.draw.rect(screen, pygame.Color(BORDER_COLOR), pygame.Rect(x, y, w, h), width=1)


"""
Possible highlight for selected card
"""
def draw_card_highlight(screen, gs, which_card):
    if which_card is not None:
        i = gs.get_id_by_card(which_card)
        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
        pygame.draw.rect(screen, pygame.Color(HIGHLIGHT_COLOR_2), pygame.Rect(x, y, w, h), width=5)

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

