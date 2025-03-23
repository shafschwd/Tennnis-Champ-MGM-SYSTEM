#ifndef MATCH_H
#define MATCH_H

#include <string>

struct Match {
    int matchID;
    std::string player1;
    std::string player2;
    std::string winner;
    std::string score;

    // Constructor
    Match(int id, std::string p1, std::string p2, std::string w, std::string s);

    // Default constructor
    Match();

    // Display match information
    void displayMatch() const;
};

#endif