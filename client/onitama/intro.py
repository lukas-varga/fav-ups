import parser
from components import *
from parser import Cmd
from network import Network
import game

from tkinter import *
from tkinter import messagebox
from functools import partial
#TODO delete
import random
#TODO delete


WIDTH_LOGIN = 384
HEIGHT_LOGIN = 256

def login(net: Network):
    # Init
    win = Tk()
    win.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN}")
    win.title("Login")
    win.config(bg=BACKGROUND_COLOR)
    win.resizable(width=False, height=False)
    win.protocol("WM_DELETE_WINDOW", do_nothing)

    # Icon
    icon = PhotoImage(file='pieces/bN.png')
    win.iconphoto(False, icon)

    # Set label for user's instruction
    welcome_label = Label(win, text="Welcome to Onitama", font=(FONT, 16), bg=BACKGROUND_COLOR)
    welcome_label.pack(pady=10)

    # Set username label
    info_label = Label(win, text="Username", font=(FONT, 12), bg=BACKGROUND_COLOR)
    info_label.pack()

    # Set text variables
    username_str_var = StringVar()
    username_entry = Entry(win, bg="white", borderwidth=2, textvariable=username_str_var)
    username_entry.pack(pady=20)

    # Set play button
    login_with_args = partial(login_btn_pressed, net, username_str_var)
    login_btn = Button(win, text="Login", width=10, height=1, bg=DARK_COLOR, font=(FONT, 12), command=login_with_args)
    login_btn.pack(pady=5)

    # Set close button
    close_with_args = partial(close_btn_pressed, win)
    close_btn = Button(win, text="Close", width=10, height=1, bg=DARK_COLOR, font=(FONT, 12), command=close_with_args)
    close_btn.pack()

    # #TODO delete on release
    # s = "abcdefghijklmnopqrstuvwxyz"
    # tmp = random.choice(s) + random.choice(s) + random.choice(s) + random.choice(s) + random.choice(s) + random.choice(s)+ random.choice(s)+ random.choice(s)
    # username_entry.insert(0, tmp)
    # login_btn_pressed(net, username_str_var)
    # # TODO delete on release

    global exiting
    exiting = False
    global is_logged
    is_logged = False
    global win_wait
    username = ""

    while True:
        win.update()
        win.update_idletasks()

        # Incoming message from server
        server_rx_buffer = []
        if is_logged and net.network_data_arrived(server_rx_buffer):
            for record in server_rx_buffer:
                data = parser.parse(record)
                cmd = data[0]

                # WAITING | username
                if cmd == Cmd.WAITING.value:
                    print("Intro: Waiting!")
                    username_entry['state'] = DISABLED
                    login_btn['state'] = DISABLED

                    win_wait = Toplevel(win)
                    win_wait.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN // 3}")
                    win_wait.title("Waiting")
                    win_wait.config(bg=BACKGROUND_COLOR)
                    win_wait.resizable(width=False, height=False)
                    win_wait.protocol("WM_DELETE_WINDOW", do_nothing)

                    # Set username label
                    waiting_label = Label(win_wait, text="Waiting for opponent...", font=(FONT, 12), bg=BACKGROUND_COLOR)
                    waiting_label.pack(pady=30)

                    username = data[1]

                # FAILED_LOGIN | *detailed_message*
                elif cmd == Cmd.FAILED_LOGIN.value:
                    print("Intro: Failed login!")
                    messagebox.showinfo(Cmd.FAILED_LOGIN.value, data[1])

                # START | P1 (black) | P1 (white) | 5x cards
                elif cmd == Cmd.START.value:
                    print("Intro: START!")

                    win_wait.destroy()
                    rematch = game.play(net, data, username)

                    if rematch:
                        rematch_mess = parser.prepare_rematch(username)
                        net.send_data(rematch_mess)
                    else:
                        exiting = True

                # RECONNECT
                # is_player_white | white_to_move |
                # black_p | white_p | (5x) cards |
                # (25x) "wP" "wK" "--"
                elif cmd == Cmd.RECONNECT.value:
                    print(data)
                    # TODO PARSE AND REVIVE STATE
                    
                elif cmd == "WRONG_DATA":
                    print("WRONG_DATA", f"Data are not parsable!")
                else:
                    print(f"ERR: Unknown message in Intro")

        # Exiting to main menu by pressing Close
        if exiting:
            return


"""
Play button pressed
"""
def login_btn_pressed(net: Network, username_str_var):
    global is_logged
    is_logged = True

    # Connect
    net.create_connection()
    # Send LOGIN | name
    user = str(username_str_var.get())
    data = parser.prepare_login(user)
    net.send_data(data)


"""
Define a function to close the window
"""
def close_btn_pressed(win):
    global exiting
    exiting = True
    win.destroy()


"""
Not close windows on X click
"""
def do_nothing():
    pass
