import socket

def send_message(host, port, msg):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(msg)

        try:
            data = s.recv(4096)
            print("received: ", data.decode())
        except socket.timeout:
            print("timeout waiting for response")

if __name__ == "__main__":
    send_message("127.0.0.1", 9000, b"hello from python!\n")