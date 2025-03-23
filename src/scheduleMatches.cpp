// scheduleMatches.cpp - Implementation of tournament scheduling component
#include "../include/scheduleMatches.hpp"

// Player Implementation
Player::Player(int id, std::string name, std::string username, std::string password, int rank)
    : id(id), name(name), username(username), password(password), rank(rank), matchesWon(0), matchesLost(0), totalPointsScored(0) {}

int Player::getId() const { return id; }
std::string Player::getName() const { return name; }
std::string Player::getUsername() const { return username; }
std::string Player::getPassword() const { return password; }
int Player::getRank() const { return rank; }
int Player::getMatchesWon() const { return matchesWon; }
int Player::getMatchesLost() const { return matchesLost; }
int Player::getTotalPointsScored() const { return totalPointsScored; }

void Player::setMatchesWon(int wins) { matchesWon = wins; }
void Player::setMatchesLost(int losses) { matchesLost = losses; }
void Player::setTotalPointsScored(int points) { totalPointsScored = points; }

double Player::getWinningPercentage() const {
    if (matchesWon + matchesLost == 0) return 0.0;
    return (static_cast<double>(matchesWon) / (matchesWon + matchesLost)) * 100.0;
}

void Player::display() const {
    std::cout << "ID: " << id << ", Name: " << name << ", Rank: " << rank
              << ", Wins: " << matchesWon
              << ", Losses: " << matchesLost
              << ", Winning Percentage: " << std::fixed << std::setprecision(2) << getWinningPercentage() << "%\n";
}

// Match Implementation
Match::Match(int id, Player* p1, Player* p2, std::string round)
    : id(id), player1(p1), player2(p2), round(round), score("NOT AVAILABLE"), winner(nullptr), status("UPCOMING") {}

void Match::setScore(std::string score) { this->score = score; }
void Match::setWinner(Player* winner) { this->winner = winner; }

void Match::display() const {
    std::cout << "Match ID: " << id << ", " << player1->getName() << " vs " << player2->getName()
              << ", Stage: " << round
              << ", Score: " << score << "\n";
}

// TournamentBracket Implementation
TournamentBracket::TournamentBracket(std::string type) : roundType(type), matchCount(0) {
    for (int i = 0; i < 100; i++) matches[i] = nullptr;
}

void TournamentBracket::addMatch(Match* match) {
    if (matchCount < 100) matches[matchCount++] = match;
}

void TournamentBracket::display() const {
    std::cout << "Bracket Round: " << roundType << "\n";
    for (int i = 0; i < matchCount; i++) matches[i]->display();
}

// MatchHistory Implementation
MatchHistory::MatchHistory() : completedCount(0) {}

void MatchHistory::addCompletedMatch(Match* match) {
    if (completedCount < 100) completedMatches[completedCount++] = match;
}

void MatchHistory::display() const {
    std::cout << "Match History:\n";
    for (int i = 0; i < completedCount; i++) completedMatches[i]->display();
}

// Clear screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Display main menu
void displayTournamentMenu() {
    std::cout << " ========== Welcome to the APU Tennis Tournament Management System ==========\n";
    std::cout << "1. Tennis Player Account\n";
    std::cout << "2. Start Match\n";
    std::cout << "0. Return to Main Menu\n";
    std::cout << "Enter your choice: ";
}

// Handle start match menu
void handleStartMatchMenu(Match* matches[], int matchCount, TournamentBracket& bracket, int matchChoice, MatchHistory& history) {
    if (matchChoice < 1 || matchChoice > matchCount) {
        clearScreen();
        std::cout << "Invalid match selection.\n";
        return;
    }

    if (matches[matchChoice - 1]->status == "Completed") {
        clearScreen();
        std::cout << "This match has already been simulated.\n";
        return;
    }

    int p1Sets, p2Sets;
    bool validScore = false;
    std::cout << "Enter the number of sets won by each player (best of 3 sets):\n";

    while (!validScore) {
        std::cout << "Sets won by " << matches[matchChoice - 1]->player1->getName() << ": ";
        std::cin >> p1Sets;
        while (std::cin.fail() || p1Sets < 0 || p1Sets > 2) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number between 0 and 2: ";
            std::cin >> p1Sets;
        }

        std::cout << "Sets won by " << matches[matchChoice - 1]->player2->getName() << ": ";
        std::cin >> p2Sets;
        while (std::cin.fail() || p2Sets < 0 || p2Sets > 2) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number between 0 and 2: ";
            std::cin >> p2Sets;
        }

        if ((p1Sets == 2 && (p2Sets == 0 || p2Sets == 1)) || (p2Sets == 2 && (p1Sets == 0 || p1Sets == 1))) {
            validScore = true;
        } else {
            std::cout << "Invalid score. One player must win 2 sets.\n";
        }
    }

    std::string score = std::to_string(p1Sets) + "-" + std::to_string(p2Sets);
    matches[matchChoice - 1]->setScore(score);

    Player* winner = (p1Sets > p2Sets) ? matches[matchChoice - 1]->player1 : matches[matchChoice - 1]->player2;
    matches[matchChoice - 1]->setWinner(winner);

    // Update player stats
    winner->setMatchesWon(winner->getMatchesWon() + 1);

    history.addCompletedMatch(matches[matchChoice - 1]);
    std::cout << "Match result saved.\n";
}

