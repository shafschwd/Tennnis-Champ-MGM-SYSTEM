// TicketManager.h
#ifndef TICKET_MANAGER_H
#define TICKET_MANAGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

const int MAX_SPECTATORS = 50; // Venue capacity

struct Ticket {
    std::string ticketID;
    std::string buyerName;
    std::string type;  // VIP or Regular
    std::string status;
};

class TicketQueue {
private:
    Ticket tickets[100]; // Manual array-based priority queue
    int size;

public:
    TicketQueue();
    void enqueue(Ticket newTicket);
    void processTicketEntry();
    void displayQueue();
    void saveToFile();
    void loadFromFile();
    int countTicketsSold();
};

std::string generateTicketID();
void purchaseTicket(TicketQueue& ticketQueue);
void runTicketManager();

#endif // TICKET_MANAGER_H