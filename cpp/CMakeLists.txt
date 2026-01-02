cmake_minimum_required(VERSION 3.10)
project(proxy_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_proxy
    src/main.cpp
    src/tcp_proxy.cpp
)

target_include_directories(tcp_proxy PRIVATE include)
