cmake_minimum_required(VERSION 3.10)
project(proxy_cpp)

set(CMAKER_CXX_STANDARD 17)

add_executable(main
        src/main.cpp
)

add_executable(main
        src/tcp_proxy.cpp
)