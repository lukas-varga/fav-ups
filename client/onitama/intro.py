import game
import parser
from parser import Cmd
from network import Network

#TODO delete
import random
#TODO delete
from tkinter import *
from tkinter import messagebox
from functools import partial


BG_COLOR = game.BACKGROUND_COLOR
BTN_COLOR = game.DARK_COLOR
FONT = game.FONT

WIDTH_LOGIN = 384
HEIGHT_LOGIN = 256

def login(net: Network):
    # Wait for playing
    global is_waiting
    is_waiting = False
    global exiting
    exiting = False
    global username
    username = ""

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
    welcome_label = Label(win, text="Welcome to Onitama", font=(FONT, 16), bg=BG_COLOR)
    welcome_label.pack(pady=15)

    # Set text variables
    global username_str_var
    username_str_var = StringVar()

    # Set username label
    username_label = Label(win, text="Username", font=(FONT, 12), bg=BG_COLOR)
    username_label.pack(pady=5)

    # Set username entry
    username_entry = Entry(win, textvariable=username_str_var)
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

    # Not correctly connected
    if net.id == -1:
        username_entry['state'] = DISABLED
        play_btn['state'] = DISABLED
        messagebox.showinfo("Server error", "Connection refused!")

    # TODO delete on release
    login_btn_pressed(net)
    # TODO

    while True:
        win.update()
        win.update_idletasks()

        if is_waiting:
            # START | P1 (white) | P1 (black) | 5x cards
            start = net.recv_data()
            start_arr = parser.parse(start)

            return start_arr, username
        if exiting:
            return None, None


"""
Play button pressed
"""
def login_btn_pressed(net: Network):
    usr = str(username_str_var.get())

    #TODO delete on release
    string = "striver"
    usr = random.choice(string) + random.choice(string) + random.choice(string) + random.choice(string) + random.choice(string) + random.choice(string)+ random.choice(string)+ random.choice(string)
    #TODO delete on release

    snd = parser.send_login(usr)
    net.send_data(snd)

    waiting = net.recv_data()
    waiting_arr = parser.parse(waiting)
    cmd = waiting_arr[0]
    wait_name = waiting_arr[1]

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
        waiting_label = Label(tk_wait, text="Waiting for opponent...", font=(FONT, 12), bg=BG_COLOR)
        waiting_label.pack(pady=5)

        global username
        username = wait_name
        print("In waiting room!")

        global is_waiting
        is_waiting = True

    elif cmd == Cmd.FAILED_LOGIN.value:
        messagebox.showinfo("Login Failed", wait_name)
    elif cmd == Cmd.RECONNECT.value:
        messagebox.showinfo("Reconnecting...", wait_name)
        # TODO reconnect
    else:
        # Waiting label
        messagebox.showinfo("Error", "Unknown error!")


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
