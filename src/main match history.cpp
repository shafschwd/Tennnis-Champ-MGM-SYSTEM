// main.cpp
#include <iostream>
#include <string>
#include <limits>
#include <algorithm> // for std::all_of
#include "MatchHistory.h"

// Function to trim whitespace from beginning and end of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Function to validate if a string contains only alphabetic characters and spaces
bool isValidName(const std::string& name) {
    if (name.empty()) return false;

    std::string trimmed = trim(name);
    if (trimmed.empty()) return false;

    return std::all_of(trimmed.begin(), trimmed.end(), [](char c) {
        return std::isalpha(c) || c == ' ' || c == '.' || c == '-';
    });
}

// Function to validate match ID (positive integer)
bool isValidMatchID(int id) {
    return id > 0;
}

// Function to validate score format (e.g., "6-4 7-6")
bool isValidScore(const std::string& score) {
    if (score.empty()) return false;

    std::string trimmed = trim(score);
    if (trimmed.empty()) return false;

    // Basic check for score format: should contain numbers, hyphens, and spaces
    bool hasDigit = false;
    bool hasSeparator = false;

    for (char c : trimmed) {
        if (std::isdigit(c)) {
            hasDigit = true;
        } else if (c == '-' || c == ' ') {
            hasSeparator = true;
        } else {
            return false; // Invalid character
        }
    }

    return hasDigit && hasSeparator;
}

