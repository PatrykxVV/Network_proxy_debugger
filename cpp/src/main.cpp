#include "tcp_proxy.hpp"

int main()
{
    run_proxy(
        9000,
        "127.0.0.1",
        8080
    );

    return 0;
}