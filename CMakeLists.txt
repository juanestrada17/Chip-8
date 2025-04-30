cmake_minimum_required(VERSION 3.10.0)
project(Chip8 VERSION 0.1.0 LANGUAGES C CXX)


find_package(SDL2 CONFIG REQUIRED)
add_executable(Chip8 src/main.cpp src/Chip8.cpp src/GraphicsController.cpp)

target_link_libraries(Chip8
    SDL2::SDL2
)
