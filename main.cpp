// main.cpp
#include <iostream>
#include <string>
#include "MatchHistory.h"

void displayMenu() {
    std::cout << "\n===== MATCH HISTORY SYSTEM =====" << std::endl;
    std::cout << "1. Add New Match" << std::endl;
    std::cout << "2. View Recent Matches" << std::endl;
    std::cout << "3. Search Matches by Player" << std::endl;
    std::cout << "4. Save Match History to File" << std::endl;
    std::cout << "5. Load Match History from File" << std::endl;
    std::cout << "6. Display Total Matches" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    MatchHistory history;
    int choice;
    bool running = true;

    std::cout << "Asia Pacific University Tennis Championship" << std::endl;
    std::cout << "Match History Tracking System" << std::endl;

    while (running) {
        displayMenu();
        std::cin >> choice;

        // Clear input buffer
        std::cin.ignore();

        switch (choice) {
            case 1: { // Add New Match
                int matchID;
                std::string player1, player2, winner, score;

                std::cout << "Enter Match ID: ";
                std::cin >> matchID;
                std::cin.ignore();

                std::cout << "Enter Player 1 Name: ";
                std::getline(std::cin, player1);

                std::cout << "Enter Player 2 Name: ";
                std::getline(std::cin, player2);

                std::cout << "Enter Winner Name: ";
                std::getline(std::cin, winner);

                std::cout << "Enter Score (e.g., 6-4 7-6): ";
                std::getline(std::cin, score);

                Match newMatch(matchID, player1, player2, winner, score);
                history.addMatch(newMatch);
                break;
            }
            case 2: { // View Recent Matches
                int count;
                std::cout << "Enter number of recent matches to view: ";
                std::cin >> count;
                history.viewRecentMatches(count);
                break;
            }
            case 3: { // Search Matches by Player
                std::string playerName;
                std::cout << "Enter player name to search: ";
                std::getline(std::cin, playerName);
                history.searchMatchesByPlayer(playerName);
                break;
            }
            case 4: { // Save Match History to File
                std::string filename;
                std::cout << "Enter filename to save (e.g., match_history.txt): ";
                std::getline(std::cin, filename);
                history.saveToFile(filename);
                break;
            }
            case 5: { // Load Match History from File
                std::string filename;
                std::cout << "Enter filename to load (e.g., match_history.txt): ";
                std::getline(std::cin, filename);
                history.loadFromFile(filename);
                break;
            }
            case 6: { // Display Total Matches
                std::cout << "Total matches in history: " << history.getTotalMatches() << std::endl;
                break;
            }
            case 0: // Exit
                running = false;
                std::cout << "Exiting Match History System. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}