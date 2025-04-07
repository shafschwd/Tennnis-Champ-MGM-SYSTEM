// PlayerWithdrawalManager.cpp
#include "../include/PlayerWithdrawalManager.h"

// ===================== Queue Method Definitions =====================

void PlayerQueue::enqueue(TournamentSystem::Player* player) {
    PlayerQueueNode* newNode = new PlayerQueueNode{ player, nullptr };
    if (rear) rear->next = newNode;
    else front = newNode;
    rear = newNode;
}

TournamentSystem::Player* PlayerQueue::dequeue() {
    if (!front) return nullptr;
    PlayerQueueNode* temp = front;
    TournamentSystem::Player* player = temp->data;
    front = front->next;
    if (!front) rear = nullptr;
    delete temp;
    return player;
}

bool PlayerQueue::isEmpty() const {
    return front == nullptr;
}


TournamentSystem::Match::Match(std::string mid, std::string r, std::string p1, std::string p2, std::string d, std::string s) {
    matchID = mid;
    round = r;
    player1 = p1;
    player2 = p2;
    date = d;
    status = s;
    next = nullptr;
}

void TournamentSystem::pause() {
    std::cout << "\nPress Enter to return to menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string TournamentSystem::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = tolower(c);
    }
    return result;
}

void TournamentSystem::updatePlayerFile() {
    std::ofstream file("data/player_list.txt");
    if (!file) {
        std::cerr << "Error: Unable to open player_list.txt for writing.\n";
        return;
    }
    file << "PlayerID, Name, Status\n";
    Player* current = playerHead;
    while (current) {
        file << current->id << ", " << current->name << ", " << current->status;
        if (current->status == "Withdrawn")
            file << " (" << current->withdrawReason << ")";
        file << "\n";
        current = current->next;
    }
    file.close();
    std::cout << "Player list successfully updated in player_list.txt.\n";
}

void TournamentSystem::updateMatchSchedule(std::string playerName) {
    Match* current = matchHead;
    int cancelledCount = 0;

    while (current != nullptr) {
        if ((current->player1 == playerName || current->player2 == playerName) &&
            (current->status == "Upcoming" || current->status == "Updated")) {
            current->status = "Cancelled";
            cancelledCount++;
        }
        current = current->next;
    }

    updateScheduleFile(); // Save changes to file
    std::cout << cancelledCount << " match(es) involving \"" << playerName << "\" were cancelled.\n";
}

void TournamentSystem::updateScheduleFile() {
    std::ofstream file("data/schedule.txt");
    if (!file) {
        std::cerr << "Error: Unable to open schedule.txt for writing.\n";
        return;
    }
    file << "MatchID, Round, Player1, Player2, Date, Status (Upcoming/Completed)\n";
    Match* current = matchHead;
    while (current) {
        file << current->matchID << ", "
            << current->round << ", "
            << current->player1 << ", "
            << current->player2 << ", "
            << current->date << ", "
            << current->status << "\n";
        current = current->next;
    }
    file.close();
    std::cout << "Match schedule successfully updated in schedule.txt.\n";
}

void TournamentSystem::displayPlayersByStatus(const std::string& statusFilter, const std::string& title) {
    Player* current = playerHead;
    int count = 0;

    std::cout << title << std::endl;
    std::cout << "----------------------------------------------------------------------------\n";
    std::cout << "| No. | Player Name         | ID       | Status                            |\n";
    std::cout << "----------------------------------------------------------------------------\n";

    while (current) {
        if (current->status == statusFilter) {
            std::string fullStatus = current->status;
            if (current->status == "Withdrawn" && !current->withdrawReason.empty())
                fullStatus += " (" + current->withdrawReason + ")";

            std::cout << "| " << std::left << std::setw(3) << ++count << " | "
                << std::left << std::setw(20) << current->name << "| "
                << std::setw(8) << current->id << "| "
                << std::setw(30) << fullStatus << "\n";
        }
        current = current->next;
    }

    std::cout << "----------------------------------------------------------------------------\n";
}

TournamentSystem::TournamentSystem() {
    playerHead = nullptr;
    matchHead = nullptr;
}

TournamentSystem::~TournamentSystem() {
    while (playerHead) {
        Player* temp = playerHead;
        playerHead = playerHead->next;
        delete temp;
    }
    while (matchHead) {
        Match* temp = matchHead;
        matchHead = matchHead->next;
        delete temp;
    }
}

