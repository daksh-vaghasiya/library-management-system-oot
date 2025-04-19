using namespace std;
#include <bits/stdc++.h>
#include "RegisteredUser.h"
#include "LibrarySystem.h"
#include "Book.h"
#include "Transaction.h"
#include <ctime>
#include <iomanip>  // for setprecision

RegisteredUser::RegisteredUser(const string& id, const string& n, const string& e, const string& p)
    : User(id, n, e, p, "registered"), outstandingFine(0.0) {}

void RegisteredUser::displayMenu() {
    cout << "\nUser Menu\n"
         << "1. Search Books\n"
         << "2. View Book Details\n"
         << "3. Borrow Book\n"
         << "4. Return Book\n"
         << "5. View Current Borrowings\n"
         << "6. View Profile\n"
         << "7. Update Profile\n"
         << "8. Change Password\n"
         << "9. View Fines\n"
         << "10. Pay Fines\n"
         << "11. View Borrowing History\n"
         << "12. View Fine History\n"
         << "13. Logout\n"
         << "Enter your choice: ";
}

void RegisteredUser::performAction(int choice) {
    switch (choice) {
        case 1: searchBooks(); break;
        case 2: viewBookDetails(); break;
        case 3: borrowBook(); break;
        case 4: returnBook(); break;
        case 5: viewCurrentBorrowings(); break;
        case 6: viewProfile(); break;
        case 7: updateProfile(); break;
        case 8: changePassword(); break;
        case 9: viewFines(); break;
        case 10: payFines(); break;
        case 11: viewBorrowingHistory(); break;
        case 12: viewFineHistory(); break;
        case 13: cout << "Logging out...\n"; break;
        default: cout << "Invalid choice\n";
    }
}

void RegisteredUser::searchBooks() {
    string query;
    cout << "Enter search query: "; 
    cin.ignore();
    getline(cin, query);
    
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<Book*> results = library->searchBooks(query);
    
    if (results.empty()) {
        cout << "No books found matching your search.\n";
    } else {
        cout << "\nSearch Results:\n";
        for (Book* book : results) {
            book->displayDetails();
        }
    }
}

void RegisteredUser::viewBookDetails() {
    string bookID;
    cout << "Enter book ID: "; cin >> bookID;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = library->findBook(bookID);
    
    if (book) {
        book->displayDetails();
        
        // Show borrowing history
        vector<Transaction*> bookTransactions = library->getBookTransactions(bookID);
        if (!bookTransactions.empty()) {
            cout << "\nBorrowing History:\n";
            for (Transaction* transaction : bookTransactions) {
                time_t transDate = transaction->getTransactionDate();
                cout << "Transaction ID: " << transaction->getTransactionID() << "\n"
                     << "Type: " << transaction->getType() << "\n"
                     << "Date: " << ctime(&transDate);
                if (transaction->getType() == "borrow") {
                    time_t dueDate = transaction->getDueDate();
                    cout << "Due Date: " << ctime(&dueDate);
                } else {
                    time_t returnDate = transaction->getReturnDate();
                    cout << "Return Date: " << ctime(&returnDate);
                    if (transaction->getFineAmount() > 0) {
                        cout << "Fine: $" << transaction->getFineAmount() << "\n";
                    }
                }
                cout << "------------------------\n";
            }
        }
    } else {
        cout << "Book not found.\n";
    }
}

void RegisteredUser::borrowBook() {
    string bookID;
    cout << "Enter book ID to borrow: "; cin >> bookID;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = library->findBook(bookID);
    
    if (!book) {
        cout << "Book not found.\n";
        return;
    }
    
    if (!book->getAvailability()) {
        cout << "Book is currently not available.\n";
        return;
    }
    
    if (borrowedBooks.size() >= 5) {
        cout << "You have reached the maximum limit of borrowed books (5).\n";
        return;
    }
    
    // Create transaction
    time_t now = time(nullptr);
    string transactionID = "T" + to_string(now);
    Transaction* transaction = new Transaction(transactionID, getUserID(), bookID, "borrow", now);
    library->createTransaction(transaction);
    
    // Update book status and user's borrowed books
    book->setAvailability(false);
    addBorrowedBook(bookID);
    
    time_t dueDate = transaction->getDueDate();
    cout << "Book borrowed successfully. Due date: " << ctime(&dueDate);
}

