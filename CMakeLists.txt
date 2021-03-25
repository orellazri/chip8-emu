cmake_minimum_required(VERSION 3.17)
project(chip8_emu)

set(CMAKE_CXX_STANDARD 14)

add_executable(chip8_emu main.cpp chip8.cpp)