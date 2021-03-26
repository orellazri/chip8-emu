cmake_minimum_required(VERSION 3.17)
project(chip8_emu)

set(CMAKE_CXX_STANDARD 14)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "D:/Code/Lib/SFML251/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(chip8_emu main.cpp chip8.cpp)

target_link_libraries(chip8_emu sfml-graphics sfml-window sfml-system)