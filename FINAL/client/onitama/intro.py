import game
import parser
from parser import Cmd
from components import *
from network import Network

from tkinter import *
from tkinter import messagebox
from functools import partial


# Intro win width in px
WIDTH_LOGIN = 384
# Intro win height in px
HEIGHT_LOGIN = 256


"""
Method for start Intro window and wait for user to fill username which is sent and confirmed by server.
Then user enter game waiting room/lobby. When another player enter room, game starts immediately.
"""
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

    global exiting
    exiting = False

    global win_wait

    global username
    username = ""

    while True:
        win.update()
        win.update_idletasks()

        # Incoming message from server
        server_rx_buffer = []
        result = net.network_data_arrived(server_rx_buffer)
        if result is None:
            exiting = True
        elif net.broken_connection():
            messagebox.showinfo("Broken Conn", "Disconnect!")
            exiting = True
        else:
            for record in server_rx_buffer:
                data = parser.parse(record)
                cmd = data[0]

                # PING | PING
                if cmd == Cmd.PING.value:
                    if data[1] == Cmd.PING.value:
                        net.allow_ping()
                    else:
                        exiting = net.wrong_attempt()

                # WAITING | username
                elif cmd == Cmd.WAITING.value:
                    if len(data) == 2:
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
                    else:
                        exiting = net.wrong_attempt()

                # FAILED_LOGIN | *detailed_message*
                elif cmd == Cmd.FAILED.value:
                    if len(data) == 2:
                        messagebox.showinfo(Cmd.FAILED.value, data[1])
                    else:
                        exiting = net.wrong_attempt()

                # START | P1 (black) | P1 (white) | 5x cards
                elif cmd == Cmd.START.value:
                    if len(data) == 8:
                        # Waiting is not needed anymore
                        if win_wait.winfo_exists():
                            win_wait.destroy()

                        # Hide Login screen
                        win.withdraw()
                        # Play game
                        rematch = game.play(net, data, username, None)
                        # Show Login screen
                        win.deiconify()

                        if rematch:
                            again_mess = parser.prepare_login(username)
                            net.send_data(again_mess)
                        else:
                            exiting = True
                    else:
                        exiting = net.wrong_attempt()

                # RECONNECT
                # black_p | white_p | (5x) cards |
                # is_player_white | white_to_move | (25x) "wP" "wK" "--"
                elif cmd == Cmd.RECONNECT.value:
                    if len(data) == 35:
                        # START | P1 (black) | P2 (white) | 5x cards
                        start_like = ["START", data[1], data[2]]
                        for r in range(3, 8):
                            start_like.append(data[r])

                        # is_player_white | white_to_move | (25x) "wP" "wK" "--"
                        rec_data = []
                        for r in range(8, len(data)):
                            rec_data.append(data[r])

                        # Same as START without Waiting Window!!!
                        # Hide Login screen
                        win.withdraw()
                        rematch = game.play(net, start_like, username, rec_data)
                        # Show Login screen
                        win.deiconify()

                        if rematch:
                            again_mess = parser.prepare_login(username)
                            net.send_data(again_mess)
                        else:
                            exiting = True
                    else:
                        print("ERR: RECONNECT in Intro does not have right number of parameters!")
                        exiting = net.wrong_attempt()

                elif cmd == "WRONG_DATA":
                    print("ERR: WRONG_DATA", f"Data are not parsable!")
                    exiting = net.wrong_attempt()

                else:
                    print(f"ERR: Unknown message in Intro!")
                    exiting = net.wrong_attempt()

        # Exiting to main menu by pressing Close
        if exiting:
            return


"""
Play button pressed and username sent to server
"""
def login_btn_pressed(net: Network, username_str_var):
    # Send LOGIN | name
    global username
    username = str(username_str_var.get())
    data = parser.prepare_login(username)
    net.send_data(data)


"""
Define a function to close the window by clicking button
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