void RegisteredUser::returnBook() {
    string bookID;
    cout << "Enter book ID to return: "; cin >> bookID;
    
    // Check if user has borrowed this book
    if (find(borrowedBooks.begin(), borrowedBooks.end(), bookID) == borrowedBooks.end()) {
        cout << "You haven't borrowed this book.\n";
        return;
    }
    
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = library->findBook(bookID);
    
    if (!book) {
        cout << "Book not found in system.\n";
        return;
    }
    
    // Create return transaction
    time_t now = time(nullptr);
    string transactionID = "T" + to_string(now);
    Transaction* transaction = new Transaction(transactionID, getUserID(), bookID, "return", now);
    transaction->setReturnDate(now);
    transaction->closeTransaction();
    library->createTransaction(transaction);
    
    // Update book status and user's borrowed books
    book->setAvailability(true);
    removeBorrowedBook(bookID);
    
    if (transaction->getFineAmount() > 0) {
        outstandingFine += transaction->getFineAmount();
        cout << "Book returned with fine: $" << transaction->getFineAmount() << "\n";
    } else {
        cout << "Book returned successfully.\n";
    }
}

void RegisteredUser::viewCurrentBorrowings() {
    if (borrowedBooks.empty()) {
        cout << "You haven't borrowed any books.\n";
        return;
    }
    
    cout << "Your current borrowings:\n";
    LibrarySystem* library = LibrarySystem::getInstance();
    
    for (const string& bookID : borrowedBooks) {
        Book* book = library->findBook(bookID);
        if (book) {
            book->displayDetails();
            
            // Show due date
            vector<Transaction*> transactions = library->getUserTransactions(getUserID());
            for (Transaction* transaction : transactions) {
                if (transaction->getBookID() == bookID && transaction->getType() == "borrow") {
                    time_t dueDate = transaction->getDueDate();
                    cout << "Due Date: " << ctime(&dueDate);
                    if (transaction->isOverdue()) {
                        cout << "WARNING: This book is overdue!\n";
                    }
                    break;
                }
            }
        }
    }
}

void RegisteredUser::viewProfile() {
    cout << "\nUser Profile\n"
         << "ID: " << getUserID() << "\n"
         << "Name: " << getName() << "\n"
         << "Email: " << getEmail() << "\n"
         << "Role: " << getRole() << "\n"
         << "Outstanding Fine: $" << fixed << setprecision(2) << outstandingFine << "\n"
         << "Currently Borrowed Books: " << borrowedBooks.size() << "\n";
}

void RegisteredUser::updateProfile() {
    string newName, newEmail;
    cout << "Enter new details (leave blank to keep current):\n";
    cout << "Name: "; 
    cin.ignore();
    getline(cin, newName);
    cout << "Email: "; 
    getline(cin, newEmail);
    
    LibrarySystem* library = LibrarySystem::getInstance();
    library->updateUserDetails(getEmail(), newName.empty() ? getName() : newName, 
                             newEmail.empty() ? getEmail() : newEmail);
    
    cout << "Profile updated successfully\n";
}

void RegisteredUser::changePassword() {
    string currentPassword, newPassword;
    cout << "Enter current password: "; cin >> currentPassword;
    cout << "Enter new password: "; cin >> newPassword;
    
    if (currentPassword == password) {
        password = newPassword;
        cout << "Password changed successfully\n";
    } else {
        cout << "Current password is incorrect\n";
    }
}