// Handle player menu
void handlePlayerMenu(Player* players[], int playerCount, std::string& loggedInUsername, Match* matches[], int matchCount) {
    clearScreen();
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    Player* loggedInPlayer = nullptr;
    for (int i = 0; i < playerCount; i++) {
        if (players[i]->getUsername() == username && players[i]->getPassword() == password) {
            loggedInPlayer = players[i];
            break;
        }
    }

    if (!loggedInPlayer) {
        std::cout << "Invalid credentials.\n";
        return;
    }

    while (true) {
        clearScreen();
        std::cout << " ========== Player Dashboard ========== \n";
        std::cout << "1. View Performance\n";
        std::cout << "0. Logout\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                loggedInPlayer->display();
                break;
            case 0:
                return;
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}

// Generate round-robin matches
void generateRoundRobinMatches(Player* players[], int playerCount, Match* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage) {
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = i + 1; j < playerCount; j++) {
            if (matchCount >= maxMatches) return;
            matches[matchCount++] = new Match(matchIDCounter++, players[i], players[j], stage);
        }
    }
}

// Check if all matches in a stage are completed
bool areAllMatchesCompleted(Match* matches[], int matchCount, const std::string& stage) {
    for (int i = 0; i < matchCount; i++) {
        if (matches[i]->round == stage && matches[i]->status != "Completed") {
            return false;
        }
    }
    return true;
}

// Collect winners from a stage
int collectWinners(Match* matches[], int matchCount, const std::string& stage, Player* winners[], int maxWinners, WinnerPriorityQueue& pq) {
    int winnerCount = 0;

    if (stage == "Qualifier") {
        // Collect all players who participated in this stage
        Player* participants[50]; // Max 50 players
        int participantCount = 0;
        bool playerAdded[50] = { false }; // Track players to avoid duplicates

        for (int i = 0; i < matchCount; i++) {
            if (matches[i]->round == stage) {
                Player* p1 = matches[i]->player1;
                Player* p2 = matches[i]->player2;
                int p1ID = p1->getId();
                int p2ID = p2->getId();

                if (!playerAdded[p1ID - 1]) {
                    participants[participantCount++] = p1;
                    playerAdded[p1ID - 1] = true;
                }
                if (!playerAdded[p2ID - 1]) {
                    participants[participantCount++] = p2;
                    playerAdded[p2ID - 1] = true;
                }
            }
        }

        // Calculate wins for each participant
        int wins[50] = { 0 }; // Wins for each player (indexed by playerID - 1)
        for (int i = 0; i < matchCount; i++) {
            if (matches[i]->round == stage && matches[i]->status == "Completed") {
                Player* winner = matches[i]->winner;
                if (winner != nullptr) {
                    int winnerID = winner->getId();
                    wins[winnerID - 1]++;
                }
            }
        }

        // Use a WinnerPriorityQueue to select top players based on wins
        for (int i = 0; i < participantCount; i++) {
            int playerID = participants[i]->getId();
            pq.enqueue(participants[i], wins[playerID - 1]);
        }

        // Collect top players (up to maxWinners or a fixed number like 4)
        int maxAdvancing = std::min(participantCount, 4); // Advance top 4 players
        maxAdvancing = std::min(maxAdvancing, maxWinners);

        while (!pq.isEmpty() && winnerCount < maxAdvancing) {
            Player* topPlayer = pq.dequeue();
            if (topPlayer != nullptr) {
                winners[winnerCount++] = topPlayer;
            }
        }
    } else {
        // For knockout stages (Semifinal, Final), collect the winners of each match in the stage
        bool playerAdded[50] = { false }; // Track players to avoid duplicates

        for (int i = 0; i < matchCount; i++) {
            if (matches[i]->round == stage && matches[i]->status == "Completed") {
                Player* winner = matches[i]->winner;
                if (winner != nullptr && winnerCount < maxWinners) {
                    int playerID = winner->getId();
                    if (!playerAdded[playerID - 1]) { // Avoid duplicates
                        winners[winnerCount++] = winner;
                        playerAdded[playerID - 1] = true;
                    }
                }
            }
        }
    }

    return winnerCount;
}

