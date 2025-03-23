cmake_minimum_required(VERSION 3.30)
project(TCMS)

set(CMAKE_CXX_STANDARD 20)

# Set the directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Include directories
include_directories(include)

# Source files
set(SOURCES
        src/main.cpp
        src/Match.cpp
        src/MatchHistory.cpp
        src/scheduleMatches.cpp
        src/playerPerformance.cpp
        src/TicketManager.cpp
        src/PlayerWithdrawalManager.cpp
)

# Create executable
add_executable(TCMS ${SOURCES})