#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User {
protected:
    std::string userID;
    std::string name;
    std::string email;
    std::string password;
    std::string role;  // "admin", "librarian", "user"

public:
    User(const std::string& id, const std::string& n, const std::string& e, 
         const std::string& p, const std::string& r);
    virtual ~User() = default;
    
    // Getters
    std::string getUserID() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getRole() const;
    
    // Authentication
    bool login(const std::string& email, const std::string& password);
    void logout();
    
    // Virtual functions for role-specific operations
    virtual void displayMenu() = 0;
    virtual void performAction(int choice) = 0;

    friend class LibrarySystem;  // Allow LibrarySystem to access protected members
};

#endif // USER_H 