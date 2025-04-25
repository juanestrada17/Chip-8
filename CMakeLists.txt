cmake_minimum_required(VERSION 3.10.0)
project(Chip8 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Chip8 main.cpp)

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(Chip8
    SDL2::SDL2
)
