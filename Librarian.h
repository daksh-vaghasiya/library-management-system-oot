#pragma once
#include "User.h"
#include "LibrarySystem.h"

class Librarian : public User {
public:
    Librarian(const std::string& id, const std::string& n, const std::string& e, 
              const std::string& p);
    
    // Override virtual functions
    void displayMenu() override;
    void performAction(int choice) override;
    
    // Librarian-specific operations
    void addBook();
    void updateBook();
    void removeBook();
    void viewAllBooks();
    void issueBook();
    void acceptReturn();
    void viewCurrentIssues();
    void viewUserFines();
    void acceptFinePayment();
    void searchBooks();
    void searchUsers();

    // Test helper methods
    bool addBookDirect(const std::string& id, const std::string& title, const std::string& author,
                      const std::string& category, const std::string& isbn);
}; 