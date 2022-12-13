"""
Class for login screen
"""

from tkinter import *
from tkinter import messagebox
from functools import partial
import select

import game
import parser
from parser import Commands


BG_COLOR = game.BACKGROUND_COLOR
BTN_COLOR = game.DARK_COLOR
FONT = game.FONT

WIDTH_LOGIN = 384
HEIGHT_LOGIN = 256

def login(net):
    # Wait for playing
    global logged
    logged = False

    # Init
    global login_scr
    login_scr = Tk()
    login_scr.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN}")
    login_scr.title("Login")
    login_scr.config(bg=BG_COLOR)
    login_scr.resizable(width=False, height=False)
    login_scr.protocol("WM_DELETE_WINDOW", do_nothing)

    # Icon
    icon = PhotoImage(file='pieces/bN.png')
    login_scr.iconphoto(False, icon)

    # Set label for user's instruction
    welcome_label = Label(login_scr, text="Welcome to Onitama", font=(FONT, 15), bg=BG_COLOR)
    welcome_label.pack(pady=15)

    # Set text variables
    global username
    username = StringVar()

    # Set username label
    username_lable = Label(login_scr, text="Username", font=(FONT, 12), bg=BG_COLOR)
    username_lable.pack(pady=5)

    # Set username entry
    username_entry = Entry(login_scr, textvariable=username)
    username_entry.pack(pady=15)

    # Set play button
    global play_btn
    action_with_arg = partial (login_btn_pressed, net)
    play_btn = Button(login_scr, text="Login", width=10, height=1, bg=BTN_COLOR, font=(FONT, 12), command=action_with_arg)
    play_btn.pack()

    # Set close button
    global close_btn
    close_btn = Button(login_scr, text="Close", width=10, height=1, bg=BTN_COLOR, font=(FONT, 12), command=close_win)
    close_btn.pack(pady=10)

    # Main loop
    while True:  # Only exits, because update cannot be used on a destroyed application
        login_scr.update()
        login_scr.update_idletasks()

        # Waiting for the game
        if logged:
            while True:
                sockets_list = [net.client]
                read_sockets, write_socket, error_socket = select.select(sockets_list, [], [])
                for socks in read_sockets:
                    if socks == net.client:
                        rcv = net.recv_data()
                        rcv_arr = parser.parse(rcv)

                        print(rcv_arr)
                        print("Game started")

                        login_scr.destroy()
                        return


"""
Play button pressed
"""
def login_btn_pressed(net):
    usr = str(username.get())
    snd = parser.login(usr)
    net.send_data(snd)

    rcv = net.recv_data()
    rcv_arr = parser.parse(rcv)
    cmd = rcv_arr[0]

    if cmd == Commands.WAITING.value:
        print("Logged successfully")
        play_btn['state'] = DISABLED
        close_btn['state'] = DISABLED

        waiting_scr = Toplevel(login_scr)
        waiting_scr.geometry(f"{WIDTH_LOGIN}x{HEIGHT_LOGIN // 2}")
        waiting_scr.title("Waiting")
        waiting_scr.config(bg=BG_COLOR)
        waiting_scr.resizable(width=False, height=False)
        waiting_scr.protocol("WM_DELETE_WINDOW", do_nothing)

        # Set username label
        waiting_label = Label(waiting_scr, text="Waiting for player...", font=(FONT, 12), bg=BG_COLOR)
        waiting_label.pack(pady=5)

        global logged
        logged = True

    elif cmd == Commands.FAILED_LOGIN.value:
        # Waiting label
        messagebox.showinfo("Login Failed", "Username must be shorter than 30 chars and not empty!",)

    elif cmd == Commands.RECONNECT.value:
        print("Recconecting...")
        # TODO reconnect
    else:
        # Waiting label
        messagebox.showinfo("Error", "Unknown error")


"""
Define a function to close the window
"""
def close_win():
    login_scr.destroy()
    exit()

"""
Not close windows on X click
"""
def do_nothing():
    pass
