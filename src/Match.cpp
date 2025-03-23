// Match.cpp
#include "../include/Match.h"
#include <iostream>

// Constructor
Match::Match(int id, std::string p1, std::string p2, std::string w, std::string s)
    : matchID(id), player1(p1), player2(p2), winner(w), score(s) {
}

// Default constructor
Match::Match()
    : matchID(0), player1(""), player2(""), winner(""), score("") {
}

// Display match information
void Match::displayMatch() const {
    std::cout << "Match ID: " << matchID << std::endl;
    std::cout << "Players: " << player1 << " vs " << player2 << std::endl;
    std::cout << "Winner: " << winner << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "------------------------" << std::endl;
}