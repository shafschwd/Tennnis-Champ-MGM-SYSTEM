cmake_minimum_required(VERSION 3.30)
project(TCMS)

set(CMAKE_CXX_STANDARD 20)

# Set the directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Add the executable
add_executable(TCMS main.cpp
    Match.cpp
    Match.h
    MatchHistory.h
    MatchHistory.cpp
    stack.h
)