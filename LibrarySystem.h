#ifndef LIBRARY_SYSTEM_H
#define LIBRARY_SYSTEM_H

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include "User.h"
#include "Book.h"
#include "Transaction.h"

class Admin;
class Librarian;
class RegisteredUser;

class LibrarySystem {
private:
    static LibrarySystem* instance;
    std::vector<User*> users;
    std::vector<Book*> books;
    std::vector<Transaction*> transactions;
    User* currentUser;

    LibrarySystem(); // Private constructor for singleton
    ~LibrarySystem(); // Destructor

public:
    static LibrarySystem* getInstance();
    static void destroyInstance();
    
    // User Management
    bool login(const std::string& email, const std::string& password);
    void logout();
    User* getCurrentUser() const;
    void addUser(User* user);
    void removeUser(const std::string& email);
    void updateUserDetails(const std::string& email, const std::string& newName, const std::string& newEmail);
    User* findUser(const std::string& email) const;
    std::vector<User*> getAllUsers() const;
    
    // Book Management
    void addBook(Book* book);
    void removeBook(const std::string& bookID);
    Book* findBook(const std::string& bookID);
    std::vector<Book*> searchBooks(const std::string& query);
    std::vector<Book*> getAllBooks() const;
    
    // Transaction Management
    void createTransaction(Transaction* transaction);
    std::vector<Transaction*> getUserTransactions(const std::string& userID);
    std::vector<Transaction*> getBookTransactions(const std::string& bookID);
    
    // File Operations
    void loadData();
    void saveData();
    
    // Menu Display
    void displayMainMenu();
    void displayUserMenu();
    void displayLibrarianMenu();
    void displayAdminMenu();
};

#endif // LIBRARY_SYSTEM_H 