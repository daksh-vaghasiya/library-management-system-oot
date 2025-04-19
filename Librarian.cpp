using namespace std;
#include <bits/stdc++.h>
#include "Librarian.h"
#include "LibrarySystem.h"
#include "Book.h"
#include "Transaction.h"
#include "RegisteredUser.h"
#include <ctime>
#include <iomanip>

Librarian::Librarian(const string& id, const string& n, const string& e, const string& p)
    : User(id, n, e, p, "librarian") {}

void Librarian::displayMenu() {
    cout << "\nLibrarian Menu\n"
         << "1. Book Management\n"
         << "2. Issue/Return Books\n"
         << "3. Fine Management\n"
         << "4. Search\n"
         << "5. Logout\n"
         << "Enter your choice: ";
}

void Librarian::performAction(int choice) {
    switch (choice) {
        case 1: {
            cout << "\nBook Management\n"
                 << "1. Add Book\n"
                 << "2. Update Book\n"
                 << "3. Remove Book\n"
                 << "4. View All Books\n"
                 << "Enter your choice: ";
            int subChoice;
            cin >> subChoice;
            switch (subChoice) {
                case 1: addBook(); break;
                case 2: updateBook(); break;
                case 3: removeBook(); break;
                case 4: viewAllBooks(); break;
                default: cout << "Invalid choice\n";
            }
            break;
        }
        case 2: {
            cout << "\nIssue/Return Books\n"
                 << "1. Issue Book\n"
                 << "2. Accept Return\n"
                 << "3. View Current Issues\n"
                 << "Enter your choice: ";
            int subChoice;
            cin >> subChoice;
            switch (subChoice) {
                case 1: issueBook(); break;
                case 2: acceptReturn(); break;
                case 3: viewCurrentIssues(); break;
                default: cout << "Invalid choice\n";
            }
            break;
        }
        case 3: {
            cout << "\nFine Management\n"
                 << "1. View User Fines\n"
                 << "2. Accept Fine Payment\n"
                 << "Enter your choice: ";
            int subChoice;
            cin >> subChoice;
            switch (subChoice) {
                case 1: viewUserFines(); break;
                case 2: acceptFinePayment(); break;
                default: cout << "Invalid choice\n";
            }
            break;
        }
        case 4: {
            cout << "\nSearch\n"
                 << "1. Search Books\n"
                 << "2. Search Users\n"
                 << "Enter your choice: ";
            int subChoice;
            cin >> subChoice;
            switch (subChoice) {
                case 1: searchBooks(); break;
                case 2: searchUsers(); break;
                default: cout << "Invalid choice\n";
            }
            break;
        }
        case 5:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice\n";
    }
}

void Librarian::addBook() {
    string id, title, author, category, isbn;
    cout << "Enter book details:\n";
    cout << "ID: "; cin >> id;
    cout << "Title: "; 
    cin.ignore();
    getline(cin, title);
    cout << "Author: "; 
    getline(cin, author);
    cout << "Category: "; 
    getline(cin, category);
    cout << "ISBN: "; cin >> isbn;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Check if book already exists
    if (library->findBook(id)) {
        cout << "Book with this ID already exists.\n";
        return;
    }
    
    Book* newBook = new Book(id, title, author, category, isbn);
    library->addBook(newBook);
    cout << "Book added successfully\n";
}

void Librarian::updateBook() {
    string id;
    cout << "Enter book ID to update: "; cin >> id;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = library->findBook(id);
    
    if (!book) {
        cout << "Book not found.\n";
        return;
    }
    
    string title, author, category;
    cout << "Enter new details (leave blank to keep current):\n";
    cin.ignore();
    cout << "Title [" << book->getTitle() << "]: ";
    getline(cin, title);
    cout << "Author [" << book->getAuthor() << "]: ";
    getline(cin, author);
    cout << "Category [" << book->getCategory() << "]: ";
    getline(cin, category);
    
    if (!title.empty()) book->setTitle(title);
    if (!author.empty()) book->setAuthor(author);
    if (!category.empty()) book->setCategory(category);
    
    cout << "Book updated successfully\n";
}

