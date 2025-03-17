// MatchHistory.cpp
#include "MatchHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Constructor
MatchHistory::MatchHistory() {
    // Initialize any needed variables
}

// Add a new match to history
void MatchHistory::addMatch(const Match& match) {
    matchStack.push(match);
    std::cout << "Match added to history successfully." << std::endl;
}

// View most recent matches (top N matches)
void MatchHistory::viewRecentMatches(int count) {
    if (matchStack.isEmpty()) {
        std::cout << "No matches in history." << std::endl;
        return;
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

    // Create temporary stacks
    Stack<Match> tempStack;
    Stack<Match> resultStack;

    // Search through the stack
    while (!matchStack.isEmpty()) {
        Match currentMatch = matchStack.pop();

        // Check if player name matches
        if (currentMatch.player1.find(playerName) != std::string::npos ||
            currentMatch.player2.find(playerName) != std::string::npos) {
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

// Save match history to file
bool MatchHistory::saveToFile(const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cout << "Error: Could not open file for writing." << std::endl;
        return false;
    }

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
    return true;
}


// Load match history from file
// Load match history from file
bool MatchHistory::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cout << "Error: Could not open file for reading." << std::endl;
        return false;
    }

    // Clear current history
    while (!matchStack.isEmpty()) {
        matchStack.pop();
    }

    std::string line;

    // Skip header line
    std::getline(inFile, line);

    // Read match data
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string token;

        // Parse matchID
        std::getline(ss, token, ',');
        int matchID = std::stoi(token);

        // Parse player1 (trim leading space)
        std::getline(ss, token, ',');
        std::string player1 = token.substr(token[0] == ' ' ? 1 : 0);

        // Parse player2 (trim leading space)
        std::getline(ss, token, ',');
        std::string player2 = token.substr(token[0] == ' ' ? 1 : 0);

        // Parse winner (trim leading space)
        std::getline(ss, token, ',');
        std::string winner = token.substr(token[0] == ' ' ? 1 : 0);

        // Parse score (trim leading space)
        std::getline(ss, token);
        std::string score = token.substr(token[0] == ' ' ? 1 : 0);

        // Create match and add to stack
        Match match(matchID, player1, player2, winner, score);
        matchStack.push(match);
    }

    inFile.close();
    std::cout << "Match history loaded from " << filename << " successfully." << std::endl;
    return true;
}

// Get the total number of matches in history
int MatchHistory::getTotalMatches() const {
    return matchStack.getSize();
}