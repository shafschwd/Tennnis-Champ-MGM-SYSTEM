// MatchHistory.h
#ifndef MATCH_HISTORY_H
#define MATCH_HISTORY_H

#include "Match.h"
#include "Stack.h" 

class MatchHistory {
private:
    Stack<Match> matchStack;  // Stack to store match history

public:
    // Constructor
    MatchHistory();

    // Add a new match to history
    void addMatch(const Match& match);

    // View most recent matches (top N matches)
    void viewRecentMatches(int count);

    // Search for matches by player name
    void searchMatchesByPlayer(const std::string& playerName);

    // Search for a match by ID
    bool searchMatchByID(int matchID);

    // Save match history to file
    bool saveToFile(const std::string& filename);

    // Load match history from file
    bool loadFromFile(const std::string& filename);

    // Get the total number of matches in history
    int getTotalMatches() const;
};

// Standalone function to run match history system
void runMatchHistorySystem(MatchHistory& history);

#endif // MATCH_HISTORY_H