void Librarian::removeBook() {
    string id;
    cout << "Enter book ID to remove: "; cin >> id;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = library->findBook(id);
    
    if (!book) {
        cout << "Book not found.\n";
        return;
    }
    
    if (!book->getAvailability()) {
        cout << "Cannot remove book as it is currently borrowed.\n";
        return;
    }
    
    library->removeBook(id);
    cout << "Book removed successfully\n";
}

void Librarian::viewAllBooks() {
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<Book*> books = library->getAllBooks();
    
    if (books.empty()) {
        cout << "No books found in the system.\n";
        return;
    }
    
    cout << "\nList of all books:\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(10) << "ID"
         << setw(30) << "Title"
         << setw(20) << "Author"
         << setw(15) << "Category"
         << setw(15) << "ISBN"
         << setw(10) << "Status" << "\n";
    cout << string(100, '-') << "\n";
    
    for (Book* book : books) {
        cout << left << setw(10) << book->getBookID()
             << setw(30) << book->getTitle()
             << setw(20) << book->getAuthor()
             << setw(15) << book->getCategory()
             << setw(15) << book->getISBN()
             << setw(10) << (book->getAvailability() ? "Available" : "Borrowed") << "\n";
    }
    cout << string(100, '-') << "\n";
}

void Librarian::issueBook() {
    string userId, bookId;
    cout << "Enter user ID: "; 
    cin.ignore();
    getline(cin, userId);
    cout << "Enter book ID: "; 
    getline(cin, bookId);
    
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Find user and book
    User* user = nullptr;
    for (User* u : library->getAllUsers()) {
        if (u->getUserID() == userId) {
            user = u;
            break;
        }
    }
    
    if (!user) {
        cout << "Error: User ID '" << userId << "' not found.\n";
        return;
    }
    
    if (user->getRole() != "registered") {
        cout << "Error: Only registered users can borrow books. This user is a " << user->getRole() << ".\n";
        return;
    }
    
    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
    if (!regUser) {
        cout << "Error: Failed to process user data.\n";
        return;
    }
    
    Book* book = library->findBook(bookId);
    if (!book) {
        cout << "Error: Book ID '" << bookId << "' not found.\n";
        return;
    }
    
    if (!book->getAvailability()) {
        cout << "Error: Book '" << book->getTitle() << "' is currently not available.\n";
        // Show who has borrowed it
        vector<Transaction*> bookTrans = library->getBookTransactions(bookId);
        for (Transaction* t : bookTrans) {
            if (t->getType() == "borrow" && !t->getReturnDate()) {
                User* borrower = library->findUser(t->getUserID());
                if (borrower) {
                    time_t dueDate = t->getDueDate();
                    cout << "Currently borrowed by: " << borrower->getName() << "\n";
                    cout << "Due date: " << ctime(&dueDate);
                }
                break;
            }
        }
        return;
    }
    
    if (regUser->getBorrowedBooks().size() >= 5) {
        cout << "Error: User has reached the maximum limit of borrowed books (5).\n";
        cout << "Currently borrowed books:\n";
        for (const string& id : regUser->getBorrowedBooks()) {
            Book* b = library->findBook(id);
            if (b) cout << "- " << b->getTitle() << "\n";
        }
        return;
    }
    
    if (regUser->getOutstandingFine() > 0) {
        cout << "Error: User has outstanding fines ($" << regUser->getOutstandingFine() << ").\n";
        cout << "Please clear fines before borrowing.\n";
        return;
    }
    
    // Check if user already has this book
    if (find(regUser->getBorrowedBooks().begin(), regUser->getBorrowedBooks().end(), bookId) 
        != regUser->getBorrowedBooks().end()) {
        cout << "Error: User has already borrowed this book.\n";
        return;
    }
    
    // Create transaction
    time_t now = time(nullptr);
    string transactionID = "T" + to_string(now) + "_" + userId + "_" + bookId;
    Transaction* transaction = new Transaction(transactionID, userId, bookId, "borrow", now);
    library->createTransaction(transaction);
    
    // Update book status and user's borrowed books
    book->setAvailability(false);
    regUser->addBorrowedBook(bookId);
    
    // Display success message with details
    time_t dueDate = transaction->getDueDate();
    cout << "\nBook issued successfully!\n";
    cout << "Book: " << book->getTitle() << "\n";
    cout << "Issued to: " << user->getName() << "\n";
    cout << "Issue date: " << ctime(&now);
    cout << "Due date: " << ctime(&dueDate);
    cout << "Please return the book on time to avoid fines ($1 per day).\n";
}

