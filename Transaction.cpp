using namespace std;
#include <bits/stdc++.h>
#include "Transaction.h"
#include <ctime>
#include <iostream>

Transaction::Transaction(const string& id, const string& uid, const string& bid,
                         const string& t, time_t date)
    : transactionID(id), userID(uid), bookID(bid), type(t), transactionDate(date),
      dueDate(date + (14 * 24 * 60 * 60)), returnDate(0), fineAmount(0.0) {
}

// Getters
string Transaction::getTransactionID() const { return transactionID; }
string Transaction::getUserID() const { return userID; }
string Transaction::getBookID() const { return bookID; }
string Transaction::getType() const { return type; }
time_t Transaction::getTransactionDate() const { return transactionDate; }
time_t Transaction::getDueDate() const { return dueDate; }
time_t Transaction::getReturnDate() const { return returnDate; }
double Transaction::getFineAmount() const { return fineAmount; }

// Setters
void Transaction::setReturnDate(time_t date) { returnDate = date; }
void Transaction::setFineAmount(double amount) { fineAmount = amount; }

// Operations
void Transaction::calculateFine() {
    if (type == "return" && returnDate > dueDate) {
        // Calculate days overdue (round up to nearest day)
        double daysOverdue = ceil(difftime(returnDate, dueDate) / (24.0 * 60 * 60));
        // Fine is $1 per day
        fineAmount = daysOverdue;
    }
}

bool Transaction::isOverdue() const {
    if (type == "borrow") {
        time_t now = time(nullptr);
        return now > dueDate;
    }
    return false;
}

void Transaction::closeTransaction() {
    if (type == "return") {
        calculateFine();
    }
} 