void TournamentSystem::loadSchedule() {
    std::ifstream file("data/schedule.txt");
    if (!file) {
        std::cerr << "Error: Unable to open schedule.txt\n";
        return;
    }
    std::string line;
    getline(file, line);
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string matchID, round, player1, player2, date, status;
        getline(ss, matchID, ',');
        getline(ss >> std::ws, round, ',');
        getline(ss >> std::ws, player1, ',');
        getline(ss >> std::ws, player2, ',');
        getline(ss >> std::ws, date, ',');
        getline(ss >> std::ws, status);
        Match* newMatch = new Match(matchID, round, player1, player2, date, status);
        newMatch->next = matchHead;
        matchHead = newMatch;
    }
    file.close();
}

std::string TournamentSystem::extractWithdrawReason(const std::string& statusStr) {
    size_t start = statusStr.find("(");
    size_t end = statusStr.find(")");
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return statusStr.substr(start + 1, end - start - 1);
    }
    return "";
}

void TournamentSystem::loadPlayers() {
    std::ifstream file("data/player_list.txt");
    if (!file) {
        std::cerr << "Error: Unable to open player_list.txt\n";
        return;
    }
    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        std::stringstream ss(line);
        Player* newPlayer = new Player;
        getline(ss, newPlayer->id, ',');
        getline(ss >> std::ws, newPlayer->name, ',');
        getline(ss >> std::ws, newPlayer->status);

        // Remove potential trailing newline or whitespace
        newPlayer->status.erase(remove(newPlayer->status.begin(), newPlayer->status.end(), '\r'), newPlayer->status.end());
        newPlayer->status.erase(remove(newPlayer->status.begin(), newPlayer->status.end(), '\n'), newPlayer->status.end());

        // Extract withdraw reason if present
        if (newPlayer->status.find("Withdrawn") != std::string::npos) {
            newPlayer->withdrawReason = extractWithdrawReason(newPlayer->status);
            newPlayer->status = "Withdrawn";
        }

        newPlayer->next = playerHead;
        playerHead = newPlayer;
    }
    file.close();
}

int TournamentSystem::levenshteinDistance(const std::string& a, const std::string& b) {
    int lenA = a.length();
    int lenB = b.length();
    int dp[101][101] = {};

    for (int i = 0; i <= lenA; ++i) dp[i][0] = i;
    for (int j = 0; j <= lenB; ++j) dp[0][j] = j;

    for (int i = 1; i <= lenA; ++i) {
        for (int j = 1; j <= lenB; ++j) {
            if (tolower(a[i - 1]) == tolower(b[j - 1])) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = 1 + std::min(std::min(dp[i - 1][j], dp[i][j - 1]), dp[i - 1][j - 1]);
            }
        }
    }
    return dp[lenA][lenB];
}

