#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

class TournamentSystem {
private:
    struct Player {
        string id;
        string name;
        string status;
        string withdrawReason;
        Player* next = nullptr;
    };

    struct Match {
        string matchID, round, player1, player2, date, status;
        Match* next;

        Match(string mid, string r, string p1, string p2, string d, string s) {
            matchID = mid;
            round = r;
            player1 = p1;
            player2 = p2;
            date = d;
            status = s;
            next = nullptr;
        }
    };

    Player* playerHead;
    Match* matchHead;

    void pause() {
        cout << "\nPress Enter to return to menu...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string toLower(const string& str) {
        string result = str;
        for (char& c : result) {
            c = tolower(c);
        }
        return result;
    }

    void updatePlayerFile() {
        ofstream file("player_list.txt");
        if (!file) {
            cerr << "Error: Unable to open player_list.txt for writing.\n";
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
        cout << "Player list successfully updated in player_list.txt.\n";
    }

    void updateMatchSchedule(string playerName) {
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
        cout << cancelledCount << " match(es) involving \"" << playerName << "\" were cancelled.\n";
    }


    void updateScheduleFile() {
        ofstream file("schedule.txt");
        if (!file) {
            cerr << "Error: Unable to open schedule.txt for writing.\n";
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
        cout << "Match schedule successfully updated in schedule.txt.\n";
    }

    void displayPlayersByStatus(const string& statusFilter, const string& title) {
        Player* current = playerHead;
        int count = 0;

        cout << title << endl;
        cout << "----------------------------------------------------------------------------\n";
        cout << "| No. | Player Name         | ID       | Status                            |\n";
        cout << "----------------------------------------------------------------------------\n";

        while (current) {
            if (current->status == statusFilter) {
                string fullStatus = current->status;
                if (current->status == "Withdrawn" && !current->withdrawReason.empty())
                    fullStatus += " (" + current->withdrawReason + ")";

                cout << "| " << left << setw(3) << ++count << " | "
                    << left << setw(20) << current->name << "| "
                    << setw(8) << current->id << "| "
                    << setw(30) << fullStatus << "\n";
            }
            current = current->next;
        }

        cout << "----------------------------------------------------------------------------\n";
    }

public:
    TournamentSystem() {
        playerHead = nullptr;
        matchHead = nullptr;
    }

    ~TournamentSystem() {
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

    void loadSchedule() {
        ifstream file("schedule.txt");
        if (!file) {
            cerr << "Error: Unable to open schedule.txt\n";
            return;
        }
        string line;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string matchID, round, player1, player2, date, status;
            getline(ss, matchID, ',');
            getline(ss >> ws, round, ',');
            getline(ss >> ws, player1, ',');
            getline(ss >> ws, player2, ',');
            getline(ss >> ws, date, ',');
            getline(ss >> ws, status);
            Match* newMatch = new Match(matchID, round, player1, player2, date, status);
            newMatch->next = matchHead;
            matchHead = newMatch;
        }
        file.close();
    }

    string extractWithdrawReason(const string& statusStr) {
        size_t start = statusStr.find("(");
        size_t end = statusStr.find(")");
        if (start != string::npos && end != string::npos && end > start) {
            return statusStr.substr(start + 1, end - start - 1);
        }
        return "";
    }

    void loadPlayers() {
        ifstream file("player_list.txt");
        if (!file) {
            cerr << "Error: Unable to open player_list.txt\n";
            return;
        }
        string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            stringstream ss(line);
            Player* newPlayer = new Player;
            getline(ss, newPlayer->id, ',');
            getline(ss >> ws, newPlayer->name, ',');
            getline(ss >> ws, newPlayer->status);

            // Remove potential trailing newline or whitespace
            newPlayer->status.erase(remove(newPlayer->status.begin(), newPlayer->status.end(), '\r'), newPlayer->status.end());
            newPlayer->status.erase(remove(newPlayer->status.begin(), newPlayer->status.end(), '\n'), newPlayer->status.end());

            // Extract withdraw reason if present
            if (newPlayer->status.find("Withdrawn") != string::npos) {
                newPlayer->withdrawReason = extractWithdrawReason(newPlayer->status);
                newPlayer->status = "Withdrawn";
            }

            newPlayer->next = playerHead;
            playerHead = newPlayer;
        }
        file.close();
    }

    


    // Levenshtein Distance function
    int levenshteinDistance(const string& a, const string& b) {
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
                    dp[i][j] = 1 + min(min(dp[i - 1][j], dp[i][j - 1]), dp[i - 1][j - 1]);
                }
            }
        }
        return dp[lenA][lenB];
    }

    void searchPlayer();
    void searchPlayerByID(const string& query, TournamentSystem::Player* playerHead) {
        cout << "\n===== Search Results by ID =====\n";
        cout << "--------------------------------------------------\n";
        cout << "| No. | Player Name       | ID    | Status         |\n";
        cout << "--------------------------------------------------\n";
        int count = 0;
        TournamentSystem::Player* current = playerHead;
        while (current) {
            if (current->id == query) {
                cout << "| " << setw(3) << ++count << " | "
                    << left << setw(17) << current->name << "| "
                    << setw(5) << current->id << " | "
                    << setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
            }
            current = current->next;
        }
        if (count == 0) {
            cout << "No player found with ID: " << query << "\n";
        }
        else {
            cout << "--------------------------------------------------\n";
        }
    }

    void validateMatchStatuses();

    void searchPlayerByName(const string& query, TournamentSystem::Player* playerHead);

    string getPlayerNameByID(const string& id) {
        Player* current = playerHead;
        while (current) {
            if (current->id == id) return current->name;
            current = current->next;
        }
        return id;
    }

    string getPlayerStatusTag(const string& playerID) {
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

    void restoreMatchStatus(const string& playerID) {
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

    void showMatchesByStatus() {
        while (true) {
            cout << "\n===== View Matches by Status =====\n";
            cout << "Select match status to view:\n";
            cout << "1. All\n2. Upcoming\n3. Cancelled\n4. Updated\nEnter choice: ";

            int filterChoice;
            cin >> filterChoice;
            cin.ignore();

            string header;
            if (filterChoice == 1) header = "All";
            else if (filterChoice == 2) header = "Upcoming";
            else if (filterChoice == 3) header = "Cancelled";
            else if (filterChoice == 4) header = "Updated";
            else {
                cout << "Invalid choice. Returning to main menu.\n";
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
                cout << "\nNo matches found for the selected status.\n";

                int option;
                do {
                    cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
                    cin >> option;
                    cin.ignore();
                    if (option == 1)
                        return showMatchesByStatus(); // re-enter the filter menu
                    else if (option == 2)
                        return;
                    else
                        cout << "Invalid choice. Try again.\n";
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
            cout << "\n===== Matches with Status: " << header << " =====\n";
            cout << "----------------------------------------------------------------------------------------------------\n";
            cout << "| No. | Match ID | Round         | Player 1                 | Player 2                 | Date       | Status     |\n";
            cout << "----------------------------------------------------------------------------------------------------\n";

            for (int j = 0; j < count; ++j) {
                string p1Name = getPlayerNameByID(matches[j]->player1);
                string p2Name = getPlayerNameByID(matches[j]->player2);
                string p1Full = p1Name + getPlayerStatusTag(matches[j]->player1);
                string p2Full = p2Name + getPlayerStatusTag(matches[j]->player2);

                cout << left
                    << "| " << setw(3) << j + 1 << " | "
                    << setw(8) << matches[j]->matchID << " | "
                    << setw(13) << matches[j]->round << " | "
                    << setw(25) << p1Full << " | "
                    << setw(25) << p2Full << " | "
                    << setw(10) << matches[j]->date << " | "
                    << setw(10) << matches[j]->status << " |\n";
            }

            cout << "----------------------------------------------------------------------------------------------------\n";
            delete[] matches;

            int option;
            do {
                cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
                cin >> option;
                cin.ignore();
                if (option == 2) return;
                else if (option != 1) cout << "Invalid choice. Try again.\n";
            } while (option != 1);
        }
    }


    void showPlayers(int filter) {
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
            cout << "No players found for the selected category.\n";
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
        cout << "\n===== Player List =====\n";
        cout << "----------------------------------------------------------------------------\n";
        cout << "| No. | Player Name         | ID       | Status                            |\n";
        cout << "----------------------------------------------------------------------------\n";

        for (int j = 0; j < count; ++j) {
            string fullStatus = players[j]->status;
            if (players[j]->status == "Withdrawn" && !players[j]->withdrawReason.empty())
                fullStatus += " (" + players[j]->withdrawReason + ")";

            cout << "| " << left << setw(3) << j + 1 << " | "
                << left << setw(20) << players[j]->name << "| "
                << setw(8) << players[j]->id << "| "
                << setw(30) << fullStatus << "\n";
        }

        cout << "----------------------------------------------------------------------------\n";
        delete[] players;

        int option;
        do {
            cout << "\n1. Return to filter selection\n2. Return to main menu\nEnter choice: ";
            cin >> option;
            cin.ignore();
            if (option == 1) {
                cout << "\nView players by:\n";
                cout << "1. All\n2. Active\n3. Withdrawn\nEnter choice: ";
                int filterChoice;
                cin >> filterChoice;
                cin.ignore();
                if (filterChoice >= 1 && filterChoice <= 3)
                    showPlayers(filterChoice);
                else
                    cout << "Invalid choice.\n";
                return;
            }
            else if (option == 2) {
                return;
            }
            else {
                cout << "Invalid choice. Try again.\n";
            }
        } while (option != 2);
    }


    void addReplacementPlayer() {
        while (true) {
            cout << "\n===== Add Replacement Player =====\n";

            int withdrawnCount = 0, activeCount = 0;
            Player* current = playerHead;
            while (current) {
                if (current->status == "Withdrawn") withdrawnCount++;
                else if (current->status == "Active") activeCount++;
                current = current->next;
            }

            Player** withdrawn = new Player * [withdrawnCount];
            Player** active = new Player * [activeCount];
            current = playerHead;
            int wi = 0, ai = 0;
            while (current) {
                if (current->status == "Withdrawn") withdrawn[wi++] = current;
                else if (current->status == "Active") active[ai++] = current;
                current = current->next;
            }

            for (int i = 0; i < withdrawnCount - 1; ++i)
                for (int j = 0; j < withdrawnCount - i - 1; ++j)
                    if (withdrawn[j]->id > withdrawn[j + 1]->id)
                        swap(withdrawn[j], withdrawn[j + 1]);

            for (int i = 0; i < activeCount - 1; ++i)
                for (int j = 0; j < activeCount - i - 1; ++j)
                    if (active[j]->id > active[j + 1]->id)
                        swap(active[j], active[j + 1]);

            cout << "Withdrawn Players:\n";
            cout << "----------------------------------------------------------------------------\n";
            cout << "| No. | Player Name         | ID       | Status                            |\n";
            cout << "----------------------------------------------------------------------------\n";
            for (int i = 0; i < withdrawnCount; ++i) {
                string fullStatus = withdrawn[i]->status;
                if (!withdrawn[i]->withdrawReason.empty())
                    fullStatus += " (" + withdrawn[i]->withdrawReason + ")";
                cout << "| " << setw(3) << i + 1 << " | "
                    << left << setw(20) << withdrawn[i]->name << "| "
                    << setw(8) << withdrawn[i]->id << "| "
                    << setw(30) << fullStatus << "|\n";
            }

            cout << "\nActive Players:\n";
            cout << "----------------------------------------------------------------------------\n";
            cout << "| No. | Player Name         | ID       | Status                            |\n";
            cout << "----------------------------------------------------------------------------\n";
            for (int i = 0; i < activeCount; ++i) {
                cout << "| " << setw(3) << i + 1 << " | "
                    << left << setw(20) << active[i]->name << "| "
                    << setw(8) << active[i]->id << "| "
                    << setw(30) << active[i]->status << "|\n";
            }

            string withdrawnID;
            Player* withdrawnPlayer = nullptr;
            while (true) {
                cout << "\nEnter withdrawn Player ID (or 0 to return): ";
                getline(cin, withdrawnID);
                if (withdrawnID == "0") return;

                current = playerHead;
                while (current) {
                    if (current->id == withdrawnID && current->status == "Withdrawn") {
                        withdrawnPlayer = current;
                        break;
                    }
                    current = current->next;
                }

                if (withdrawnPlayer) break;
                cout << "Invalid withdrawn player ID. Please try again.\n";
            }

            while (true) {
                string replacementID;
                cout << "Enter replacement Player ID (or 0 to return): ";
                getline(cin, replacementID);
                if (replacementID == "0") break;

                Player* replacement = nullptr;
                current = playerHead;
                while (current) {
                    if (current->id == replacementID) {
                        replacement = current;
                        break;
                    }
                    current = current->next;
                }

                if (!replacement) {
                    cout << "Invalid replacement player ID.\n";
                    continue;
                }
                else if (replacement->status != "Active") {
                    cout << "Replacement player must be active.\n";
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
                    cout << "Replacement successfully made and schedule updated.\n";
                }
                else {
                    cout << "No matches found to update for the withdrawn player.\n";
                }

                int option;
                do {
                    cout << "\n1. Try another replacement\n2. Return to main menu\nEnter choice: ";
                    cin >> option;
                    cin.ignore();
                    if (option == 2) return;
                    else if (option != 1) cout << "Invalid choice. Try again.\n";
                } while (option != 1);
            }

            delete[] withdrawn;
            delete[] active;
        }
    }

    void modifyPlayerStatus() {
        while (true) {
            cout << "\n===== Modify Player Status =====\n";

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
            cout << "Active Players:\n";
            cout << "----------------------------------------------------------------------------\n";
            cout << "| No. | Player Name         | ID       | Status                            |\n";
            cout << "----------------------------------------------------------------------------\n";
            for (int i = 0; i < activeCount; ++i) {
                cout << "| " << setw(3) << i + 1 << " | "
                    << left << setw(20) << activePlayers[i]->name << "| "
                    << setw(8) << activePlayers[i]->id << "| "
                    << setw(30) << activePlayers[i]->status << "|\n";
            }

            // Display Withdrawn Players
            cout << "\nWithdrawn Players:\n";
            cout << "----------------------------------------------------------------------------\n";
            cout << "| No. | Player Name         | ID       | Status                            |\n";
            cout << "----------------------------------------------------------------------------\n";
            for (int i = 0; i < withdrawnCount; ++i) {
                string fullStatus = withdrawnPlayers[i]->status;
                if (!withdrawnPlayers[i]->withdrawReason.empty())
                    fullStatus += " (" + withdrawnPlayers[i]->withdrawReason + ")";

                cout << "| " << setw(3) << i + 1 << " | "
                    << left << setw(20) << withdrawnPlayers[i]->name << "| "
                    << setw(8) << withdrawnPlayers[i]->id << "| "
                    << setw(30) << fullStatus << "|\n";
            }
            cout << "----------------------------------------------------------------------------\n";

            // Clean up
            delete[] activePlayers;
            delete[] withdrawnPlayers;

            string playerID;
            cout << "\nEnter Player ID to modify status (or type '0' to return): ";
            getline(cin, playerID);
            if (playerID == "0") return;

            Player* target = playerHead;
            while (target) {
                if (target->id == playerID) break;
                target = target->next;
            }

            if (!target) {
                cout << "\nPlayer not found.\n1. Try again\n2. Return to menu\nEnter choice: ";
                int retry;
                cin >> retry;
                cin.ignore();
                if (retry == 2) return;
                continue;
            }

            cout << "\nCurrent Status: " << target->status;
            cout << "\n1. Set Active\n2. Set Withdrawn\nEnter choice: ";
            int choice;
            cin >> choice;
            cin.ignore();

            if ((choice == 1 && target->status == "Active") || (choice == 2 && target->status == "Withdrawn")) {
                cout << "\nStatus is already set to " << target->status << ". Try again.\n";
                continue;
            }

            if (choice == 1) {
                target->status = "Active";
                target->withdrawReason = "";
                restoreMatchStatus(playerID);
                cout << "\nPlayer status changed to Active.\n";
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
                    cout << "\nWARNING: This player has upcoming matches. Are you sure you want to withdraw them?\n";
                    cout << "1. Yes, Withdraw\n2. Cancel\nEnter choice: ";
                    int confirm;
                    cin >> confirm;
                    cin.ignore();
                    if (confirm != 1) {
                        cout << "Withdrawal cancelled.\n";
                        continue;
                    }
                }

                cout << "\nSelect reason for withdrawal:\n";
                cout << "1. Injury\n2. Personal Reason\n3. Disciplinary Action\n4. Scheduling Conflict\n5. Other\nEnter choice: ";
                int reasonChoice;
                cin >> reasonChoice;
                cin.ignore();
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
                cout << "\nPlayer status changed to Withdrawn (" << target->withdrawReason << ").\n";
            }
            else {
                cout << "Invalid choice.\n";
                continue;
            }

            updatePlayerFile();
            validateMatchStatuses(); // ← ensures match statuses reflect player status
            cout << "\n1. Modify another player\n2. Return to menu\nEnter choice: ";
            int next;
            cin >> next;
            cin.ignore();
            if (next == 2) return;
        }
    }

};

void TournamentSystem::searchPlayer() {
    int choice;
    while (true) {
        cout << "\n===== Search Player =====\n";
        cout << "1. Search by Player ID\n";
        cout << "2. Search by Player Name\n";
        cout << "3. Return to main menu\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore();

        if (choice == 3) return;

        string query;
        if (choice == 1) {
            cout << "Enter Player ID: ";
            getline(cin, query);
            searchPlayerByID(query, playerHead);
        }
        else if (choice == 2) {
            cout << "Enter Player Name: ";
            getline(cin, query);
            searchPlayerByName(query, playerHead);
        }
        else {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        // Ask user to search again or return
        while (true) {
            int next;
            cout << "\n1. Search again\n2. Return to main menu\nEnter choice: ";
            if (!(cin >> next)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            cin.ignore();

            if (next == 1) break; // go back to top of outer while loop
            else if (next == 2) return;
            else cout << "Invalid choice. Try again.\n";
        }
    }
}

void TournamentSystem::validateMatchStatuses() {
    Match* currentMatch = matchHead;
    while (currentMatch) {
        // Check both players' current status by name
        Player* p1 = playerHead;
        Player* p2 = playerHead;
        string status1 = "Active", status2 = "Active";

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

void TournamentSystem::searchPlayerByName(const string& query, TournamentSystem::Player* playerHead) {
    cout << "\n===== Search Results by Name =====\n";
    cout << "--------------------------------------------------\n";
    cout << "| No. | Player Name       | ID    | Status         |\n";
    cout << "--------------------------------------------------\n";

    int count = 0;
    TournamentSystem::Player* current = playerHead;

    // Exact or substring match
    while (current) {
        if (toLower(current->name).find(toLower(query)) != string::npos) {
            cout << "| " << setw(3) << ++count << " | "
                << left << setw(17) << current->name << "| "
                << setw(5) << current->id << " | "
                << setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
        }
        current = current->next;
    }

    if (count == 0) {
        cout << "No exact match found for: " << query << "\n";
        cout << "\nShowing similar results:\n";
        cout << "--------------------------------------------------\n";
        cout << "| No. | Player Name       | ID    | Status         |\n";
        cout << "--------------------------------------------------\n";

        current = playerHead;
        while (current) {
            string firstName = current->name.substr(0, current->name.find(" "));
            int dist = levenshteinDistance(toLower(query), toLower(firstName));

            if (dist <= 2) {
                cout << "| " << setw(3) << ++count << " | "
                    << left << setw(17) << current->name << "| "
                    << setw(5) << current->id << " | "
                    << setw(15) << current->status + (current->status == "Withdrawn" ? " (" + current->withdrawReason + ")" : "") << " |\n";
            }
            current = current->next;
        }

        if (count == 0) cout << "No similar names found.\n";
        else cout << "--------------------------------------------------\n";
    }
    else {
        cout << "--------------------------------------------------\n";
    }
}

int main() {
    TournamentSystem ts;
    ts.loadPlayers();
    ts.loadSchedule();
    ts.validateMatchStatuses(); // ← add this line here

    int choice;
    do {
        cout << "\n===== Tournament System =====\n";
        cout << "1. Show Players (All/Active/Withdrawn)\n";
        cout << "2. Modify Player Status\n";
        cout << "3. Show Matches (Upcoming/Cancelled/Updated)\n";
        cout << "4. Add Replacement Player\n";
        cout << "5. Search Player\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
        case 1: {
            int filterChoice;
            cout << "\nView players by:\n1. All\n2. Active\n3. Withdrawn\nEnter choice: ";
            cin >> filterChoice;
            cin.ignore();
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
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}
