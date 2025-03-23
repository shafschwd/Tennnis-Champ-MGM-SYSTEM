// playerPerformance.cpp
#include "../include/playerPerformance.hpp"

void PlayerPerformance::displayPerformance(Player* player) {
    std::cout << " ========== Player Performance ========== \n";
    std::cout << "Name: " << player->getName() << "\n";
    std::cout << "Matches Won: " << player->getMatchesWon() << "\n";
    std::cout << "Matches Lost: " << player->getMatchesLost() << "\n";
    std::cout << "Winning Percentage: " << std::fixed << std::setprecision(2) << player->getWinningPercentage() << "%\n";
}