// Function to get integer input with validation
int getIntInput(const std::string& prompt, bool (*validator)(int) = nullptr) {
    int value;
    bool valid = false;

    do {
        std::cout << prompt;

        if (std::cin >> value) {
            // Input is an integer
            if (validator == nullptr || validator(value)) {
                valid = true;
            } else {
                std::cout << "Invalid input. Please enter a valid value." << std::endl;
            }
        } else {
            // Input is not an integer
            std::cout << "Error: Please enter a valid integer." << std::endl;
            std::cin.clear(); // Clear error flags
        }

        // Discard any remaining characters in the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (!valid);

    return value;
}

// Function to get string input with validation
std::string getStringInput(const std::string& prompt, bool (*validator)(const std::string&) = nullptr) {
    std::string value;
    bool valid = false;

    do {
        std::cout << prompt;
        std::getline(std::cin, value);

        value = trim(value); // Trim whitespace

        if (validator == nullptr || validator(value)) {
            valid = true;
        } else {
            std::cout << "Invalid input. Please enter a valid value." << std::endl;
        }

    } while (!valid);

    return value;
}

void displayMenu() {
    std::cout << "\n===== MATCH HISTORY SYSTEM =====" << std::endl;
    std::cout << "1. Add New Match" << std::endl;
    std::cout << "2. View Recent Matches" << std::endl;
    std::cout << "3. Search Matches by Player" << std::endl;
    std::cout << "4. Search Match by ID" << std::endl;  // New option
    std::cout << "5. Save Match History to File" << std::endl;
    std::cout << "6. Load Match History from File" << std::endl;
    std::cout << "7. Display Total Matches" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}
// Function to verify file name is valid
bool isValidFilename(const std::string& filename) {
    if (filename.empty()) return false;

    // Check for invalid characters in filename
    const std::string invalidChars = "\\/:*?\"<>|";
    if (filename.find_first_of(invalidChars) != std::string::npos) {
        return false;
    }

    return true;
}

int main() {
    MatchHistory history;

    // Attempt to load the existing match history with error handling
    std::cout << "Attempting to load existing match history..." << std::endl;
    if (history.loadFromFile("match_history.txt")) {
        std::cout << "Match history loaded successfully!" << std::endl;
    } else {
        std::cout << "Note: Starting with an empty match history." << std::endl;
        std::cout << "A new file will be created when you save." << std::endl;
    }

    int choice;
    bool running = true;

    std::cout << "Asia Pacific University Tennis Championship" << std::endl;
    std::cout << "Match History Tracking System" << std::endl;

    while (running) {
        displayMenu();

        // Get menu choice with validation
        choice = getIntInput("", [](int c) { return c >= 0 && c <= 6; });

        switch (choice) {
            case 1: { // Add New Match
                std::cout << "\n----- Add New Match -----" << std::endl;

                // Get match ID with validation
                int matchID = getIntInput("Enter Match ID: ", isValidMatchID);

                // Get player names with validation
                std::string player1 = getStringInput("Enter Player 1 Name: ", isValidName);
                std::string player2 = getStringInput("Enter Player 2 Name: ", isValidName);

                // Ensure player2 is not the same as player1
                while (player2 == player1) {
                    std::cout << "Error: Player 2 cannot be the same as Player 1." << std::endl;
                    player2 = getStringInput("Enter Player 2 Name: ", isValidName);
                }

                // Get winner with validation (must be one of the players)
                std::string winner;
                do {
                    winner = getStringInput("Enter Winner Name: ", isValidName);
                    if (winner != player1 && winner != player2) {
                        std::cout << "Error: Winner must be one of the players." << std::endl;
                    }
                } while (winner != player1 && winner != player2);

                // Get score with validation
                std::string score = getStringInput("Enter Score (e.g., 6-4 7-6): ", isValidScore);

                // Create and add match
                Match newMatch(matchID, player1, player2, winner, score);
                history.addMatch(newMatch);
                break;
            }

            case 2: { // View Recent Matches
                std::cout << "\n----- View Recent Matches -----" << std::endl;

                if (history.getTotalMatches() == 0) {
                    std::cout << "No matches in history to display." << std::endl;
                } else {
                    std::cout << "Enter number of recent matches to view (max "
                              << history.getTotalMatches() << "): ";
                    int count;
                    bool valid = false;

                    do {
                        if (std::cin >> count) {
                            if (count > 0 && count <= history.getTotalMatches()) {
                                valid = true;
                            } else {
                                std::cout << "Please enter a number between 1 and "
                                          << history.getTotalMatches() << ": ";
                            }
                        } else {
                            std::cout << "Invalid input. Please enter a valid number: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                    } while (!valid);

                    // Clear input buffer after getting valid input
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    history.viewRecentMatches(count);
                }
                break;
            }
            case 3: { // Search Matches by Player
                std::cout << "\n----- Search Matches by Player -----" << std::endl;

                if (history.getTotalMatches() == 0) {
                    std::cout << "No matches in history to search." << std::endl;
                } else {
                    std::string playerName = getStringInput("Enter player name to search: ", [](const std::string& s) {
                        return !s.empty();
                    });
                    history.searchMatchesByPlayer(playerName);
                }
                break;
            }

            case 4: { // Search Match by ID - New option
                std::cout << "\n----- Search Match by ID -----" << std::endl;

                if (history.getTotalMatches() == 0) {
                    std::cout << "No matches in history to search." << std::endl;
                } else {
                    std::cout << "Enter match ID to search: ";
                    int matchID;
                    bool valid = false;

                    do {
                        if (std::cin >> matchID) {
                            if (matchID > 0) {
                                valid = true;
                            } else {
                                std::cout << "Match ID must be positive. Please try again: ";
                            }
                        } else {
                            std::cout << "Invalid input. Please enter a valid number: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                    } while (!valid);

                    // Clear input buffer after getting valid input
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    history.searchMatchByID(matchID);
                }
                break;
            }

            case 5: { // Save Match History to File
                std::cout << "\n----- Save Match History to File -----" << std::endl;

                if (history.getTotalMatches() == 0) {
                    std::cout << "Warning: No matches in history to save." << std::endl;
                    char confirm;
                    std::cout << "Do you still want to save an empty file? (y/n): ";
                    std::cin >> confirm;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (confirm != 'y' && confirm != 'Y') {
                        std::cout << "Save operation cancelled." << std::endl;
                        break;
                    }
                }

                std::string filename = getStringInput("Enter filename to save (e.g., match_history.txt): ", isValidFilename);
                history.saveToFile(filename);
                break;
            }

            case 6: { // Load Match History from File
                std::cout << "\n----- Load Match History from File -----" << std::endl;

                if (history.getTotalMatches() > 0) {
                    char confirm;
                    std::cout << "Warning: This will replace the current match history ("
                              << history.getTotalMatches() << " matches)." << std::endl;
                    std::cout << "Do you want to continue? (y/n): ";
                    std::cin >> confirm;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (confirm != 'y' && confirm != 'Y') {
                        std::cout << "Load operation cancelled." << std::endl;
                        break;
                    }
                }

                std::string filename = getStringInput("Enter filename to load (e.g., match_history.txt): ", isValidFilename);
                history.loadFromFile(filename);
                break;
            }
            case 7: { // Display Total Matches
                std::cout << "\n----- Match History Statistics -----" << std::endl;
                std::cout << "Total matches in history: " << history.getTotalMatches() << std::endl;
                break;
            }
            case 0: // Exit
                running = false;
                std::cout << "Exiting Match History System. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please enter a number between 0 and 6." << std::endl;
        }
    }

    return 0;
}