#include "tcp_proxy.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <cerrno>
#include <sys/time.h>

// Licznik aktywnych połączeń (współdzielony między wątkami)
// atomic -> brak race condition
static std::atomic<int> active_connections{0};


/// Tworzy socket nasłuchujący (proxy)
static int create_listener(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    // Pozwala szybko ponownie uruchomić proxy
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 5) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    std::cout << "[proxy] listening on port " << port << "\n";
    return fd;
}


/// Proxy łączy się z backendem jako KLIENT TCP
static int connect_backend(const char* ip, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket (backend)");
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(fd);
        return -1;
    }

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}


/// Obsługa JEDNEGO klienta
/// Ta funkcja działa w OSOBNYM WĄTKU
static void handle_client(int client_fd,
                          const char* backend_ip,
                          int backend_port)
{

    std::cerr << "[DEBUG] handle_client started\n";

    // Zwiększamy licznik połączeń
    active_connections.fetch_add(1);
    std::cout << "[proxy] client connected, active="
              << active_connections.load() << "\n";

    // Łączymy się z backendem
    int backend_fd = connect_backend(backend_ip, backend_port);
    if (backend_fd < 0) {
        close(client_fd);
        active_connections.fetch_sub(1);
        return;
    }

    // Ustawiamy timeouty (żeby recv nie wisiał wiecznie)
    timeval tv{};
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(backend_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[4096];

    // Główna pętla forwardingu
    while (true)
    {
        // klient -> backend 
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;

        if (send(backend_fd, buffer, n, 0) < 0)
            break;

        // backend -> klient 
        ssize_t r = recv(backend_fd, buffer, sizeof(buffer), 0);
        if (r <= 0) break;

        if (send(client_fd, buffer, r, 0) < 0)
            break;
    }

    // Sprzątanie połączeń 
    close(backend_fd);
    close(client_fd);

    active_connections.fetch_sub(1);
    std::cout << "[proxy] client disconnected, active="
              << active_connections.load() << "\n";
}



void run_proxy(int listen_port,
               const char* backend_ip,
               int backend_port)
{
    int listen_fd = create_listener(listen_port);
    if (listen_fd < 0)
        return;

    std::cout << "[proxy] waiting for clients...\n";

    // Proxy działa 
    while (true)
    {
        int client_fd = accept(listen_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Każdy klient dostaje OSOBNY WĄTEK
        std::thread(
            handle_client,
            client_fd,
            backend_ip,
            backend_port
        ).detach();
    }
}
