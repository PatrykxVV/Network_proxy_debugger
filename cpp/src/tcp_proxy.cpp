#include "tcp_proxy.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>


// Tworzy socket nasłuchujący (proxy)

static int create_listener(int port)
{
    // Tworzymy socket TCP IPV4
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }


    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    // Struktora adresu lokalnego (proxy)
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);


    // Przypisujemy (bind) adres do socketu
    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(fd);
        return -1;
    }


    // Przejscie w tryb listen
    if (listen(fd, 5) < 0)
    {
        perror("listen");
        close(fd);
        return -1;
    }

    std::cout << "[proxy] listening on port " << port << "\n";
    return fd;
}




// Łączymy sie z serwerem backendowym
//Proxy działa tutaj jako KLIENT TCP


static int connect_backend(const char* ip, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket (backend)");
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET; //IPV4
    addr.sin_port = htons(port); //Port backednu

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(fd);
        return -1;
    }

    // Nawiązywanie polaczenia z backendem
    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(fd);
        return -1;
    }

    std::cout << "[proxy] connected to backend\n";
    return fd;
}


void run_proxy(int listen_port, const char* backend_ip, int backend_port)
{
    // Socket nasluchujacy proxy
    int listen_fd = create_listener(listen_port);
    if (listen_fd < 0)
        return;

    std::cout << "[proxy] waiting for client...\n";

    // Akceptujemy polaczenie od client
    // accept tworzy nowy socket client_fd któy reprezentuje polaczenia klient - proxy
    int client_fd = accept(listen_fd, nullptr, nullptr);
    if (client_fd < 0)
    {
        perror("accept");
        close(listen_fd);
        return;
    }

    std::cout << "[proxy] client connected\n";

    // Jesli klient zostal polaczony to laczymy sie z backendem
    int backend_fd = connect_backend(backend_ip, backend_port);
    if (backend_fd < 0)
    {
        close(client_fd);
        close(listen_fd);
        return;
    }

    char buffer[4096];

    while (true)
{
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
        if(n <= 0) 
            break;


        send(backend_fd, buffer, n, 0);

        ssize_t r = recv(backend_fd, buffer, sizeof(buffer), 0);
        if(r <= 0)
            break;

        send(client_fd, buffer, r, 0);
}
    
close(backend_fd);
close(client_fd);
close(listen_fd);

}


