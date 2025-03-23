// main.cpp - Main entry point for the Tennis Championship Management System
#include <iostream>
#include <string>
#include <limits>

#include "../include/MatchHistory.h"
#include "../include/scheduleMatches.hpp"
#include "../include/TicketManager.h"
#include "../include/PlayerWithdrawalManager.h"

void displayMainMenu() {
    std::cout << "\n========== ASIA PACIFIC UNIVERSITY TENNIS CHAMPIONSHIP MANAGEMENT SYSTEM ==========\n";
    std::cout << "1. Tournament Scheduling and Player Progression\n";
    std::cout << "2. Ticket Sales and Spectator Management\n";
    std::cout << "3. Player Withdrawals Management\n";
    std::cout << "4. Match History Tracking\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    // Initialize components as needed
    MatchHistory matchHistory;
    // Try to load existing match history
    matchHistory.loadFromFile("data/match_history.txt");

    // For the tournament scheduling component
    TournamentBracket bracket("Round 1");
    Match* scheduledMatches[100];
    int matchCount = 0;

    int choice;
    bool running = true;

    std::cout << "Asia Pacific University Tennis Championship Management System\n";
    std::cout << "Version 1.0\n";

    while (running) {
        clearScreen(); // This function is from scheduleMatches.hpp
        displayMainMenu();

        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                // Tournament Scheduling
                runMainMenu(scheduledMatches, matchCount, bracket, matchHistory);
                break;

            case 2:
                // Ticket Sales
                runTicketManager();
                break;

            case 3:
                // Player Withdrawals
                runPlayerWithdrawalSystem();
                break;

            case 4:
                // Match History Tracking
                runMatchHistorySystem(matchHistory);
                break;

            case 0:
                running = false;
                std::cout << "Thank you for using the APU Tennis Championship Management System!\n";
                break;

            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}