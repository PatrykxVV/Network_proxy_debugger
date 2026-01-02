import socket
import time
import argparse

HOST = "127.0.0.1"
PORT = 9000 # port proxy

# Wysyła dane bez czekania na odpowiedz
def backend_timeout():
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    s.sendall(b"will backend reply?\n")
    print("message sent, waiting...")

    time.sleep(10) # zakladam ze proxy powinno dostac timeout
    s.close()

# Dane wysyłane bardzo wolno
def slow_client():
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    for c in b"slow traffic...\n":
        s.send(bytes([c]))
        time.sleep(1)

    print("[scenario] slow client finished sending")
    s.close()

# Wysyła częściowe dane i nagle zamyka połączenie. 
def disconnect_mid_stream():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST,PORT))

    s.sendall(b"partial message...")
    print("[scenario] disconnecting mid-stream")
    s.close()
    

def main():
    parser = argparse.ArgumentParser(description="TCP proxy scenario tester")
    parser.add_argument(
        "scenario",
        choices=["backend-timeout", "slow-client","disconnect"],
        help="Scenario to run"
    )

    args = parser.parse_args()

    if args.scenario == "backend-timeout":
        backend_timeout()
    elif args.scenario == "slow-client":
        slow_client()
    elif args.scenario == "disconnect":
        disconnect_mid_stream()

if __name__ == "__main__":
    backend_timeout()