void Librarian::acceptReturn() {
    string userId, bookId;
    cout << "Enter user ID: "; cin >> userId;
    cout << "Enter book ID: "; cin >> bookId;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Find user and book
    User* user = nullptr;
    for (User* u : library->getAllUsers()) {
        if (u->getUserID() == userId) {
            user = u;
            break;
        }
    }
    
    if (!user) {
        cout << "User not found.\n";
        return;
    }
    
    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
    if (!regUser) {
        cout << "Error: Invalid user type.\n";
        return;
    }
    
    // Check if user has borrowed this book
    const vector<string>& borrowedBooks = regUser->getBorrowedBooks();
    if (find(borrowedBooks.begin(), borrowedBooks.end(), bookId) == borrowedBooks.end()) {
        cout << "This book was not borrowed by this user.\n";
        return;
    }
    
    Book* book = library->findBook(bookId);
    if (!book) {
        cout << "Book not found in system.\n";
        return;
    }
    
    // Create return transaction
    time_t now = time(nullptr);
    string transactionID = "T" + to_string(now);
    Transaction* transaction = new Transaction(transactionID, userId, bookId, "return", now);
    transaction->setReturnDate(now);
    transaction->closeTransaction();
    library->createTransaction(transaction);
    
    // Update book status and user's borrowed books
    book->setAvailability(true);
    regUser->removeBorrowedBook(bookId);
    
    if (transaction->getFineAmount() > 0) {
        regUser->setOutstandingFine(regUser->getOutstandingFine() + transaction->getFineAmount());
        cout << "Book returned with fine: $" << fixed << setprecision(2) << transaction->getFineAmount() << "\n";
    } else {
        cout << "Book returned successfully.\n";
    }
}

