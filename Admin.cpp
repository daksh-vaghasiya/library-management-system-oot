using namespace std;
#include <bits/stdc++.h>
#include "Admin.h"
#include "LibrarySystem.h"
#include "Book.h"
#include "Librarian.h"
#include "RegisteredUser.h"

Admin::Admin(const string& id, const string& n, const string& e, const string& p)
    : User(id, n, e, p, "admin") {}

void Admin::displayMenu() {
    cout << "\nAdmin Menu\n"
         << "1. User Management\n"
         << "2. Book Management\n"
         << "3. Logout\n"
         << "Enter your choice: ";
}

void Admin::performAction(int choice) {
    LibrarySystem* library = LibrarySystem::getInstance();
    
    switch (choice) {
        case 1: {
            cout << "\nUser Management\n"
                 << "1. Add User\n"
                 << "2. Remove User\n"
                 << "3. Modify User Role\n"
                 << "4. View All Users\n"
                 << "Enter your choice: ";
            int subChoice;
            cin >> subChoice;
            cin.ignore();  // Clear the newline
            
            switch (subChoice) {
                case 1: {
                    string id, name, email, password, role;
                    cout << "Enter user details:\n";
                    cout << "ID: ";
                    getline(cin, id);
                    cout << "Name: ";
                    getline(cin, name);
                    cout << "Email: ";
                    getline(cin, email);
                    cout << "Password: ";
                    getline(cin, password);
                    cout << "Role (admin/librarian/registered): ";
                    getline(cin, role);
                    
                    if (library->findUser(email)) {
                        cout << "User with this email already exists.\n";
                        return;
                    }
                    
                    User* newUser = nullptr;
                    if (role == "admin") {
                        newUser = new Admin(id, name, email, password);
                    } else if (role == "librarian") {
                        newUser = new Librarian(id, name, email, password);
                    } else if (role == "registered") {
                        newUser = new RegisteredUser(id, name, email, password);
                    } else {
                        cout << "Invalid role specified. Valid roles are: admin, librarian, registered\n";
                        return;
                    }
                    
                    library->addUser(newUser);
                    cout << "User added successfully\n";
                    break;
                }
                case 2: removeUser(); break;
                case 3: modifyUserRole(); break;
                case 4: viewAllUsers(); break;
                default: cout << "Invalid choice\n";
            }
            break;
        }
        case 2: {
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
        case 3:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice\n";
    }
}

void Admin::removeUser() {
    string email;
    cout << "Enter user email to remove: "; cin >> email;
    
    LibrarySystem* library = LibrarySystem::getInstance();
    User* user = library->findUser(email);
    
    if (!user) {
        cout << "User not found.\n";
        return;
    }
    
    // Don't allow removing the last admin
    if (user->getRole() == "admin") {
        vector<User*> allUsers = library->getAllUsers();
        int adminCount = 0;
        for (User* u : allUsers) {
            if (u->getRole() == "admin") adminCount++;
        }
        if (adminCount <= 1) {
            cout << "Cannot remove the last admin user.\n";
            return;
        }
    }
    
    library->removeUser(email);
    cout << "User removed successfully\n";
}

void Admin::modifyUserRole() {
    string email, newRole;
    cout << "Enter user email: "; cin >> email;
    cout << "Enter new role (admin/librarian/user): "; cin >> newRole;
    
    if (newRole != "admin" && newRole != "librarian" && newRole != "user") {
        cout << "Invalid role specified.\n";
        return;
    }
    
    LibrarySystem* library = LibrarySystem::getInstance();
    User* user = library->findUser(email);
    
    if (!user) {
        cout << "User not found.\n";
        return;
    }
    
    // Don't allow changing role of the last admin
    if (user->getRole() == "admin") {
        vector<User*> allUsers = library->getAllUsers();
        int adminCount = 0;
        for (User* u : allUsers) {
            if (u->getRole() == "admin") adminCount++;
        }
        if (adminCount <= 1) {
            cout << "Cannot modify role of the last admin user.\n";
            return;
        }
    }
    
    // Create new user with updated role
    User* newUser = nullptr;
    if (newRole == "admin") {
        newUser = new Admin(user->getUserID(), user->getName(), email, password);
    } else if (newRole == "librarian") {
        newUser = new Librarian(user->getUserID(), user->getName(), email, password);
    } else {
        newUser = new RegisteredUser(user->getUserID(), user->getName(), email, password);
    }
    
    library->removeUser(email);
    library->addUser(newUser);
    cout << "User role modified successfully\n";
}

void Admin::viewAllUsers() {
    LibrarySystem* library = LibrarySystem::getInstance();
    vector<User*> users = library->getAllUsers();
    
    if (users.empty()) {
        cout << "No users found in the system.\n";
        return;
    }
    
    cout << "\nList of all users:\n";
    cout << string(50, '-') << "\n";
    cout << left << setw(10) << "ID"
         << setw(20) << "Name"
         << setw(30) << "Email"
         << setw(10) << "Role" << "\n";
    cout << string(50, '-') << "\n";
    
    for (User* user : users) {
        cout << left << setw(10) << user->getUserID()
             << setw(20) << user->getName()
             << setw(30) << user->getEmail()
             << setw(10) << user->getRole() << "\n";
    }
    cout << string(50, '-') << "\n";
}

void Admin::addBook() {
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

void Admin::updateBook() {
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

void Admin::removeBook() {
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

void Admin::viewAllBooks() {
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

bool Admin::addUserDirect(const string& id, const string& name, const string& email,
                         const string& password, const string& role) {
    LibrarySystem* library = LibrarySystem::getInstance();
    
    // Check if user already exists
    if (library->findUser(email)) {
        return false;
    }
    
    // Create appropriate user type
    User* newUser = nullptr;
    if (role == "admin") {
        newUser = new Admin(id, name, email, password);
    } else if (role == "librarian") {
        newUser = new Librarian(id, name, email, password);
    } else if (role == "registered") {
        newUser = new RegisteredUser(id, name, email, password);
    } else {
        return false;
    }
    
    library->addUser(newUser);
    return true;
}

bool Admin::addBookDirect(const string& id, const string& title, const string& author,
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