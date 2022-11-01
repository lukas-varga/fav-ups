"""
This is main driver file for the game
"""

import pygame as p
from onitama import onitama_engine

# Resolution
WIDTH = HEIGHT = 384
# Dimension of board
DIMENSION = 5
# Square size
SQ_SIZE = WIDTH // DIMENSION

# Card size
COEFFICIENT = 1/125
CARD_DIM = (COEFFICIENT * (300 * SQ_SIZE), COEFFICIENT * (174 * SQ_SIZE))

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
        PIECE_IMAGES[piece] = p.transform.scale(p.image.load("pieces/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))

    # Load all playing card images using dictionary CARD_IMAGES["wP"] = pieces/wP.png
    card_names = list(gs.cards.keys())
    for card in card_names:
        CARD_IMAGES[card] = p.transform.scale(p.image.load("cards/" + card + ".jpg"), CARD_DIM)


"""
Handle user input and update graphics
"""
def main():
    p.init()
    screen = p.display.set_mode((WIDTH, HEIGHT))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))
    # Game state
    gs = onitama_engine.GameState()

    # Change icon and title
    program_icon = p.image.load('pieces/bN.png')
    p.display.set_icon(program_icon)
    p.display.set_caption('Onitama')

    # Only once before loop
    load_images(gs)
    running = True
    # No square selected, last click of user (row, col)
    sq_selected = ()
    # All the player clicks two tuple [(4,5), (2,2)]
    player_clicks = []
    while running:
        for e in p.event.get():
            if e.type == p.QUIT:
                running = False
            # Mouse handler
            elif e.type == p.MOUSEBUTTONDOWN:
                # x,y location of mouse
                location = p.mouse.get_pos()
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
                    gs.make_move(move)
                    # Reset user clicks
                    sq_selected = ()
                    player_clicks = []
            elif e.type == p.KEYDOWN:
                # Undo with Z TODO delete on release
                if e.key == p.K_z:
                    gs.undo_move()

        draw_game_state(screen, gs)
        clock.tick(MAX_FPS)
        p.display.flip()


"""
Responsible for all graphics within current game state
"""
def draw_game_state(screen, gs):
    # Draw squares of board
    draw_board(screen)
    # TODO make piece highlighting
    # Draw pieces on the top of board
    draw_pieces(screen, gs.board)
    # Draw holders for cards
    draw_card_holders(screen, gs)


"""
Draw squares of board
"""
def draw_board(screen):
    colors = [p.Color("white"), p.Color("lightgray")]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            color = colors[(r + c) % 2]
            p.draw.rect(screen, color, p.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


"""
Draw pieces on the top of board
"""
def draw_pieces(screen, board):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            # Not empty
            if piece != "--":
                screen.blit(PIECE_IMAGES[piece], p.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))



"""
Draw holders for cards
"""
def draw_card_holders(screen, gs):
    for i in range(0, 5):
        name = gs.selected_cards[i]
        screen.blit(CARD_IMAGES[name], p.Rect(0, (i * CARD_DIM[1]), CARD_DIM[0], CARD_DIM[1]))


"""
 Main
"""
if __name__ == "__main__":
    main()
