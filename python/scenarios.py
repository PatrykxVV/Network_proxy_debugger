import socket
import time

HOST = "127.0.0.1"
PORT = 9000 # port proxy

def backend_timeout():
    # Wysyła dane bez czekania na odpowiedz
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    s.sendall(b"will backend reply?\n")
    print("message sent, waiting...")
    time.sleep(10) # zakladam ze proxy powinno dostac timeout
    s.close()

def slow_timeout():
    # Dane wysyłane bardzo wolno
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    for c in b"slow traffic...\n":
        s.send(bytes([c]))
        time.sleep(1)

    s.close()

if __name__ == "__main__":
    backend_timeout()