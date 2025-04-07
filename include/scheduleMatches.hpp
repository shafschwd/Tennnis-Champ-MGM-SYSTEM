#ifndef SCHEDULE_MATCHES_HPP
#define SCHEDULE_MATCHES_HPP

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <queue>
#include <algorithm>
#include "MatchHistory.h"

// Helper function
bool equalsIgnoreCase(const std::string& a, const std::string& b);

class Player {
private:
    int id;
    std::string name, username, password;
    int matchesWon, matchesLost, totalPointsScored;

public:
    Player(int id = -1, std::string name = "", std::string username = "", std::string password = "");

    int getId() const;
    std::string getName() const;
    std::string getUsername() const;
    std::string getPassword() const;
    int getMatchesWon() const;
    int getMatchesLost() const;
    int getTotalPointsScored() const;

    void setMatchesWon(int wins);
    void setMatchesLost(int losses);
    void setTotalPointsScored(int points);
    double getWinningPercentage() const;
    void display() const;
};

class TournamentMatch {
public:
    int id;
    Player* player1;
    Player* player2;
    std::string round;
    std::string score;
    Player* winner;
    std::string status;

    TournamentMatch(int id, Player* p1, Player* p2, std::string round);

    void setScore(std::string score);
    void setWinner(Player* winner);
    void display() const;
};

class TournamentBracket {
public:
    std::string roundType;
    TournamentMatch* matches[100];
    int matchCount;

    TournamentBracket(std::string type);
    void addMatch(TournamentMatch* match);
    void display() const;
};

class TournamentMatchHistory {
public:
    TournamentMatch* completedMatches[100];
    int completedCount;

    TournamentMatchHistory();
    void addCompletedMatch(TournamentMatch* match);
    void display() const;
};

class WinnerPriorityQueue {
private:
    struct Node {
        Player* player;
        int wins;
        Node(Player* p, int w) : player(p), wins(w) {}
    };

    static const int MAX_SIZE = 50;
    Node* heap[MAX_SIZE];
    int size;

    int parent(int index);
    int leftChild(int index);
    int rightChild(int index);
    void swapNodes(int i, int j);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    WinnerPriorityQueue();
    ~WinnerPriorityQueue();
    bool isEmpty() const;
    void enqueue(Player* player, int wins);
    Player* dequeue();
    int getSize() const;
};

Match convertToHistoricalMatch(const TournamentMatch* tournamentMatch);

void clearScreen();
void displayTournamentMenu();
void handleStartMatchMenu(TournamentMatch* matches[], int matchCount, TournamentBracket& bracket, int matchChoice, TournamentMatchHistory& history);
void handlePlayerMenu(Player* players[], int playerCount, std::string& loggedInUsername, TournamentMatch* matches[], int matchCount);
void generateRoundRobinMatches(Player* players[], int playerCount, TournamentMatch* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage);
bool areAllMatchesCompleted(TournamentMatch* matches[], int matchCount, const std::string& stage);
int collectWinners(TournamentMatch* matches[], int matchCount, const std::string& stage, Player* winners[], int maxWinners, WinnerPriorityQueue& pq);
void generateKnockoutMatches(Player* winners[], int winnerCount, TournamentMatch* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage);
void autoSimulateQualifierMatches(TournamentMatch* matches[], int& matchCount, TournamentMatchHistory& history);
void autoSimulateStageMatches(TournamentMatch* matches[], int matchCount, const std::string& stage, TournamentMatchHistory& history);
void autoSimulateFullTournament(TournamentMatch* matches[], int& matchCount, TournamentMatchHistory& history, int& matchIDCounter, WinnerPriorityQueue& pq);
void runMainMenu(TournamentMatch** matches, int matchCount, TournamentBracket& bracket, TournamentMatchHistory& history);

#endif // SCHEDULE_MATCHES_HPP