// Generate knockout matches
void generateKnockoutMatches(Player* winners[], int winnerCount, Match* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage) {
    if (winnerCount < 2) {
        std::cout << "Not enough winners (" << winnerCount << ") to generate " << stage << " matches.\n";
        return;
    }

    // Pair winners for the next round
    for (int i = 0; i < winnerCount - 1; i += 2) {
        if (i + 1 < winnerCount && matchCount < maxMatches) {
            matches[matchCount++] = new Match(matchIDCounter++, winners[i], winners[i + 1], stage);
        }
    }

    // If there's an odd number of winners, the last one gets a bye
    if (winnerCount % 2 == 1) {
        std::cout << "Odd number of winners. " << winners[winnerCount - 1]->getName() << " receives a bye to the next round.\n";
    }
}

// Run main menu
void runMainMenu(Match** matches, int matchCount, TournamentBracket& bracket, MatchHistory& history) {
    Player* players[50];
    int playerCount = 6;
    players[0] = new Player(1, "Low", "p1", "s1", 1);
    players[1] = new Player(2, "Chun", "p2", "s2", 2);
    players[2] = new Player(3, "Yian", "p3", "s3", 3);
    players[3] = new Player(4, "Saw", "p4", "s4", 4);
    players[4] = new Player(5, "Yu", "p5", "s5", 5);
    players[5] = new Player(6, "Huang", "p6", "s6", 6);

    int matchIDCounter = 101;
    generateRoundRobinMatches(players, playerCount, matches, matchCount, 100, matchIDCounter, "Qualifier");

    WinnerPriorityQueue pq;
    Player* winners[50];
    int winnerCount = 0;

    int choice;
    do {
        clearScreen();
        displayTournamentMenu();
        std::cin >> choice;

        std::string loggedInUsername;
        switch (choice) {
            case 1:
                handlePlayerMenu(players, playerCount, loggedInUsername, matches, matchCount);
                break;
            case 2: {
                clearScreen();
                if (matchCount == 0) {
                    std::cout << "No matches available.\n";
                    break;
                }

                std::cout << " ========== Upcoming Matches ========== \n";
                for (int i = 0; i < matchCount; i++) {
                    std::cout << (i + 1) << ". ";
                    matches[i]->display();
                }

                int matchChoice;
                std::cout << "Choose a match to start: ";
                std::cin >> matchChoice;

                if (matchChoice < 1 || matchChoice > matchCount) {
                    std::cout << "Invalid match selection.\n";
                    break;
                }

                handleStartMatchMenu(matches, matchCount, bracket, matchChoice, history);

                // Check if all qualifier matches are completed
                if (areAllMatchesCompleted(matches, matchCount, "Qualifier")) {
                    winnerCount = collectWinners(matches, matchCount, "Qualifier", winners, 4, pq);
                    std::cout << "Collected " << winnerCount << " winners for Qualifier.\n";
                    generateKnockoutMatches(winners, winnerCount, matches, matchCount, 100, matchIDCounter, "Semifinal");
                }

                // Check if all semifinal matches are completed
                if (areAllMatchesCompleted(matches, matchCount, "Semifinal")) {
                    winnerCount = collectWinners(matches, matchCount, "Semifinal", winners, 2, pq);
                    std::cout << "Collected " << winnerCount << " winners for Semifinal.\n";
                    generateKnockoutMatches(winners, winnerCount, matches, matchCount, 100, matchIDCounter, "Final");
                }

                // Check if the final match is completed
                if (areAllMatchesCompleted(matches, matchCount, "Final")) {
                    std::cout << "Tournament Champion: " << matches[matchCount - 1]->winner->getName() << "!\n";
                }
                break;
            }
            case 0:
                std::cout << "Returning to main menu...\n";
                break;
            default:
                std::cout << "Invalid choice.\n";
        }

        std::cout << "\nPress any key to proceed";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    } while (choice != 0);

    // Cleanup
    for (int i = 0; i < playerCount; i++) delete players[i];
    for (int i = 0; i < matchCount; i++) delete matches[i];
}