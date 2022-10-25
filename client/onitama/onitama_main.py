"""
This is main driver file for the game
"""

import pygame as p
from onitama import onitama_engine

# Resolution
WIDTH = HEIGHT = 512
# Dimension of board
DIMENSION = 5
# Square size
SQ_SIZE = WIDTH // DIMENSION
MAX_FPS = 15
IMAGES = {}

"""
Global dictionary of images
"""


def load_images():
    # Assigning images using dictionary IMAGES["wP"] = images/wP.png
    pieces = ["wP", "wK", "bP", "bK"]
    for piece in pieces:
        # Scale image using resolution
        IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))


"""
Handle user input and update graphics
"""


def main():
    p.init()
    screen = p.display.set_mode((WIDTH, HEIGHT))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))
    # Game state
    gs = onitama_engine.game_state()
    # Change icon and title
    program_icon = p.image.load('images/bN.png')
    p.display.set_icon(program_icon)
    p.display.set_caption('Onitama')

    # Only once before loop
    load_images()
    running = True
    while running:
        for e in p.event.get():
            if e.type == p.QUIT:
                running = False
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


"""
Draw squares of board
"""
def draw_board(screen):
    colors = [p.Color("white"), p.Color("lightgray")]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            color = colors[(r+c) % 2]
            p.draw.rect(screen, color, p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))


"""
Draw pieces on the top of board
"""
def draw_pieces(screen, board):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            # Not empty
            if piece != "--":
                screen.blit(IMAGES[piece], p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))


"""
 Main
"""
if __name__ == "__main__":
    main()