void Librarian::viewCurrentIssues() {
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<User*> users = library->getAllUsers();
    bool found = false;
    
    cout << "\nCurrent Book Issues:\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(20) << "User"
         << setw(30) << "Book"
         << setw(25) << "Due Date"
         << setw(15) << "Status" << "\n";
    cout << string(100, '-') << "\n";
    
    for (User* user : users) {
        if (user->getRole() == "registered") {
            RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
            if (regUser && !regUser->getBorrowedBooks().empty()) {
                for (const string& bookId : regUser->getBorrowedBooks()) {
                    Book* book = library->findBook(bookId);
                    if (book) {
                        // Find the borrow transaction
                        vector<Transaction*> transactions = library->getUserTransactions(user->getUserID());
                        for (Transaction* transaction : transactions) {
                            if (transaction->getBookID() == bookId && transaction->getType() == "borrow") {
                                found = true;
                                time_t dueDate = transaction->getDueDate();
                                cout << left << setw(20) << user->getName()
                                     << setw(30) << book->getTitle()
                                     << setw(25) << ctime(&dueDate)
                                     << setw(15) << (transaction->isOverdue() ? "OVERDUE" : "Active") << "\n";
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (!found) {
        cout << "No current book issues.\n";
    }
    cout << string(100, '-') << "\n";
}

void Librarian::viewUserFines() {
    string userId;
    cout << "Enter user ID: "; cin >> userId;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Find user
    User* user = nullptr;
    for (User* u : library->getAllUsers()) {
        if (u->getUserID() == userId) {
            user = u;
            break;
        }
    }
    
    if (!user) {
        cout << "User not found.\n";
        return;
    }
    
    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
    if (!regUser) {
        cout << "Error: Invalid user type.\n";
        return;
    }
    
    cout << "\nFine Details for " << user->getName() << ":\n";
    cout << "Outstanding Fine: $" << fixed << setprecision(2) << regUser->getOutstandingFine() << "\n\n";
    
    if (regUser->getOutstandingFine() > 0) {
        vector<Transaction*> transactions = library->getUserTransactions(userId);
        cout << "Fine Breakdown:\n";
        cout << string(80, '-') << "\n";
        cout << left << setw(30) << "Book"
             << setw(25) << "Return Date"
             << setw(15) << "Fine Amount" << "\n";
        cout << string(80, '-') << "\n";
        
        for (Transaction* transaction : transactions) {
            if (transaction->getType() == "return" && transaction->getFineAmount() > 0) {
                Book* book = library->findBook(transaction->getBookID());
                if (book) {
                    time_t returnDate = transaction->getReturnDate();
                    cout << left << setw(30) << book->getTitle()
                         << setw(25) << ctime(&returnDate)
                         << "$" << fixed << setprecision(2) << transaction->getFineAmount() << "\n";
                }
            }
        }
        cout << string(80, '-') << "\n";
    }
}

void Librarian::acceptFinePayment() {
    string userId;
    double amount;
    cout << "Enter user ID: "; cin >> userId;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Find user
    User* user = nullptr;
    for (User* u : library->getAllUsers()) {
        if (u->getUserID() == userId) {
            user = u;
            break;
        }
    }
    
    if (!user) {
        cout << "User not found.\n";
        return;
    }
    
    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
    if (!regUser) {
        cout << "Error: Invalid user type.\n";
        return;
    }
    
    double outstandingFine = regUser->getOutstandingFine();
    if (outstandingFine == 0) {
        cout << "User has no outstanding fines.\n";
        return;
    }
    
    cout << "Current outstanding fine: $" << fixed << setprecision(2) << outstandingFine << "\n";
    cout << "Enter payment amount: $"; cin >> amount;
    
    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }
    
    if (amount > outstandingFine) {
        cout << "Payment amount exceeds outstanding fine. Accepting $" << fixed << setprecision(2) << outstandingFine << " instead.\n";
        amount = outstandingFine;
    }
    
    regUser->setOutstandingFine(outstandingFine - amount);
    cout << "Payment of $" << fixed << setprecision(2) << amount << " accepted.\n";
    cout << "Remaining fine: $" << fixed << setprecision(2) << regUser->getOutstandingFine() << "\n";
}

void Librarian::searchBooks() {
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
            
            // Show borrowing status if book is borrowed
            if (!book->getAvailability()) {
                vector<Transaction*> transactions = library->getBookTransactions(book->getBookID());
                for (Transaction* transaction : transactions) {
                    if (transaction->getType() == "borrow") {
                        User* borrower = nullptr;
                        for (User* u : library->getAllUsers()) {
                            if (u->getUserID() == transaction->getUserID()) {
                                borrower = u;
                                break;
                            }
                        }
                        if (borrower) {
                            time_t dueDate = transaction->getDueDate();
                            cout << "Borrowed by: " << borrower->getName() << "\n"
                                 << "Due Date: " << ctime(&dueDate) << "\n";
                        }
                        break;
                    }
                }
            }
            cout << "------------------------\n";
        }
    }
}

void Librarian::searchUsers() {
    string query;
    cout << "Enter search query (name or email): "; 
    cin.ignore();
    getline(cin, query);
    
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<User*> users = library->getAllUsers();
    bool found = false;
    
    cout << "\nSearch Results:\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(10) << "ID"
         << setw(20) << "Name"
         << setw(30) << "Email"
         << setw(15) << "Role"
         << setw(15) << "Books Borrowed" << "\n";
    cout << string(100, '-') << "\n";
    
    transform(query.begin(), query.end(), query.begin(), ::tolower);
    
    for (User* user : users) {
        string name = user->getName();
        string email = user->getEmail();
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        transform(email.begin(), email.end(), email.begin(), ::tolower);
        
        if (name.find(query) != string::npos || email.find(query) != string::npos) {
            found = true;
            int borrowedCount = 0;
            if (user->getRole() == "registered") {
                RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
                if (regUser) {
                    borrowedCount = regUser->getBorrowedBooks().size();
                }
            }
            
            cout << left << setw(10) << user->getUserID()
                 << setw(20) << user->getName()
                 << setw(30) << user->getEmail()
                 << setw(15) << user->getRole()
                 << setw(15) << borrowedCount << "\n";
        }
    }
    
    if (!found) {
        cout << "No users found matching your search.\n";
    }
    cout << string(100, '-') << "\n";
}

bool Librarian::addBookDirect(const string& id, const string& title, const string& author,
                            const string& category, const string& isbn) {
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Check if book already exists
    if (library->findBook(id)) {
        return false;
    }
    
    Book* newBook = new Book(id, title, author, category, isbn);
    library->addBook(newBook);
    return true;
} 