void TournamentSystem::searchPlayer() {
    int choice;
    while (true) {
        std::cout << "\n===== Search Player =====\n";
        std::cout << "1. Search by Player ID\n";
        std::cout << "2. Search by Player Name\n";
        std::cout << "3. Return to main menu\n";
        std::cout << "Enter choice: ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore();

        if (choice == 3) return;

        std::string query;
        if (choice == 1) {
            std::cout << "Enter Player ID: ";
            getline(std::cin, query);
            searchPlayerByID(query, playerHead);
        }
        else if (choice == 2) {
            std::cout << "Enter Player Name: ";
            getline(std::cin, query);
            searchPlayerByName(query, playerHead);
        }
        else {
            std::cout << "Invalid choice. Try again.\n";
            continue;
        }

        // Ask user to search again or return
        while (true) {
            int next;
            std::cout << "\n1. Search again\n2. Return to main menu\nEnter choice: ";
            if (!(std::cin >> next)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            std::cin.ignore();

            if (next == 1) break; // go back to top of outer while loop
            else if (next == 2) return;
            else std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void TournamentSystem::searchPlayerByID(const std::string& query, TournamentSystem::Player* playerHead) {
    std::cout << "\n===== Search Results by ID =====\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "| No. | Player Name       | ID    | Status         |\n";
    std::cout << "--------------------------------------------------\n";
    int count = 0;
    TournamentSystem::Player* current = playerHead;
    while (current) {
        if (current->id == query) {
            std::cout << "| " << std::setw(3) << ++count << " | "
                << std::left << std::setw(17) << current->name << "| "
                << std::setw(5) << current->id << " | "
                << std::setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
        }
        current = current->next;
    }
    if (count == 0) {
        std::cout << "No player found with ID: " << query << "\n";
    }
    else {
        std::cout << "--------------------------------------------------\n";
    }
}

void TournamentSystem::searchPlayerByName(const std::string& query, TournamentSystem::Player* playerHead) {
    std::cout << "\n===== Search Results by Name =====\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "| No. | Player Name       | ID    | Status         |\n";
    std::cout << "--------------------------------------------------\n";

    int count = 0;
    TournamentSystem::Player* current = playerHead;

    // Exact or substring match
    while (current) {
        if (toLower(current->name).find(toLower(query)) != std::string::npos) {
            std::cout << "| " << std::setw(3) << ++count << " | "
                << std::left << std::setw(17) << current->name << "| "
                << std::setw(5) << current->id << " | "
                << std::setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
        }
        current = current->next;
    }

    if (count == 0) {
        std::cout << "No exact match found for: " << query << "\n";
        std::cout << "\nShowing similar results:\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "| No. | Player Name       | ID    | Status         |\n";
        std::cout << "--------------------------------------------------\n";

        current = playerHead;
        while (current) {
            std::string firstName = current->name.substr(0, current->name.find(" "));
            int dist = levenshteinDistance(toLower(query), toLower(firstName));

            if (dist <= 2) {
                std::cout << "| " << std::setw(3) << ++count << " | "
                    << std::left << std::setw(17) << current->name << "| "
                    << std::setw(5) << current->id << " | "
                    << std::setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
            }
            current = current->next;
        }

        if (count == 0) std::cout << "No similar names found.\n";
        else std::cout << "--------------------------------------------------\n";
    }
    else {
        std::cout << "--------------------------------------------------\n";
    }
}

std::string TournamentSystem::getPlayerNameByID(const std::string& id) {
    Player* current = playerHead;
    while (current) {
        if (current->id == id) return current->name;
        current = current->next;
    }
    return id;
}

std::string TournamentSystem::getPlayerStatusTag(const std::string& playerID) {
    Player* current = playerHead;
    while (current) {
        if (current->id == playerID) {
            if (current->status == "Withdrawn") {
                if (!current->withdrawReason.empty()) {
                    return " (Withdrawn - " + current->withdrawReason + ")";
                }
                return " (Withdrawn)";
            }
        }
        current = current->next;
    }
    return "";
}

void TournamentSystem::restoreMatchStatus(const std::string& playerID) {
    Match* current = matchHead;
    while (current) {
        if ((current->player1 == playerID || current->player2 == playerID) &&
            current->status == "Cancelled") {
            current->status = "Updated";
        }
        current = current->next;
    }
    updateScheduleFile(); // Save changes
}

void TournamentSystem::validateMatchStatuses() {
    Match* currentMatch = matchHead;
    while (currentMatch) {
        // Check both players' current status by name
        Player* p1 = playerHead;
        Player* p2 = playerHead;
        std::string status1 = "Active", status2 = "Active";

        while (p1) {
            if (p1->name == currentMatch->player1) {
                status1 = p1->status;
                break;
            }
            p1 = p1->next;
        }

        while (p2) {
            if (p2->name == currentMatch->player2) {
                status2 = p2->status;
                break;
            }
            p2 = p2->next;
        }

        // Cancel match if either player is Withdrawn
        if ((status1 == "Withdrawn" || status2 == "Withdrawn") &&
            (currentMatch->status == "Upcoming" || currentMatch->status == "Updated")) {
            currentMatch->status = "Cancelled";
        }

        currentMatch = currentMatch->next;
    }

    updateScheduleFile();
}

void TournamentSystem::showMatchesByStatus() {
    while (true) {
        std::cout << "\n===== View Matches by Status =====\n";
        std::cout << "Select match status to view:\n";
        std::cout << "1. All\n2. Upcoming\n3. Cancelled\n4. Updated\nEnter choice: ";

        int filterChoice;
        std::cin >> filterChoice;
        std::cin.ignore();

        std::string header;
        if (filterChoice == 1) header = "All";
        else if (filterChoice == 2) header = "Upcoming";
        else if (filterChoice == 3) header = "Cancelled";
        else if (filterChoice == 4) header = "Updated";
        else {
            std::cout << "Invalid choice. Returning to main menu.\n";
            return;
        }

        // Collect matches that match the filter
        int count = 0;
        Match* current = matchHead;
        while (current) {
            bool include = (filterChoice == 1) ||
                (filterChoice == 2 && current->status == "Upcoming") ||
                (filterChoice == 3 && current->status == "Cancelled") ||
                (filterChoice == 4 && current->status == "Updated");
            if (include) count++;
            current = current->next;
        }

        if (count == 0) {
            std::cout << "\nNo matches found for the selected status.\n";

            int option;
            do {
                std::cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
                std::cin >> option;
                std::cin.ignore();
                if (option == 1)
                    return showMatchesByStatus(); // re-enter the filter menu
                else if (option == 2)
                    return;
                else
                    std::cout << "Invalid choice. Try again.\n";
            } while (option != 1 && option != 2);
        }

        Match** matches = new Match * [count];
        current = matchHead;
        int i = 0;
        while (current) {
            bool include = (filterChoice == 1) ||
                (filterChoice == 2 && current->status == "Upcoming") ||
                (filterChoice == 3 && current->status == "Cancelled") ||
                (filterChoice == 4 && current->status == "Updated");
            if (include) matches[i++] = current;
            current = current->next;
        }

        // Sort matches by MatchID (ascending)
        for (int x = 0; x < count - 1; ++x) {
            for (int y = 0; y < count - x - 1; ++y) {
                if (matches[y]->matchID > matches[y + 1]->matchID) {
                    Match* temp = matches[y];
                    matches[y] = matches[y + 1];
                    matches[y + 1] = temp;
                }
            }
        }

        // Display sorted matches
        std::cout << "\n===== Matches with Status: " << header << " =====\n";
        std::cout << "----------------------------------------------------------------------------------------------------\n";
        std::cout << "| No. | Match ID | Round         | Player 1                 | Player 2                 | Date       | Status     |\n";
        std::cout << "----------------------------------------------------------------------------------------------------\n";

        for (int j = 0; j < count; ++j) {
            std::string p1Name = getPlayerNameByID(matches[j]->player1);
            std::string p2Name = getPlayerNameByID(matches[j]->player2);
            std::string p1Full = p1Name + getPlayerStatusTag(matches[j]->player1);
            std::string p2Full = p2Name + getPlayerStatusTag(matches[j]->player2);

            std::cout << std::left
                << "| " << std::setw(3) << j + 1 << " | "
                << std::setw(8) << matches[j]->matchID << " | "
                << std::setw(13) << matches[j]->round << " | "
                << std::setw(25) << p1Full << " | "
                << std::setw(25) << p2Full << " | "
                << std::setw(10) << matches[j]->date << " | "
                << std::setw(10) << matches[j]->status << " |\n";
        }

        std::cout << "----------------------------------------------------------------------------------------------------\n";
        delete[] matches;

        int option;
        do {
            std::cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
            std::cin >> option;
            std::cin.ignore();
            if (option == 2) return;
            else if (option != 1) std::cout << "Invalid choice. Try again.\n";
        } while (option != 1);
    }
}

void TournamentSystem::showPlayers(int filter) {
    // Count nodes
    int count = 0;
    Player* current = playerHead;
    while (current) {
        if ((filter == 1) ||
            (filter == 2 && current->status == "Active") ||
            (filter == 3 && current->status == "Withdrawn")) {
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        std::cout << "No players found for the selected category.\n";
        return;
    }

    // Copy filtered players into array
    Player** players = new Player * [count];
    current = playerHead;
    int i = 0;
    while (current) {
        if ((filter == 1) ||
            (filter == 2 && current->status == "Active") ||
            (filter == 3 && current->status == "Withdrawn")) {
            players[i++] = current;
        }
        current = current->next;
    }

    // Bubble sort by ID
    for (int x = 0; x < count - 1; ++x) {
        for (int y = 0; y < count - x - 1; ++y) {
            if (players[y]->id > players[y + 1]->id) {
                Player* temp = players[y];
                players[y] = players[y + 1];
                players[y + 1] = temp;
            }
        }
    }

    // Display
    std::cout << "\n===== Player List =====\n";
    std::cout << "----------------------------------------------------------------------------\n";
    std::cout << "| No. | Player Name         | ID       | Status                            |\n";
    std::cout << "----------------------------------------------------------------------------\n";

    for (int j = 0; j < count; ++j) {
        std::string fullStatus = players[j]->status;
        if (players[j]->status == "Withdrawn" && !players[j]->withdrawReason.empty())
            fullStatus += " (" + players[j]->withdrawReason + ")";

        std::cout << "| " << std::left << std::setw(3) << j + 1 << " | "
            << std::left << std::setw(20) << players[j]->name << "| "
            << std::setw(8) << players[j]->id << "| "
            << std::setw(30) << fullStatus << "\n";
    }

    std::cout << "----------------------------------------------------------------------------\n";
    delete[] players;

    int option;
    do {
        std::cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
        std::cin >> option;
        std::cin.ignore();
        if (option == 1) {
            std::cout << "\nView players by:\n";
            std::cout << "1. All\n2. Active\n3. Withdrawn\nEnter choice: ";
            int filterChoice;
            std::cin >> filterChoice;
            std::cin.ignore();
            if (filterChoice >= 1 && filterChoice <= 3)
                showPlayers(filterChoice);
            else
                std::cout << "Invalid choice.\n";
            return;
        }
        else if (option == 2) {
            return;
        }
        else {
            std::cout << "Invalid choice. Try again.\n";
        }
    } while (option != 2);
}

void TournamentSystem::addReplacementPlayer() {
    while (true) {
        std::cout << "\n===== Add Replacement Player (Queue Implementation) =====\n";

        // Create queue for Withdrawn players
        PlayerQueue withdrawnQueue;
        int withdrawnCount = 0;

        // Fill queue
        Player* current = playerHead;
        while (current) {
            if (current->status == "Withdrawn") {
                withdrawnQueue.enqueue(current);
                withdrawnCount++;
            }
            current = current->next;
        }

        if (withdrawnCount == 0) {
            std::cout << "There are no withdrawn players to process.\n";
            return;
        }

        // Build array for active players
        int activeCount = 0;
        current = playerHead;
        while (current) {
            if (current->status == "Active") activeCount++;
            current = current->next;
        }

        Player** active = new Player * [activeCount];
        int ai = 0;
        current = playerHead;
        while (current) {
            if (current->status == "Active") active[ai++] = current;
            current = current->next;
        }

        // Sort active array by ID
        for (int i = 0; i < activeCount - 1; ++i)
            for (int j = 0; j < activeCount - i - 1; ++j)
                if (active[j]->id > active[j + 1]->id)
                    std::swap(active[j], active[j + 1]);

        // Process queue
        while (!withdrawnQueue.isEmpty()) {
            Player* withdrawnPlayer = withdrawnQueue.dequeue();

            std::cout << "\nWithdrawn Player: " << withdrawnPlayer->name << " (" << withdrawnPlayer->id << ") - Reason: " << withdrawnPlayer->withdrawReason << "\n";

            std::cout << "\nActive Players:\n";
            std::cout << "----------------------------------------------------------------------------\n";
            std::cout << "| No. | Player Name         | ID       | Status                            |\n";
            std::cout << "----------------------------------------------------------------------------\n";
            for (int i = 0; i < activeCount; ++i) {
                std::cout << "| " << std::setw(3) << i + 1 << " | "
                    << std::left << std::setw(20) << active[i]->name << "| "
                    << std::setw(8) << active[i]->id << "| "
                    << std::setw(30) << active[i]->status << "|\n";
            }

            std::string replacementID;
            std::cout << "\nEnter replacement Player ID for this withdrawn player (or 0 to skip): ";
            std::getline(std::cin, replacementID);
            if (replacementID == "0") continue;

            Player* replacement = nullptr;
            current = playerHead;
            while (current) {
                if (current->id == replacementID && current->status == "Active") {
                    replacement = current;
                    break;
                }
                current = current->next;
            }

            if (!replacement) {
                std::cout << "Invalid or inactive replacement player.\n";
                continue;
            }

            Match* match = matchHead;
            bool updated = false;
            while (match) {
                if (match->status == "Cancelled") {
                    bool changed = false;
                    if (match->player1 == withdrawnPlayer->name && match->player2 != replacement->name) {
                        match->player1 = replacement->name;
                        changed = true;
                    }
                    else if (match->player2 == withdrawnPlayer->name && match->player1 != replacement->name) {
                        match->player2 = replacement->name;
                        changed = true;
                    }
                    if (changed) {
                        match->status = "Updated";
                        updated = true;
                    }
                }
                match = match->next;
            }

            if (updated) {
                updateScheduleFile();
                std::cout << "Replacement successful for player " << withdrawnPlayer->name << ". Schedule updated.\n";
            }
            else {
                std::cout << "No matches found to update for this withdrawn player.\n";
            }
        }

        delete[] active;

        int option;
        std::cout << "\nAll withdrawn players have been processed.\n";
        std::cout << "1. Return to main menu\n2. Repeat replacement process\nEnter choice: ";
        std::cin >> option;
        std::cin.ignore();

        if (option == 1) return;
    }
}


void TournamentSystem::modifyPlayerStatus() {
    while (true) {
        std::cout << "\n===== Modify Player Status =====\n";

        // Count Active and Withdrawn players
        int activeCount = 0, withdrawnCount = 0;
        Player* current = playerHead;
        while (current) {
            if (current->status == "Active") activeCount++;
            else if (current->status == "Withdrawn") withdrawnCount++;
            current = current->next;
        }

        // Store pointers for sorting
        Player** activePlayers = new Player * [activeCount];
        Player** withdrawnPlayers = new Player * [withdrawnCount];

        current = playerHead;
        int a = 0, w = 0;
        while (current) {
            if (current->status == "Active") activePlayers[a++] = current;
            else if (current->status == "Withdrawn") withdrawnPlayers[w++] = current;
            current = current->next;
        }

        // Sort both arrays by ID using bubble sort
        for (int i = 0; i < activeCount - 1; ++i) {
            for (int j = 0; j < activeCount - i - 1; ++j) {
                if (activePlayers[j]->id > activePlayers[j + 1]->id) {
                    Player* temp = activePlayers[j];
                    activePlayers[j] = activePlayers[j + 1];
                    activePlayers[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < withdrawnCount - 1; ++i) {
            for (int j = 0; j < withdrawnCount - i - 1; ++j) {
                if (withdrawnPlayers[j]->id > withdrawnPlayers[j + 1]->id) {
                    Player* temp = withdrawnPlayers[j];
                    withdrawnPlayers[j] = withdrawnPlayers[j + 1];
                    withdrawnPlayers[j + 1] = temp;
                }
            }
        }

        // Display Active Players
        std::cout << "Active Players:\n";
        std::cout << "----------------------------------------------------------------------------\n";
        std::cout << "| No. | Player Name         | ID       | Status                            |\n";
        std::cout << "----------------------------------------------------------------------------\n";
        for (int i = 0; i < activeCount; ++i) {
            std::cout << "| " << std::setw(3) << i + 1 << " | "
                << std::left << std::setw(20) << activePlayers[i]->name << "| "
                << std::setw(8) << activePlayers[i]->id << "| "
                << std::setw(30) << activePlayers[i]->status << "|\n";
        }

        // Display Withdrawn Players
        std::cout << "\nWithdrawn Players:\n";
        std::cout << "----------------------------------------------------------------------------\n";
        std::cout << "| No. | Player Name         | ID       | Status                            |\n";
        std::cout << "----------------------------------------------------------------------------\n";
        for (int i = 0; i < withdrawnCount; ++i) {
            std::string fullStatus = withdrawnPlayers[i]->status;
            if (!withdrawnPlayers[i]->withdrawReason.empty())
                fullStatus += " (" + withdrawnPlayers[i]->withdrawReason + ")";

            std::cout << "| " << std::setw(3) << i + 1 << " | "
                << std::left << std::setw(20) << withdrawnPlayers[i]->name << "| "
                << std::setw(8) << withdrawnPlayers[i]->id << "| "
                << std::setw(30) << fullStatus << "|\n";
        }
        std::cout << "----------------------------------------------------------------------------\n";

        // Clean up
        delete[] activePlayers;
        delete[] withdrawnPlayers;

        std::string playerID;
        std::cout << "\nEnter Player ID to modify status (or type '0' to return): ";
        getline(std::cin, playerID);
        if (playerID == "0") return;

        Player* target = playerHead;
        while (target) {
            if (target->id == playerID) break;
            target = target->next;
        }

        if (!target) {
            std::cout << "\nPlayer not found.\n1. Try again\n2. Return to menu\nEnter choice: ";
            int retry;
            std::cin >> retry;
            std::cin.ignore();
            if (retry == 2) return;
            continue;
        }

        std::cout << "\nCurrent Status: " << target->status;
        std::cout << "\n1. Set Active\n2. Set Withdrawn\nEnter choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if ((choice == 1 && target->status == "Active") || (choice == 2 && target->status == "Withdrawn")) {
            std::cout << "\nStatus is already set to " << target->status << ". Try again.\n";
            continue;
        }

        if (choice == 1) {
            target->status = "Active";
            target->withdrawReason = "";
            restoreMatchStatus(playerID);
            std::cout << "\nPlayer status changed to Active.\n";
        }
        else if (choice == 2) {
            Match* matchCheck = matchHead;
            bool hasUpcoming = false;
            while (matchCheck) {
                if ((matchCheck->player1 == playerID || matchCheck->player2 == playerID) &&
                    (matchCheck->status == "Upcoming" || matchCheck->status == "Updated")) {
                    hasUpcoming = true;
                    break;
                }
                matchCheck = matchCheck->next;
            }

            if (hasUpcoming) {
                std::cout << "\nWARNING: This player has upcoming matches. Are you sure you want to withdraw them?\n";
                std::cout << "1. Yes, Withdraw\n2. Cancel\nEnter choice: ";
                int confirm;
                std::cin >> confirm;
                std::cin.ignore();
                if (confirm != 1) {
                    std::cout << "Withdrawal cancelled.\n";
                    continue;
                }
            }

            std::cout << "\nSelect reason for withdrawal:\n";
            std::cout << "1. Injury\n2. Personal Reason\n3. Disciplinary Action\n4. Scheduling Conflict\n5. Other\nEnter choice: ";
            int reasonChoice;
            std::cin >> reasonChoice;
            std::cin.ignore();
            switch (reasonChoice) {
            case 1: target->withdrawReason = "Injury"; break;
            case 2: target->withdrawReason = "Personal Reason"; break;
            case 3: target->withdrawReason = "Disciplinary Action"; break;
            case 4: target->withdrawReason = "Scheduling Conflict"; break;
            case 5: target->withdrawReason = "Other"; break;
            default: target->withdrawReason = "Other";
            }

            target->status = "Withdrawn";
            updateMatchSchedule(target->name);
            std::cout << "\nPlayer status changed to Withdrawn (" << target->withdrawReason << ").\n";
        }
        else {
            std::cout << "Invalid choice.\n";
            continue;
        }

        updatePlayerFile();
        validateMatchStatuses(); // ← ensures match statuses reflect player status
        std::cout << "\n1. Modify another player\n2. Return to menu\nEnter choice: ";
        int next;
        std::cin >> next;
        std::cin.ignore();
        if (next == 2) return;
    }
}

// Standalone function to run the Player Withdrawal System
void runPlayerWithdrawalSystem() {
    TournamentSystem ts;
    ts.loadPlayers();
    ts.loadSchedule();
    ts.validateMatchStatuses();

    int choice;
    do {
        std::cout << "\n===== Player Withdrawals Management =====\n";
        std::cout << "1. Show Players (All/Active/Withdrawn)\n";
        std::cout << "2. Modify Player Status\n";
        std::cout << "3. Show Matches (Upcoming/Cancelled/Updated)\n";
        std::cout << "4. Add Replacement Player\n";
        std::cout << "5. Search Player\n";
        std::cout << "6. Return to Main Menu\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore();
        switch (choice) {
        case 1: {
            int filterChoice;
            std::cout << "\nView players by:\n1. All\n2. Active\n3. Withdrawn\nEnter choice: ";
            std::cin >> filterChoice;
            std::cin.ignore();
            ts.showPlayers(filterChoice);
            break;
        }
        case 2:
            ts.modifyPlayerStatus();
            break;
        case 3:
            ts.showMatchesByStatus();
            break;
        case 4:
            ts.addReplacementPlayer();
            break;
        case 5:
            ts.searchPlayer();
            break;
        case 6:
            std::cout << "Returning to main menu...\n";
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);
}
