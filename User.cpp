#include "User.h"
#include <iostream>

User::User(const std::string& id, const std::string& n, const std::string& e, 
           const std::string& p, const std::string& r)
    : userID(id), name(n), email(e), password(p), role(r) {}

// Getters
std::string User::getUserID() const { return userID; }
std::string User::getName() const { return name; }
std::string User::getEmail() const { return email; }
std::string User::getRole() const { return role; }

// Authentication
bool User::login(const std::string& email, const std::string& password) {
    return (this->email == email && this->password == password);
}

void User::logout() {
    // Clear any session data if needed
} 