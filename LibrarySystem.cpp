using namespace std;
#include <bits/stdc++.h>
#include "LibrarySystem.h"
#include "Admin.h"
#include "Librarian.h"
#include "RegisteredUser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// Initialize static member
LibrarySystem* LibrarySystem::instance = nullptr;

LibrarySystem::LibrarySystem() : currentUser(nullptr) {
    // Clear existing data
    for (User* user : users) delete user;
    for (Book* book : books) delete book;
    for (Transaction* transaction : transactions) delete transaction;
    users.clear();
    books.clear();
    transactions.clear();
    
    // Load fresh data
    loadData();
}

LibrarySystem::~LibrarySystem() {
    // Save data before destroying
    saveData();
    
    // Clean up memory
    for (User* user : users) delete user;
    for (Book* book : books) delete book;
    for (Transaction* transaction : transactions) delete transaction;
    users.clear();
    books.clear();
    transactions.clear();
}

LibrarySystem* LibrarySystem::getInstance() {
    if (instance == nullptr) {
        instance = new LibrarySystem();
    }
    return instance;
}

void LibrarySystem::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

// User Management
bool LibrarySystem::login(const string& email, const string& password) {
    for (User* user : users) {
        if (user->login(email, password)) {
            currentUser = user;
            return true;
        }
    }
    return false;
}

void LibrarySystem::logout() {
    currentUser = nullptr;
}

User* LibrarySystem::getCurrentUser() const {
    return currentUser;
}

void LibrarySystem::addUser(User* user) {
    std::cout << "Debug: Adding new user - Email: " << user->getEmail() 
              << ", Role: " << user->getRole() << "\n";
    
    // Check for duplicate email
    for (User* existingUser : users) {
        if (existingUser->getEmail() == user->getEmail()) {
            std::cout << "Debug: User with email " << user->getEmail() << " already exists\n";
            return;
        }
    }
    
    users.push_back(user);
    std::cout << "Debug: Total users after adding: " << users.size() << "\n";
    saveData();
}

void LibrarySystem::removeUser(const string& email) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        if ((*it)->getEmail() == email) {
            delete *it;
            users.erase(it);
            saveData();
            break;
        }
    }
}

void LibrarySystem::updateUserDetails(const string& email, const string& newName, const string& newEmail) {
    for (User* user : users) {
        if (user->getEmail() == email) {
            // Update user details
            user->name = newName;
            user->email = newEmail;
            saveData();
            break;
        }
    }
}

User* LibrarySystem::findUser(const string& email) const {
    for (User* user : users) {
        if (user->getEmail() == email) {
            return user;
        }
    }
    return nullptr;
}

vector<User*> LibrarySystem::getAllUsers() const {
    return users;
}

// Book Management
void LibrarySystem::addBook(Book* book) {
    std::cout << "Debug: Adding new book - ID: " << book->getBookID() 
              << ", Title: " << book->getTitle() << "\n";
    
    // Check for duplicate ID
    for (Book* existingBook : books) {
        if (existingBook->getBookID() == book->getBookID()) {
            std::cout << "Debug: Book with ID " << book->getBookID() << " already exists\n";
            return;
        }
    }
    
    books.push_back(book);
    std::cout << "Debug: Total books after adding: " << books.size() << "\n";
    saveData();
}

void LibrarySystem::removeBook(const string& bookID) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if ((*it)->getBookID() == bookID) {
            delete *it;
            books.erase(it);
            saveData();
            break;
        }
    }
}

Book* LibrarySystem::findBook(const string& bookID) {
    std::cout << "Debug: Looking for book with ID: " << bookID << "\n";
    std::cout << "Debug: Total books in system: " << books.size() << "\n";
    
    for (Book* book : books) {
        std::cout << "Debug: Checking book - ID: " << book->getBookID() 
                  << ", Title: " << book->getTitle() << "\n";
        if (book->getBookID() == bookID) {
            std::cout << "Debug: Found matching book\n";
            return book;
        }
    }
    std::cout << "Debug: Book not found\n";
    return nullptr;
}

vector<Book*> LibrarySystem::searchBooks(const string& query) {
    vector<Book*> results;
    for (Book* book : books) {
        if (book->getTitle().find(query) != string::npos ||
            book->getAuthor().find(query) != string::npos ||
            book->getCategory().find(query) != string::npos) {
            results.push_back(book);
        }
    }
    return results;
}

vector<Book*> LibrarySystem::getAllBooks() const {
    return books;
}

// Transaction Management
void LibrarySystem::createTransaction(Transaction* transaction) {
    transactions.push_back(transaction);
    saveData();
}

vector<Transaction*> LibrarySystem::getUserTransactions(const string& userID) {
    vector<Transaction*> userTransactions;
    for (Transaction* transaction : transactions) {
        if (transaction->getUserID() == userID) {
            userTransactions.push_back(transaction);
        }
    }
    return userTransactions;
}

vector<Transaction*> LibrarySystem::getBookTransactions(const string& bookID) {
    vector<Transaction*> bookTransactions;
    for (Transaction* transaction : transactions) {
        if (transaction->getBookID() == bookID) {
            bookTransactions.push_back(transaction);
        }
    }
    return bookTransactions;
}

