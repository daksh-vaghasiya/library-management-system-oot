#pragma once
using namespace std;
#include <bits/stdc++.h>
#include "User.h"
#include "LibrarySystem.h"
#include <vector>

class RegisteredUser : public User {
private:
    std::vector<std::string> borrowedBooks;
    double outstandingFine;

public:
    RegisteredUser(const std::string& id, const std::string& n, const std::string& e, 
                   const std::string& p);
    
    // Override virtual functions
    void displayMenu() override;
    void performAction(int choice) override;
    
    // User-specific operations
    void searchBooks();
    void viewBookDetails();
    void borrowBook();
    void returnBook();
    void viewCurrentBorrowings();
    void viewProfile();
    void updateProfile();
    void changePassword();
    void viewFines();
    void payFines();
    void viewBorrowingHistory();
    void viewFineHistory();
    
    // Getters
    double getOutstandingFine() const;
    const std::vector<std::string>& getBorrowedBooks() const;
    
    // Setters
    void addBorrowedBook(const std::string& bookID);
    void removeBorrowedBook(const std::string& bookID);
    void setOutstandingFine(double amount);
}; 