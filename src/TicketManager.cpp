// TicketManager.cpp
#include "../include/TicketManager.h"

int lastTicketNumber = 0;

TicketQueue::TicketQueue() {
    size = 0;
}

void TicketQueue::enqueue(Ticket newTicket) {
    if (size >= 100) {
        std::cout << "Queue is full!\n";
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

void TicketQueue::processTicketEntry() {
    if (size == 0) {
        std::cout << "No valid tickets to process!\n";
        return;
    }

    for (int i = 0; i < size; i++) {
        if (tickets[i].status == "Pending") {
            tickets[i].status = "Confirmed";
            std::cout << "[INFO] Ticket Processed: " << tickets[i].ticketID << " - " << tickets[i].buyerName << " (Confirmed)\n";
            saveToFile();
            return;
        }
    }

    std::cout << "No pending tickets available for processing.\n";
}

void TicketQueue::displayQueue() {
    if (size == 0) {
        std::cout << "No tickets in the queue.\n";
        return;
    }

    std::cout << "\n=== Current Ticket Queue ===\n";
    std::cout << "------------------------------------------------------\n";
    std::cout << std::setw(10) << "TicketID" << std::setw(15) << "Buyer Name" << std::setw(10) << "Type" << std::setw(15) << "Status" << "\n";
    std::cout << "------------------------------------------------------\n";

    bool vipSection = true;
    for (int i = 0; i < size; i++) {
        if (vipSection && tickets[i].type == "Regular") {
            std::cout << "-------------------- VIP SECTION END ------------------\n";
            vipSection = false;
        }

        std::cout << std::setw(10) << tickets[i].ticketID << std::setw(15) << tickets[i].buyerName
                << std::setw(10) << tickets[i].type << std::setw(15) << tickets[i].status << "\n";
    }
    std::cout << "------------------------------------------------------\n";
}

void TicketQueue::saveToFile() {
    std::ofstream file("data/ticket_sales.txt");
    if (!file) {
        std::cout << "Error opening file!\n";
        return;
    }

    file << "TicketID,BuyerName,Type,Status\n";
    for (int i = 0; i < size; i++) {
        file << tickets[i].ticketID << "," << tickets[i].buyerName << ","
             << tickets[i].type << "," << tickets[i].status << "\n";
    }
    file.close();
}

void TicketQueue::loadFromFile() {
    std::ifstream file("data/ticket_sales.txt");
    if (!file) {
        std::cout << "No existing ticket records found.\n";
        return;
    }

    std::string line;
    getline(file, line);
    while (getline(file, line)) {
        Ticket t;
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);

        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
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

int TicketQueue::countTicketsSold() {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (tickets[i].status == "Pending" || tickets[i].status == "Confirmed") {
            count++;
        }
    }
    return count;
}

std::string generateTicketID() {
    if (lastTicketNumber == 0) {
        std::ifstream file("data/ticket_sales.txt");
        std::string lastID = "T000";
        std::string line, tempID;

        if (file) {
            getline(file, line);
            while (getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
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
                lastTicketNumber = std::stoi(lastID.substr(1));
            }
            catch (...) {
                lastTicketNumber = 0;
            }
        }
    }

    lastTicketNumber++;
    return "T" + std::string(3 - std::to_string(lastTicketNumber).length(), '0') + std::to_string(lastTicketNumber);
}

void purchaseTicket(TicketQueue& ticketQueue) {
    int totalTicketsSold = ticketQueue.countTicketsSold();

    if (totalTicketsSold >= MAX_SPECTATORS) {
        std::cout << "Sorry, the venue is at full capacity! No more tickets can be sold.\n";
        return;
    }

    Ticket newTicket;
    newTicket.ticketID = generateTicketID();

    std::cout << "Enter Buyer Name: ";
    std::cin >> std::ws;
    getline(std::cin, newTicket.buyerName);

    while (newTicket.buyerName.empty()) {
        std::cout << "Error: Buyer name cannot be empty. Please enter a valid name: ";
        getline(std::cin, newTicket.buyerName);
    }

    int choice;
    do {
        std::cout << "Choose Ticket Type:\n1. VIP\n2. Regular\nEnter choice (1 or 2): ";
        std::cin >> choice;

        if (std::cin.fail() || (choice != 1 && choice != 2)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice! Please enter 1 or 2.\n";
        }
    } while (choice != 1 && choice != 2);

    newTicket.type = (choice == 1) ? "VIP" : "Regular";
    newTicket.status = "Pending";

    ticketQueue.enqueue(newTicket);
    std::cout << "[INFO] Ticket " << newTicket.ticketID << " added for " << newTicket.buyerName << " (" << newTicket.type << ")\n";

    ticketQueue.saveToFile();
}

void runTicketManager() {
    TicketQueue ticketQueue;
    ticketQueue.loadFromFile();
    int choice;

    do {
        int ticketsSold = ticketQueue.countTicketsSold();

        std::cout << "\n================== Ticket Sales & Spectator Management ==================\n";
        std::cout << "Current Tickets Sold : " << std::setw(2) << ticketsSold << " / " << MAX_SPECTATORS << "\n";
        std::cout << "-------------------------------------------------------------------------\n";
        std::cout << "  1. Purchase Ticket\n";
        std::cout << "  2. Process Ticket Entry\n";
        std::cout << "  3. Show Ticket Queue\n";
        std::cout << "  4. Return to Main Menu\n";
        std::cout << "-------------------------------------------------------------------------\n";
        std::cout << "Enter your choice: ";

        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number between 1-4.\n";
            continue;
        }

        switch(choice) {
            case 1:
                purchaseTicket(ticketQueue);
                break;
            case 2:
                ticketQueue.processTicketEntry();
                break;
            case 3:
                ticketQueue.displayQueue();
                break;
            case 4:
                std::cout << "Returning to main menu...\n";
                break;
            default:
                std::cout << "Invalid choice! Please enter a number between 1-4.\n";
        }

    } while (choice != 4);
}
