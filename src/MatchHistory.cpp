// MatchHistory.cpp
#include "../include/MatchHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <limits>

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

// Constructor
MatchHistory::MatchHistory() {
    // No additional initialization needed
}

// Add a new match to history
void MatchHistory::addMatch(const Match& match) {
    // Check for duplicate match ID
    Stack<Match> tempStack;
    bool isDuplicate = false;

    while (!matchStack.isEmpty() && !isDuplicate) {
        Match current = matchStack.pop();
        if (current.matchID == match.matchID) {
            isDuplicate = true;
            std::cout << "Warning: Match with ID " << match.matchID << " already exists." << std::endl;
        }
        tempStack.push(current);
    }

    // Restore the stack
    while (!tempStack.isEmpty()) {
        matchStack.push(tempStack.pop());
    }

    if (!isDuplicate) {
        matchStack.push(match);
        std::cout << "Match added to history successfully." << std::endl;
    } else {
        char choice;
        std::cout << "Do you want to replace the existing match? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 'y' || choice == 'Y') {
            // Remove existing match with the same ID
            Stack<Match> tempStack2;
            while (!matchStack.isEmpty()) {
                Match current = matchStack.pop();
                if (current.matchID != match.matchID) {
                    tempStack2.push(current);
                }
            }

            // Restore stack and add new match
            while (!tempStack2.isEmpty()) {
                matchStack.push(tempStack2.pop());
            }

            matchStack.push(match);
            std::cout << "Match replaced successfully." << std::endl;
        } else {
            std::cout << "Operation cancelled. Match not added." << std::endl;
        }
    }
}

// View most recent matches (top N matches)
void MatchHistory::viewRecentMatches(int count) {
    if (matchStack.isEmpty()) {
        std::cout << "No matches in history." << std::endl;
        return;
    }

    // Validate count
    if (count <= 0) {
        std::cout << "Error: Count must be positive." << std::endl;
        return;
    }

    int totalMatches = matchStack.getSize();
    if (count > totalMatches) {
        std::cout << "Note: Requested " << count << " matches, but only "
                  << totalMatches << " are available." << std::endl;
        count = totalMatches;
    }

    // Create a temporary stack to restore the matches later
    Stack<Match> tempStack;
    int displayed = 0;

    std::cout << "\n===== RECENT MATCHES =====" << std::endl;

    // Display matches from the stack (up to count)
    while (!matchStack.isEmpty() && displayed < count) {
        Match currentMatch = matchStack.pop();
        currentMatch.displayMatch();
        tempStack.push(currentMatch);
        displayed++;
    }

    // Restore matches back to original stack
    while (!tempStack.isEmpty()) {
        matchStack.push(tempStack.pop());
    }

    std::cout << displayed << " matches displayed." << std::endl;
}

// Search for matches by player name
void MatchHistory::searchMatchesByPlayer(const std::string& playerName) {
    if (matchStack.isEmpty()) {
        std::cout << "No matches in history." << std::endl;
        return;
    }

    if (playerName.empty()) {
        std::cout << "Error: Player name cannot be empty." << std::endl;
        return;
    }

    // Create temporary stacks
    Stack<Match> tempStack;
    Stack<Match> resultStack;

    // Search through the stack
    while (!matchStack.isEmpty()) {
        Match currentMatch = matchStack.pop();

        // Case-insensitive search
        std::string player1Lower = currentMatch.player1;
        std::string player2Lower = currentMatch.player2;
        std::string searchLower = playerName;

        // Convert to lowercase for case-insensitive comparison
        for (char& c : player1Lower) c = std::tolower(c);
        for (char& c : player2Lower) c = std::tolower(c);
        for (char& c : searchLower) c = std::tolower(c);

        // Check if player name matches
        if (player1Lower.find(searchLower) != std::string::npos ||
            player2Lower.find(searchLower) != std::string::npos) {
            resultStack.push(currentMatch);
        }

        tempStack.push(currentMatch);
    }

    // Restore original stack
    while (!tempStack.isEmpty()) {
        matchStack.push(tempStack.pop());
    }

    // Display search results
    std::cout << "\n===== SEARCH RESULTS FOR '" << playerName << "' =====" << std::endl;

    if (resultStack.isEmpty()) {
        std::cout << "No matches found for " << playerName << std::endl;
        return;
    }

    // Display matches from result stack
    Stack<Match> displayStack;  // To maintain order
    while (!resultStack.isEmpty()) {
        displayStack.push(resultStack.pop());
    }

    int count = 0;
    while (!displayStack.isEmpty()) {
        displayStack.pop().displayMatch();
        count++;
    }

    std::cout << count << " matches found." << std::endl;
}

