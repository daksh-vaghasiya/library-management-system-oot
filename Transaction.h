#pragma once
using namespace std;
#include <bits/stdc++.h>
#include "User.h"
#include "Book.h"

class Transaction {
private:
    string transactionID;
    string userID;
    string bookID;
    string type;  // "borrow" or "return"
    time_t transactionDate;
    time_t dueDate;
    time_t returnDate;
    double fineAmount;

public:
    Transaction(const string& id, const string& uid, const string& bid,
                const string& t, time_t date);
    
    // Getters
    string getTransactionID() const;
    string getUserID() const;
    string getBookID() const;
    string getType() const;
    time_t getTransactionDate() const;
    time_t getDueDate() const;
    time_t getReturnDate() const;
    double getFineAmount() const;
    
    // Setters
    void setReturnDate(time_t date);
    void setFineAmount(double amount);
    
    // Operations
    void calculateFine();
    bool isOverdue() const;
    void closeTransaction();
}; 