// File Operations
void LibrarySystem::loadData() {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories("data");

    // Load Users
    ifstream userFile("data/users.txt");
    if (userFile.is_open()) {
        string line;
        while (getline(userFile, line)) {
            stringstream ss(line);
            string id, name, email, password, role;
            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, email, '|');
            getline(ss, password, '|');
            getline(ss, role, '|');
            
            User* user = nullptr;
            if (role == "admin") {
                user = new Admin(id, name, email, password);
            } else if (role == "librarian") {
                user = new Librarian(id, name, email, password);
            } else if (role == "registered") {
                user = new RegisteredUser(id, name, email, password);
                // Load borrowed books and fines for registered users
                string borrowedBook;
                vector<string> borrowedBooks;
                double fine = 0.0;
                if (getline(ss, borrowedBook, '|')) {
                    stringstream bss(borrowedBook);
                    string bookId;
                    while (getline(bss, bookId, ',')) {
                        if (!bookId.empty()) {
                            borrowedBooks.push_back(bookId);
                        }
                    }
                }
                if (ss >> fine) {
                    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
                    for (const string& bookId : borrowedBooks) {
                        regUser->addBorrowedBook(bookId);
                    }
                    regUser->setOutstandingFine(fine);
                }
            }
            if (user) {
                users.push_back(user);
            }
        }
        userFile.close();
    }

    // Load Books
    ifstream bookFile("data/books.txt");
    if (bookFile.is_open()) {
        string line;
        while (getline(bookFile, line)) {
            stringstream ss(line);
            string id, title, author, category, isbn;
            bool available;
            getline(ss, id, '|');
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, category, '|');
            getline(ss, isbn, '|');
            ss >> available;
            
            Book* book = new Book(id, title, author, category, isbn);
            book->setAvailability(available);
            books.push_back(book);
        }
        bookFile.close();
    }

    // Load Transactions
    ifstream transFile("data/transactions.txt");
    if (transFile.is_open()) {
        string line;
        while (getline(transFile, line)) {
            stringstream ss(line);
            string id, userId, bookId, type;
            time_t transDate, dueDate, returnDate;
            double fine;
            
            getline(ss, id, '|');
            getline(ss, userId, '|');
            getline(ss, bookId, '|');
            getline(ss, type, '|');
            ss >> transDate;
            ss.ignore();
            ss >> dueDate;
            ss.ignore();
            ss >> returnDate;
            ss.ignore();
            ss >> fine;
            
            Transaction* transaction = new Transaction(id, userId, bookId, type, transDate);
            if (type == "return") {
                transaction->setReturnDate(returnDate);
                transaction->setFineAmount(fine);
            }
            transactions.push_back(transaction);
        }
        transFile.close();
    }
}

void LibrarySystem::saveData() {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories("data");

    // Save Users
    ofstream userFile("data/users.txt");
    if (userFile.is_open()) {
        for (User* user : users) {
            userFile << user->getUserID() << "|"
                    << user->getName() << "|"
                    << user->getEmail() << "|"
                    << user->password << "|"
                    << user->getRole() << "|";
            
            if (user->getRole() == "registered") {
                RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
                // Save borrowed books
                const vector<string>& borrowedBooks = regUser->getBorrowedBooks();
                for (size_t i = 0; i < borrowedBooks.size(); ++i) {
                    userFile << borrowedBooks[i];
                    if (i < borrowedBooks.size() - 1) {
                        userFile << ",";
                    }
                }
                userFile << "|" << regUser->getOutstandingFine();
            }
            userFile << "\n";
        }
        userFile.close();
    }

    // Save Books
    ofstream bookFile("data/books.txt");
    if (bookFile.is_open()) {
        for (Book* book : books) {
            bookFile << book->getBookID() << "|"
                    << book->getTitle() << "|"
                    << book->getAuthor() << "|"
                    << book->getCategory() << "|"
                    << book->getISBN() << "|"
                    << book->getAvailability() << "\n";
        }
        bookFile.close();
    }

    // Save Transactions
    ofstream transFile("data/transactions.txt");
    if (transFile.is_open()) {
        for (Transaction* transaction : transactions) {
            transFile << transaction->getTransactionID() << "|"
                     << transaction->getUserID() << "|"
                     << transaction->getBookID() << "|"
                     << transaction->getType() << "|"
                     << transaction->getTransactionDate() << "|"
                     << transaction->getDueDate() << "|"
                     << transaction->getReturnDate() << "|"
                     << transaction->getFineAmount() << "\n";
        }
        transFile.close();
    }
}

// Menu Display
void LibrarySystem::displayMainMenu() {
    cout << "\nLibrary Management System\n"
         << "1. Login\n"
         << "2. Exit\n"
         << "Enter your choice: ";
}

void LibrarySystem::displayUserMenu() {
    cout << "\nUser Menu\n"
         << "1. Book Operations\n"
         << "2. Borrow/Return\n"
         << "3. Profile Management\n"
         << "4. Fine Management\n"
         << "5. View History\n"
         << "6. Logout\n"
         << "Enter your choice: ";
}

void LibrarySystem::displayLibrarianMenu() {
    cout << "\nLibrarian Menu\n"
         << "1. Book Management\n"
         << "2. Issue/Return Books\n"
         << "3. Fine Management\n"
         << "4. Search\n"
         << "5. Logout\n"
         << "Enter your choice: ";
}

void LibrarySystem::displayAdminMenu() {
    cout << "\nAdmin Menu\n"
         << "1. User Management\n"
         << "2. Book Management\n"
         << "3. Logout\n"
         << "Enter your choice: ";
} 