// Search for a match by ID
bool MatchHistory::searchMatchByID(int matchID) {
    if (matchStack.isEmpty()) {
        std::cout << "No matches in history." << std::endl;
        return false;
    }

    if (matchID <= 0) {
        std::cout << "Error: Match ID must be positive." << std::endl;
        return false;
    }

    // Create temporary stack
    Stack<Match> tempStack;
    bool found = false;

    // Search through the stack
    std::cout << "\n===== SEARCH RESULTS FOR MATCH ID " << matchID << " =====" << std::endl;

    while (!matchStack.isEmpty()) {
        Match currentMatch = matchStack.pop();

        // Check if match ID matches
        if (currentMatch.matchID == matchID) {
            currentMatch.displayMatch();
            found = true;
        }

        tempStack.push(currentMatch);
    }

    // Restore original stack
    while (!tempStack.isEmpty()) {
        matchStack.push(tempStack.pop());
    }

    if (!found) {
        std::cout << "No match found with ID " << matchID << std::endl;
    }

    return found;
}

// Save match history to file
bool MatchHistory::saveToFile(const std::string& filename) {
    // Validate filename
    if (filename.empty()) {
        std::cout << "Error: Filename cannot be empty." << std::endl;
        return false;
    }

    std::string fullPath = "data/" + filename;
    std::ofstream outFile(fullPath);

    if (!outFile.is_open()) {
        std::cout << "Error: Could not open file for writing." << std::endl;
        std::cout << "Make sure you have write permissions for this location." << std::endl;
        return false;
    }

    try {
        // Write header with exact format from the sample file
        outFile << "MatchID, Player1, Player2, Winner, Score" << std::endl;

        // Create temporary stack
        Stack<Match> tempStack;

        // Save all matches to file
        while (!matchStack.isEmpty()) {
            Match currentMatch = matchStack.pop();

            // Write match data to file with the exact format (notice spaces after commas)
            outFile << currentMatch.matchID << ", "
                    << currentMatch.player1 << ", "
                    << currentMatch.player2 << ", "
                    << currentMatch.winner << ", "
                    << currentMatch.score << std::endl;

            tempStack.push(currentMatch);
        }

        // Restore original stack
        while (!tempStack.isEmpty()) {
            matchStack.push(tempStack.pop());
        }

        outFile.close();
        std::cout << "Match history saved to " << fullPath << " successfully." << std::endl;
        std::cout << "Total matches saved: " << matchStack.getSize() << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error occurred while saving file: " << e.what() << std::endl;
        outFile.close();
        return false;
    }
}

