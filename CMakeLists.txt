cmake_minimum_required(VERSION 3.14)
project(Blackjack)
set(CMAKE_CXX_STANDARD 17)

# Main executable
add_executable(Blackjack
    src/main.cpp
    src/game.cpp
    src/simulation.cpp
)
target_include_directories(Blackjack PRIVATE include)







