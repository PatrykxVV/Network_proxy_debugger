#include "tcp_proxy.hpp"

#include<arpa/inet.h> //sockaddr_in, inet_pton
#include<unistd.h> //close()
#include<cstring>
#include<iostream>
#include<sys/socket.h> // socket(), bind(), listen()

// == Tworzymy socket nasluchujacy ==

static int create_listener(int port)
{
    // Socket TCP  (IPV4)

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(fd);
        return -1;
    }



    // Struktura adresu lokalnego
    sockaddr_in addr{};                             //konstruktor
    addr.sin_family = AF_INET;                      // IPV4
    addr.sin_addr.s_addr = INADDR_ANY;              // Nasluch na wszystkich interfejsach 0.0.0.0
    addr.sin_port = htons(port);                    // wprowadzamy port w kolejnosci sieciowej

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) // Przypisujemy adres do socketu
    {
        perror("bind");
        close(fd);
        return -1;
    }    

    if(listen(fd, 5) < 0)                            //Rozpoczynamy nasluchiwanie, max 5 polaczen
    {
        perror("listen");
        close(fd);
        return -1;
    }          
    
    std::cout << "[proxy] listening on port " << port << '\n';


    return fd;
}



// === PROXY ===

void run_proxy(int listen_port, const char *backend_ip, int backend_port)
{
    int listen_fd = create_listener(listen_port);
    if(listen_fd < 0)
        return;

    std::cout << "[proxy] waiting for client...\n";

    // Accept
    int client_fd = accept(listen_fd, nullptr, nullptr);
    if(client_fd < 0)
    {
        perror("accept");
        close(listen_fd);
        return;
    }

    std::cout << "[proxy] client connected\n";



    close(client_fd);
    close(listen_fd);
}
