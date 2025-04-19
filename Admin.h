#pragma once
using namespace std;
#include <bits/stdc++.h>
#include "User.h"
#include "LibrarySystem.h"

class Admin : public User {
public:
    Admin(const string& id, const string& n, const string& e, 
          const string& p);
    
    // Override virtual functions
    void displayMenu() override;
    void performAction(int choice) override;
    
    // Admin-specific operations
    void addUser();
    void removeUser();
    void modifyUserRole();
    void viewAllUsers();
    void addBook();
    void updateBook();
    void removeBook();
    void viewAllBooks();

    // Test helper methods
    bool addUserDirect(const string& id, const string& name, const string& email,
                      const string& password, const string& role);
    bool addBookDirect(const string& id, const string& title, const string& author,
                      const string& category, const string& isbn);
}; 