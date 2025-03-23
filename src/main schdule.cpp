#include "scheduleMatches.hpp"

int main() {
    clearScreen();

    MatchHistory history;
    TournamentBracket bracket("Round 1");

    Match* matches[100];
    int matchCount = 0;

    runMainMenu(matches, matchCount, bracket, history);

    return 0;
}