// Load match history from file
bool MatchHistory::loadFromFile(const std::string& filename) {
    // Validate filename
    if (filename.empty()) {
        std::cout << "Error: Filename cannot be empty." << std::endl;
        return false;
    }

    std::string fullPath = filename; // Already contains the path
    std::ifstream inFile(fullPath);

    if (!inFile.is_open()) {
        std::cout << "Error: Could not open file for reading: " << fullPath << std::endl;
        std::cout << "Make sure the file exists and you have read permissions." << std::endl;
        return false;
    }

    try {
        // Clear current history
        int oldSize = matchStack.getSize();
        while (!matchStack.isEmpty()) {
            matchStack.pop();
        }

        std::string line;
        int lineCount = 0;
        int matchesLoaded = 0;

        // Skip header line
        if (!std::getline(inFile, line)) {
            throw std::runtime_error("File is empty or has invalid format.");
        }
        lineCount++;

        // Check header format
        if (line.find("MatchID") == std::string::npos ||
            line.find("Player1") == std::string::npos ||
            line.find("Player2") == std::string::npos ||
            line.find("Winner") == std::string::npos ||
            line.find("Score") == std::string::npos) {
            throw std::runtime_error("File header does not match expected format.");
        }

        // Read match data
        while (std::getline(inFile, line)) {
            lineCount++;

            if (line.empty()) {
                std::cout << "Warning: Empty line at line " << lineCount << ", skipping." << std::endl;
                continue;
            }

            std::stringstream ss(line);
            std::string token;

            try {
                // Parse matchID
                if (!std::getline(ss, token, ',')) {
                    throw std::runtime_error("Failed to parse Match ID.");
                }
                int matchID;
                try {
                    matchID = std::stoi(token);
                    if (matchID <= 0) {
                        throw std::runtime_error("Invalid Match ID (must be positive).");
                    }
                } catch (const std::exception&) {
                    throw std::runtime_error("Invalid Match ID format at line " + std::to_string(lineCount));
                }

                // Parse player1 (trim leading space)
                if (!std::getline(ss, token, ',')) {
                    throw std::runtime_error("Failed to parse Player 1 name.");
                }
                std::string player1 = token.substr(token[0] == ' ' ? 1 : 0);
                if (player1.empty()) {
                    throw std::runtime_error("Player 1 name is empty.");
                }

                // Parse player2 (trim leading space)
                if (!std::getline(ss, token, ',')) {
                    throw std::runtime_error("Failed to parse Player 2 name.");
                }
                std::string player2 = token.substr(token[0] == ' ' ? 1 : 0);
                if (player2.empty()) {
                    throw std::runtime_error("Player 2 name is empty.");
                }

                // Parse winner (trim leading space)
                if (!std::getline(ss, token, ',')) {
                    throw std::runtime_error("Failed to parse Winner name.");
                }
                std::string winner = token.substr(token[0] == ' ' ? 1 : 0);
                if (winner.empty()) {
                    throw std::runtime_error("Winner name is empty.");
                }

                // Parse score (trim leading space)
                if (!std::getline(ss, token)) {
                    throw std::runtime_error("Failed to parse Score.");
                }
                std::string score = token.substr(token[0] == ' ' ? 1 : 0);
                if (score.empty()) {
                    throw std::runtime_error("Score is empty.");
                }

                // Validate that winner is one of the players
                if (winner != player1 && winner != player2) {
                    throw std::runtime_error("Winner must be one of the players.");
                }

                // Create match and add to stack
                Match match(matchID, player1, player2, winner, score);
                matchStack.push(match);
                matchesLoaded++;
            }
            catch (const std::exception& e) {
                std::cout << "Warning: " << e.what() << " at line " << lineCount << ", skipping this match." << std::endl;
            }
        }

        inFile.close();

        if (matchesLoaded == 0) {
            std::cout << "Warning: No valid matches were found in the file." << std::endl;
            // Restore old stack if no matches were loaded
            return false;
        }

        std::cout << "Match history loaded from " << fullPath << " successfully." << std::endl;
        std::cout << "Loaded " << matchesLoaded << " matches." << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error occurred while loading file: " << e.what() << std::endl;
        inFile.close();
        return false;
    }
}

// Get the total number of matches in history
int MatchHistory::getTotalMatches() const {
    return matchStack.getSize();
}

// Function to run the match history subsystem
void runMatchHistorySystem(MatchHistory& history) {
    int choice;
    bool running = true;

    std::cout << "Asia Pacific University Tennis Championship" << std::endl;
    std::cout << "Match History Tracking System" << std::endl;

    while (running) {
        std::cout << "\n===== MATCH HISTORY SYSTEM =====" << std::endl;
        std::cout << "1. Add New Match" << std::endl;
        std::cout << "2. View Recent Matches" << std::endl;
        std::cout << "3. Search Matches by Player" << std::endl;
        std::cout << "4. Search Match by ID" << std::endl;
        std::cout << "5. Save Match History to File" << std::endl;
        std::cout << "6. Load Match History from File" << std::endl;
        std::cout << "7. Display Total Matches" << std::endl;
        std::cout << "0. Return to Main Menu" << std::endl;
        std::cout << "Enter your choice: ";

        // Get menu choice with validation
        choice = getIntInput("", [](int c) { return c >= 0 && c <= 7; });

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

            case 4: { // Search Match by ID
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
                history.loadFromFile("data/" + filename);
                break;
            }
            case 7: { // Display Total Matches
                std::cout << "\n----- Match History Statistics -----" << std::endl;
                std::cout << "Total matches in history: " << history.getTotalMatches() << std::endl;
                break;
            }
            case 0: // Return to Main Menu
                running = false;
                std::cout << "Returning to Main Menu..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please enter a number between 0 and 7." << std::endl;
        }
    }
}