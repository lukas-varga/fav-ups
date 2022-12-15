from tkinter import *
from tkinter import messagebox
from functools import partial

import game
import parser
from parser import Cmd
from network import Network


BG_COLOR = game.BACKGROUND_COLOR
BTN_COLOR = game.DARK_COLOR
FONT = game.FONT

WIDTH_LOGIN = 384
HEIGHT_LOGIN = 256

def login(net: Network):
    # Wait for playing
    global waiting
    waiting = False

    global exiting
    exiting = False

    # Init
    global win
    win = Tk()
    win.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN}")
    win.title("Login")
    win.config(bg=BG_COLOR)
    win.resizable(width=False, height=False)
    win.protocol("WM_DELETE_WINDOW", do_nothing)

    # Icon
    icon = PhotoImage(file='pieces/bN.png')
    win.iconphoto(False, icon)

    # Set label for user's instruction
    welcome_label = Label(win, text="Welcome to Onitama", font=(FONT, 15), bg=BG_COLOR)
    welcome_label.pack(pady=15)

    # Set text variables
    global username
    username = StringVar()

    # Set username label
    username_label = Label(win, text="Username", font=(FONT, 12), bg=BG_COLOR)
    username_label.pack(pady=5)

    # Set username entry
    username_entry = Entry(win, textvariable=username)
    username_entry.pack(pady=15)

    # Set play button
    global play_btn
    action_with_arg = partial(login_btn_pressed, net)
    play_btn = Button(win, text="Login", width=10, height=1, bg=BTN_COLOR, font=(FONT, 12), command=action_with_arg)
    play_btn.pack()

    # Set close button
    global close_btn
    close_btn = Button(win, text="Close", width=10, height=1, bg=BTN_COLOR, font=(FONT, 12), command=close_win)
    close_btn.pack(pady=10)

    while True:
        win.update()
        win.update_idletasks()

        if waiting:
            rcv = net.recv_data()
            rcv_arr = parser.parse(rcv)
            return rcv_arr
        if exiting:
            lst = ["EXIT"]
            return lst


"""
Play button pressed
"""
def login_btn_pressed(net: Network):
    usr = str(username.get())
    snd = parser.login(usr)
    net.send_data(snd)

    rcv = net.recv_data()
    rcv_arr = parser.parse(rcv)
    cmd = rcv_arr[0]

    if cmd == Cmd.WAITING.value:
        play_btn['state'] = DISABLED
        close_btn['state'] = DISABLED

        tk_wait = Toplevel(win)
        tk_wait.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN // 2}")
        tk_wait.title("Waiting")
        tk_wait.config(bg=BG_COLOR)
        tk_wait.resizable(width=False, height=False)
        tk_wait.protocol("WM_DELETE_WINDOW", do_nothing)

        # Set username label
        waiting_label = Label(tk_wait, text="Waiting for player...", font=(FONT, 12), bg=BG_COLOR)
        waiting_label.pack(pady=5)

        print("Logged successfully")

        global waiting
        waiting = True

    elif cmd == Cmd.FAILED_LOGIN.value:
        # Waiting label
        messagebox.showinfo("Login Failed", "Username must be shorter than 30 chars and not empty!",)

    elif cmd == Cmd.RECONNECT.value:
        print("Reconnecting...")
        # TODO reconnect
    else:
        # Waiting label
        messagebox.showinfo("Error", "Unknown error")


"""
Define a function to close the window
"""
def close_win():
    win.destroy()

    global exiting
    exiting = True

"""
Not close windows on X click
"""
def do_nothing():
    pass
