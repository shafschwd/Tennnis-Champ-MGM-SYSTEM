#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

const int MAX_SPECTATORS = 50; // Venue capacity

struct Ticket {
    string ticketID;
    string buyerName;
    string type;  // VIP or Regular
    string status;
};

class TicketQueue {
private:
    Ticket tickets[100]; // Manual array-based priority queue
    int size;

public:
    TicketQueue() { size = 0; }

    void enqueue(Ticket newTicket) {
        if (size >= 100) {
            cout << "Queue is full!\n";
            return;
        }

        if (newTicket.status == "Cancelled" || newTicket.status == "Rejected" || newTicket.status == "Waiting") {
            return; // Ignore these tickets
        }

        int pos = size;
        while (pos > 0 && tickets[pos - 1].type == "Regular" && newTicket.type == "VIP") {
            tickets[pos] = tickets[pos - 1]; // Shift Regular ticket down to insert VIP
            pos--;
        }

        tickets[pos] = newTicket;
        size++;
    }

    void processTicketEntry() {
        if (size == 0) {
            cout << "No valid tickets to process!\n";
            return;
        }

        for (int i = 0; i < size; i++) {
            if (tickets[i].status == "Pending") {
                tickets[i].status = "Confirmed";
                cout << "[INFO] Ticket Processed: " << tickets[i].ticketID << " - " << tickets[i].buyerName << " (Confirmed)\n";
                saveToFile();
                return;
            }
        }

        cout << "No pending tickets available for processing.\n";
    }

    void displayQueue() {
        if (size == 0) {
            cout << "No tickets in the queue.\n";
            return;
        }

        cout << "\n=== Current Ticket Queue ===\n";
        cout << "------------------------------------------------------\n";
        cout << setw(10) << "TicketID" << setw(15) << "Buyer Name" << setw(10) << "Type" << setw(15) << "Status" << "\n";
        cout << "------------------------------------------------------\n";

        bool vipSection = true;
        for (int i = 0; i < size; i++) {
            if (vipSection && tickets[i].type == "Regular") {
                cout << "-------------------- VIP SECTION END ------------------\n";
                vipSection = false;
            }

            cout << setw(10) << tickets[i].ticketID << setw(15) << tickets[i].buyerName
                << setw(10) << tickets[i].type << setw(15) << tickets[i].status << "\n";
        }
        cout << "------------------------------------------------------\n";
    }

    void saveToFile() {
        ofstream file("ticket_sales.txt");
        if (!file) {
            cout << "Error opening file!\n";
            return;
        }

        file << "TicketID,BuyerName,Type,Status\n";
        for (int i = 0; i < size; i++) {
            file << tickets[i].ticketID << "," << tickets[i].buyerName << ","
                << tickets[i].type << "," << tickets[i].status << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file("ticket_sales.txt");
        if (!file) {
            cout << "No existing ticket records found.\n";
            return;
        }

        string line;
        getline(file, line);
        while (getline(file, line)) {
            Ticket t;
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
                continue; // Skip malformed lines
            }

            t.ticketID = line.substr(0, pos1);
            t.buyerName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            t.type = line.substr(pos2 + 1, pos3 - pos2 - 1);
            t.status = line.substr(pos3 + 1);

            enqueue(t);
        }
        file.close();
    }

    int countTicketsSold() {
        int count = 0;
        for (int i = 0; i < size; i++) {
            if (tickets[i].status == "Pending" || tickets[i].status == "Confirmed") {
                count++;
            }
        }
        return count;
    }
};

int lastTicketNumber = 0;

string generateTicketID() {
    if (lastTicketNumber == 0) {
        ifstream file("ticket_sales.txt");
        string lastID = "T000";
        string line, tempID;

        if (file) {
            getline(file, line);
            while (getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != string::npos) {
                    tempID = line.substr(0, pos);
                }
            }
            if (!tempID.empty()) {
                lastID = tempID;
            }
            file.close();
        }

        if (lastID.size() > 1 && lastID[0] == 'T') {
            try {
                lastTicketNumber = stoi(lastID.substr(1));
            }
            catch (...) {
                lastTicketNumber = 0;
            }
        }
    }

    lastTicketNumber++;
    return "T" + string(3 - to_string(lastTicketNumber).length(), '0') + to_string(lastTicketNumber);
}

void purchaseTicket(TicketQueue& ticketQueue) {
    int totalTicketsSold = ticketQueue.countTicketsSold();

    if (totalTicketsSold >= MAX_SPECTATORS) {
        cout << "Sorry, the venue is at full capacity! No more tickets can be sold.\n";
        return;
    }

    Ticket newTicket;
    newTicket.ticketID = generateTicketID();

    cout << "Enter Buyer Name: ";
    cin >> ws;
    getline(cin, newTicket.buyerName);

    while (newTicket.buyerName.empty()) {
        cout << "Error: Buyer name cannot be empty. Please enter a valid name: ";
        getline(cin, newTicket.buyerName);
    }

    int choice;
    do {
        cout << "Choose Ticket Type:\n1. VIP\n2. Regular\nEnter choice (1 or 2): ";
        cin >> choice;

        if (cin.fail() || (choice != 1 && choice != 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice! Please enter 1 or 2.\n";
        }
    } while (choice != 1 && choice != 2);

    newTicket.type = (choice == 1) ? "VIP" : "Regular";
    newTicket.status = "Pending";

    ticketQueue.enqueue(newTicket);
    cout << "[INFO] Ticket " << newTicket.ticketID << " added for " << newTicket.buyerName << " (" << newTicket.type << ")\n";

    ticketQueue.saveToFile();
}

int main() {
    TicketQueue ticketQueue;
    ticketQueue.loadFromFile();
    int choice;

    do {
        int ticketsSold = ticketQueue.countTicketsSold();

        cout << "\n================== Ticket Sales & Spectator Management ==================\n";
        cout << "Current Tickets Sold : " << setw(2) << ticketsSold << " / " << MAX_SPECTATORS << "\n";
        cout << "-------------------------------------------------------------------------\n";
        cout << "  1. Purchase Ticket\n";
        cout << "  2. Process Ticket Entry\n";
        cout << "  3. Show Ticket Queue\n";
        cout << "  4. Exit\n";
        cout << "-------------------------------------------------------------------------\n";
        cout << "Enter your choice: ";

        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number between 1-4.\n";
            continue;
        }

        if (choice == 1) purchaseTicket(ticketQueue);
        else if (choice == 2) ticketQueue.processTicketEntry();
        else if (choice == 3) ticketQueue.displayQueue();

    } while (choice != 4);

    cout << "[EXIT] Program closed.\n";
    return 0;
}
