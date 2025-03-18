// MatchHistory.cpp
#include "MatchHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

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

    std::ofstream outFile(filename);

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
        std::cout << "Match history saved to " << filename << " successfully." << std::endl;
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

    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cout << "Error: Could not open file for reading." << std::endl;
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

        std::cout << "Match history loaded from " << filename << " successfully." << std::endl;
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