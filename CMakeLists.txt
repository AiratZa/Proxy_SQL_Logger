cmake_minimum_required(VERSION 3.17)
project(proxy_sql)

set(CMAKE_CXX_STANDARD 98)

add_executable(proxy_sql main.cpp ProxyServer.cpp ProxyServer.hpp ProxyServer.hpp Client.cpp Client.hpp utils.cpp utils.hpp Config.cpp Config.hpp)