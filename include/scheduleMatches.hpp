// scheduleMatches.hpp
#ifndef SCHEDULE_MATCHES_HPP
#define SCHEDULE_MATCHES_HPP

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <queue>
#include "../include/MatchHistory.h"

class Player {
private:
    int id;
    std::string name;
    std::string username;
    std::string password;
    int rank;
    int matchesWon;
    int matchesLost;
    int totalPointsScored;

public:
    Player(int id = -1, std::string name = "", std::string username = "", std::string password = "", int rank = 0);

    int getId() const;
    std::string getName() const;
    std::string getUsername() const;
    std::string getPassword() const;
    int getRank() const;
    int getMatchesWon() const;
    int getMatchesLost() const;
    int getTotalPointsScored() const;

    void setMatchesWon(int wins);
    void setMatchesLost(int losses);
    void setTotalPointsScored(int points);

    double getWinningPercentage() const;
    void display() const;
};

class Match {
public:
    int id;
    Player* player1;
    Player* player2;
    std::string round;
    std::string score;
    Player* winner;
    std::string status;

    Match(int id, Player* p1, Player* p2, std::string round);

    void setScore(std::string score);
    void setWinner(Player* winner);

    void display() const;
};

class TournamentBracket {
public:
    std::string roundType;
    Match* matches[100];
    int matchCount;

    TournamentBracket(std::string type);

    void addMatch(Match* match);
    void display() const;
};

class MatchHistory {
public:
    Match* completedMatches[100];
    int completedCount;

    MatchHistory();

    void addCompletedMatch(Match* match);
    void display() const;
};

class WinnerPriorityQueue {
private:
    struct Node {
        Player* player;
        int wins;
        Node(Player* p, int w) : player(p), wins(w) {}
    };

    static const int MAX_SIZE = 50; // Max 50 players in the tournament
    Node* heap[MAX_SIZE];           // Storage for heap nodes
    int size;

    // Helper functions for heap operations
    int parent(int index) { return (index - 1) / 2; }
    int leftChild(int index) { return 2 * index + 1; }
    int rightChild(int index) { return 2 * index + 2; }

    void swapNodes(int i, int j) {
        Node* temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int p = parent(index);
            if (heap[p]->wins < heap[index]->wins ||
                (heap[p]->wins == heap[index]->wins &&
                    heap[p]->player->getTotalPointsScored() < heap[index]->player->getTotalPointsScored())) {
                swapNodes(index, p);
                index = p;
            }
            else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int maxIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size && (heap[left]->wins > heap[maxIndex]->wins ||
            (heap[left]->wins == heap[maxIndex]->wins &&
                heap[left]->player->getTotalPointsScored() > heap[maxIndex]->player->getTotalPointsScored()))) {
            maxIndex = left;
        }
        if (right < size && (heap[right]->wins > heap[maxIndex]->wins ||
            (heap[right]->wins == heap[maxIndex]->wins &&
                heap[right]->player->getTotalPointsScored() > heap[maxIndex]->player->getTotalPointsScored()))) {
            maxIndex = right;
        }
        if (index != maxIndex) {
            swapNodes(index, maxIndex);
            heapifyDown(maxIndex);
        }
    }

public:
    WinnerPriorityQueue() : size(0) {
        for (int i = 0; i < MAX_SIZE; i++) {
            heap[i] = nullptr;
        }
    }

    ~WinnerPriorityQueue() {
        for (int i = 0; i < size; i++) {
            delete heap[i];
        }
    }

    bool isEmpty() const { return size == 0; }

    void enqueue(Player* player, int wins) {
        if (size >= MAX_SIZE) return; // Queue is full
        heap[size] = new Node(player, wins);
        heapifyUp(size);
        size++;
    }

    Player* dequeue() {
        if (isEmpty()) return nullptr;

        Node* result = heap[0];
        Player* player = result->player;

        // Move the last element to the root
        heap[0] = heap[size - 1];
        size--;
        heap[size] = nullptr;

        if (size > 0) {
            heapifyDown(0);
        }

        delete result; // Free the node
        return player;
    }

    int getSize() const { return size; }
};

// Function declarations
void clearScreen();
void displayMainMenu();
void handleStartMatchMenu(Match* matches[], int matchCount, TournamentBracket& bracket, int matchChoice, MatchHistory& history);
void handlePlayerMenu(Player* players[], int playerCount, std::string& loggedInUsername, Match* matches[], int matchCount);
void generateRoundRobinMatches(Player* players[], int playerCount, Match* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage);
bool areAllMatchesCompleted(Match* matches[], int matchCount, const std::string& stage);
int collectWinners(Match* matches[], int matchCount, const std::string& stage, Player* winners[], int maxWinners, WinnerPriorityQueue& pq);
void generateKnockoutMatches(Player* winners[], int winnerCount, Match* matches[], int& matchCount, int maxMatches, int& matchIDCounter, const std::string& stage);
void runMainMenu(Match** matches, int matchCount, TournamentBracket& bracket, MatchHistory& history);

#endif // SCHEDULE_MATCHES_HPP