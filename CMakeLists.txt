cmake_minimum_required(VERSION 3.14)
project(Blackjack)
set(CMAKE_CXX_STANDARD 17)

# Main executable
add_executable(Blackjack
    src/main.cpp
    src/training.cpp
    src/qlearning.cpp
    src/q_table_validation.cpp
    src/files.cpp
    src/helpers.cpp
)
target_include_directories(Blackjack PRIVATE include)







