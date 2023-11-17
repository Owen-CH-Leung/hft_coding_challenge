cmake_minimum_required(VERSION 3.22)
project(hft_coding_challenge)

set(CMAKE_CXX_STANDARD 17)

add_executable(client main.cpp message.cpp message.h utils.cpp utils.h connect.cpp connect.h)
