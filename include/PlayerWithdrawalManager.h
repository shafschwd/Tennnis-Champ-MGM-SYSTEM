// PlayerWithdrawalManager.h
#ifndef PLAYER_WITHDRAWAL_MANAGER_H
#define PLAYER_WITHDRAWAL_MANAGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

class TournamentSystem {
private:
    struct Player {
        std::string id;
        std::string name;
        std::string status;
        std::string withdrawReason;
        Player* next = nullptr;
    };

    struct Match {
        std::string matchID, round, player1, player2, date, status;
        Match* next;

        Match(std::string mid, std::string r, std::string p1, std::string p2, std::string d, std::string s);
    };

    Player* playerHead;
    Match* matchHead;

    // Helper methods
    void pause();
    std::string toLower(const std::string& str);
    void updatePlayerFile();
    void updateMatchSchedule(std::string playerName);
    void updateScheduleFile();
    void displayPlayersByStatus(const std::string& statusFilter, const std::string& title);
    int levenshteinDistance(const std::string& a, const std::string& b);

public:
    TournamentSystem();
    ~TournamentSystem();

    void loadSchedule();
    void loadPlayers();
    std::string extractWithdrawReason(const std::string& statusStr);
    void searchPlayer();
    void searchPlayerByID(const std::string& query, Player* playerHead);
    void searchPlayerByName(const std::string& query, Player* playerHead);
    std::string getPlayerNameByID(const std::string& id);
    std::string getPlayerStatusTag(const std::string& playerID);
    void restoreMatchStatus(const std::string& playerID);
    void validateMatchStatuses();
    void showMatchesByStatus();
    void showPlayers(int filter);
    void addReplacementPlayer();
    void modifyPlayerStatus();
};

void runPlayerWithdrawalSystem();

#endif // PLAYER_WITHDRAWAL_MANAGER_H