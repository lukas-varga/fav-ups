"""
This is main driver file for the game
"""

import pygame

from onitama import onitama_engine
from onitama.network import Network

# Colors
LIGHT_COLOR = "antiquewhite1"
DARK_COLOR = "antiquewhite3"
BACKGROUND_COLOR = "antiquewhite2"
BORDER_COLOR = "black"
HIGHLIGHT_COLOR_1 = "blue"
HIGHLIGHT_COLOR_2 = "yellow"

# Resolution
WIDTH = 1024
HEIGHT = 512
# Dimension of board
DIMENSION = 5
# Square size
SQ_SIZE = HEIGHT // DIMENSION

# Card size
COEFFICIENT = 1/125
CARD_SIZE = (COEFFICIENT * (300 * SQ_SIZE), COEFFICIENT * (174 * SQ_SIZE))

# Card positions
CARD_POS = [
    (WIDTH // 2, 0),
    (WIDTH - CARD_SIZE[0], 0),
    (0.5 * CARD_SIZE[0] + WIDTH // 2, HEIGHT // 2 - CARD_SIZE[1] // 2),
    (WIDTH // 2, HEIGHT - CARD_SIZE[1]),
    (WIDTH - CARD_SIZE[0], HEIGHT - CARD_SIZE[1]),
]

# Others
MAX_FPS = 15
PIECE_IMAGES = {}
CARD_IMAGES = {}

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
Handle user input and update graphics
"""
def main():
    pygame.init()
    clock = pygame.time.Clock()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    screen.fill(pygame.Color(BACKGROUND_COLOR))

    # Game state
    gs = onitama_engine.GameState()

    # Change icon and title
    program_icon = pygame.image.load('pieces/bN.png')
    pygame.display.set_icon(program_icon)
    pygame.display.set_caption('Onitama')

    # Only once before loop
    load_images(gs)
    running = True
    # No square selected, last click of user (row, col)
    sq_selected = ()
    # All the player clicks two tuple [(4,5), (2,2)]
    player_clicks = []

    # Valid moves generated
    valid_moves = gs.get_valid_moves()
    # If user selected card
    is_card_selected = False

    while running:
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False

            # Mouse handler
            elif e.type == pygame.MOUSEBUTTONDOWN:
                # x,y location of mouse
                location = pygame.mouse.get_pos()

                # Clicking on the board
                if is_card_selected and (location[0] <= WIDTH // 2 and location[1] <= HEIGHT):
                    col = location[0] // SQ_SIZE
                    row = location[1] // SQ_SIZE

                    # User clicked at the same square twice
                    if sq_selected == (row, col):
                        sq_selected = ()
                        player_clicks = []
                    else:
                        # Append fst and snd click
                        sq_selected = (row, col)
                        player_clicks.append(sq_selected)

                    # Snd click
                    if len(player_clicks) == 2:
                        move = onitama_engine.Move(player_clicks[0], player_clicks[1], gs.board)
                        print(move.get_chess_like_notation())
                        if move in valid_moves:
                            gs.make_move(move)
                            is_card_selected = True
                        # Reset user clicks
                        sq_selected = ()
                        player_clicks = []

                # Clicking cards on the right side
                elif not is_card_selected:
                    pass

                else:
                    print("error moving")

            # Keyboard handler
            elif e.type == pygame.KEYDOWN:
                # Undo with Z TODO delete on release
                if e.key == pygame.K_z:
                    gs.undo_move()
                    is_card_selected = False

        if is_card_selected:
            valid_moves = gs.get_valid_moves()
            is_card_selected = False
        draw_game_state(screen, gs, valid_moves, sq_selected)
        clock.tick(MAX_FPS)
        pygame.display.flip()


"""
Responsible for all graphics within current game state
"""
def draw_game_state(screen, gs, valid_moves, sq_selected):
    # Draw squares of board
    draw_board(screen)
    # Highlight
    highlight_square(screen, gs, valid_moves, sq_selected)
    # Draw pieces on the top of board
    draw_pieces(screen, gs.board)
    # Draw holders for cards
    draw_card_holders(screen, gs)


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
                    screen.blit(s, (move.end_row * SQ_SIZE, move.end_col * SQ_SIZE))


"""
Draw squares of board
"""
def draw_board(screen):
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
Draw cards
"""
def draw_card_holders(screen, gs):
    for i in range(0, 5):
        name = gs.selected_cards[i]
        x, y, w, h = (CARD_POS[i][0], CARD_POS[i][1], CARD_SIZE[0], CARD_SIZE[1])
        screen.blit(CARD_IMAGES[name], pygame.Rect(x, y, w, h))
        pygame.draw.rect(screen, pygame.Color(BORDER_COLOR), pygame.Rect(x, y, w, h), width=1)


"""
 Main
"""
if __name__ == "__main__":
    main()