void RegisteredUser::viewFines() {
    cout << "Your current fine: $" << fixed << setprecision(2) << outstandingFine << "\n";
    
    if (outstandingFine > 0) {
        LibrarySystem* library = LibrarySystem::getInstance();
        vector<Transaction*> transactions = library->getUserTransactions(getUserID());
        
        cout << "\nFine Details:\n";
        for (Transaction* transaction : transactions) {
            if (transaction->getType() == "return" && transaction->getFineAmount() > 0) {
                Book* book = library->findBook(transaction->getBookID());
                if (book) {
                    time_t returnDate = transaction->getReturnDate();
                    cout << "Book: " << book->getTitle() << "\n"
                         << "Fine Amount: $" << fixed << setprecision(2) << transaction->getFineAmount() << "\n"
                         << "Return Date: " << ctime(&returnDate) << "\n";
                }
            }
        }
    }
}

void RegisteredUser::payFines() {
    cout << "Current outstanding fine: $" << fixed << setprecision(2) << outstandingFine << "\n";
    cout << "Enter amount to pay: $"; 
    double amount;
    cin >> amount;
    
    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }
    
    if (amount > outstandingFine) {
        cout << "Payment amount exceeds outstanding fine.\n";
        return;
    }
    
    outstandingFine -= amount;
    cout << "Payment successful. Remaining fine: $" << fixed << setprecision(2) << outstandingFine << "\n";
}

void RegisteredUser::viewBorrowingHistory() {
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<Transaction*> transactions = library->getUserTransactions(getUserID());
    
    if (transactions.empty()) {
        cout << "No borrowing history found.\n";
        return;
    }
    
    cout << "\nBorrowing History:\n";
    for (Transaction* transaction : transactions) {
        if (transaction->getType() == "borrow") {
            Book* book = library->findBook(transaction->getBookID());
            if (book) {
                time_t transDate = transaction->getTransactionDate();
                time_t dueDate = transaction->getDueDate();
                cout << "Book: " << book->getTitle() << "\n"
                     << "Borrowed on: " << ctime(&transDate)
                     << "Due date: " << ctime(&dueDate) << "\n";
                
                // Check if book was returned
                bool returned = false;
                for (Transaction* returnTransaction : transactions) {
                    if (returnTransaction->getType() == "return" && 
                        returnTransaction->getBookID() == transaction->getBookID() &&
                        returnTransaction->getTransactionDate() > transaction->getTransactionDate()) {
                        returned = true;
                        time_t returnDate = returnTransaction->getReturnDate();
                        cout << "Returned on: " << ctime(&returnDate);
                        if (returnTransaction->getFineAmount() > 0) {
                            cout << "Fine paid: $" << fixed << setprecision(2) << returnTransaction->getFineAmount() << "\n";
                        }
                        break;
                    }
                }
                if (!returned) {
                    cout << "Status: Currently Borrowed\n";
                }
                cout << "------------------------\n";
            }
        }
    }
}

void RegisteredUser::viewFineHistory() {
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<Transaction*> transactions = library->getUserTransactions(getUserID());
    
    if (transactions.empty()) {
        cout << "No fine history found.\n";
        return;
    }
    
    cout << "\nFine History:\n";
    for (Transaction* transaction : transactions) {
        if (transaction->getType() == "return" && transaction->getFineAmount() > 0) {
            Book* book = library->findBook(transaction->getBookID());
            if (book) {
                time_t returnDate = transaction->getReturnDate();
                cout << "Book: " << book->getTitle() << "\n"
                     << "Returned on: " << ctime(&returnDate)
                     << "Fine amount: $" << fixed << setprecision(2) << transaction->getFineAmount() << "\n"
                     << "------------------------\n";
            }
        }
    }
}

// Getters
double RegisteredUser::getOutstandingFine() const {
    return outstandingFine;
}

const vector<string>& RegisteredUser::getBorrowedBooks() const {
    return borrowedBooks;
}

// Setters
void RegisteredUser::addBorrowedBook(const string& bookID) {
    borrowedBooks.push_back(bookID);
}

void RegisteredUser::removeBorrowedBook(const string& bookID) {
    auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookID);
    if (it != borrowedBooks.end()) {
        borrowedBooks.erase(it);
    }
}

void RegisteredUser::setOutstandingFine(double amount) {
    outstandingFine = amount;
} 