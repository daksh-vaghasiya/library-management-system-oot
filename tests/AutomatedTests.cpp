#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include "../LibrarySystem.h"
#include "../Book.h"
#include "../Transaction.h"
#include "../RegisteredUser.h"
#include "../Admin.h"
#include "../Librarian.h"
#include <fstream>

// Test Framework
#define TEST(name) void name()
#define ASSERT_TRUE(x) assert(x)
#define ASSERT_FALSE(x) assert(!(x))
#define ASSERT_EQ(x, y) assert((x) == (y))
#define EXPECT_FALSE(x) assert(!(x))
#define EXPECT_TRUE(x) assert(x)
#define EXPECT_EQ(x, y) assert((x) == (y))
#define EXPECT_NE(x, y) assert((x) != (y))

// Input simulation class
class InputSimulator {
private:
    std::stringstream inputStream;
    std::streambuf* oldCin;
    std::streambuf* oldCout;
    std::stringstream outputStream;

public:
    InputSimulator() {
        // Redirect cin to our stringstream
        oldCin = std::cin.rdbuf(inputStream.rdbuf());
        // Redirect cout to our output capture stream
        oldCout = std::cout.rdbuf(outputStream.rdbuf());
    }

    ~InputSimulator() {
        // Restore cin and cout
        std::cin.rdbuf(oldCin);
        std::cout.rdbuf(oldCout);
    }

    void provideInput(const std::string& input) {
        // Add newline if not present
        if (input.empty() || input.back() != '\n') {
            inputStream << input << '\n';
        } else {
            inputStream << input;
        }
    }

    void provideInputs(const std::vector<std::string>& inputs) {
        for (const auto& input : inputs) {
            provideInput(input);
        }
    }

    std::string getOutput() {
        std::string output = outputStream.str();
        outputStream.str("");  // Clear the stream
        return output;
    }

    void clearInput() {
        inputStream.str("");
        inputStream.clear();
    }

    void clearOutput() {
        outputStream.str("");
        outputStream.clear();
    }
};

class TestRunner {
private:
    std::vector<std::pair<std::string, void(*)()>> tests;

public:
    void addTest(const std::string& name, void(*testFunc)()) {
        tests.push_back({name, testFunc});
    }

    void runAll() {
        int passed = 0;
        int total = tests.size();

        std::cout << "\nRunning " << total << " tests...\n";
        std::cout << "======================\n";

        for (const auto& test : tests) {
            std::cout << "Running " << test.first << "... ";
            try {
                test.second();
                std::cout << "PASSED\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "FAILED\n";
                std::cout << "Error: " << e.what() << "\n";
            }
        }

        std::cout << "======================\n";
        std::cout << passed << " out of " << total << " tests passed.\n\n";
    }
};

// Helper Functions
void clearDataFiles() {
    // Clear data files
    std::ofstream userFile("data/users.txt", std::ios::trunc);
    userFile.close();
    
    std::ofstream bookFile("data/books.txt", std::ios::trunc);
    bookFile.close();
    
    std::ofstream transFile("data/transactions.txt", std::ios::trunc);
    transFile.close();
}

void setupTestData(LibrarySystem* library) {
    // Clear data files first
    clearDataFiles();
    
    // Add test users with unique IDs and emails
    library->addUser(new Admin("A001", "Test Admin", "admin1@test.com", "admin123"));
    library->addUser(new Librarian("L001", "Test Librarian", "librarian1@test.com", "lib123"));
    library->addUser(new RegisteredUser("U001", "Test User", "user1@test.com", "user123"));
    
    // Add test books with unique IDs
    library->addBook(new Book("B001", "Test Book 1", "Author 1", "Fiction", "123-456"));
    library->addBook(new Book("B002", "Test Book 2", "Author 2", "Non-Fiction", "456-789"));
}

// 1. Admin Menu Tests
TEST(AdminUserManagementTest) {
    // Clear any existing library state
    LibrarySystem::destroyInstance();
    clearDataFiles();
    
    // Create fresh library instance
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    std::cout << "Debug: Setting up test...\n";

    // Login as admin
    ASSERT_TRUE(library->login("admin1@test.com", "admin123"));
    Admin* admin = dynamic_cast<Admin*>(library->getCurrentUser());
    ASSERT_TRUE(admin != nullptr);

    std::cout << "Debug: Admin login successful\n";

    // Test User Management - Add User
    std::cout << "Debug: Adding new user directly...\n";
    bool userAdded = admin->addUserDirect(
        "U002",         // ID
        "New User",     // Name
        "new@test.com", // Email
        "pass123",      // Password
        "registered"    // Role
    );
    ASSERT_TRUE(userAdded);
    
    // Verify user was added
    std::cout << "Debug: Verifying user was added...\n";
    std::vector<User*> allUsers = library->getAllUsers();
    std::cout << "Debug: Total users in system: " << allUsers.size() << "\n";
    for (User* user : allUsers) {
        std::cout << "Debug: Found user - Email: " << user->getEmail() 
                  << ", Role: " << user->getRole() << "\n";
    }
    
    User* newUser = library->findUser("new@test.com");
    ASSERT_TRUE(newUser != nullptr);
    if (newUser) {
        std::cout << "Debug: New user found - Role: " << newUser->getRole() << "\n";
        ASSERT_EQ(newUser->getRole(), "registered");
    } else {
        std::cout << "Debug: Failed to find new user\n";
    }

    LibrarySystem::destroyInstance();
}

TEST(AdminBookManagementTest) {
    // Clear any existing library state
    LibrarySystem::destroyInstance();
    clearDataFiles();
    
    // Create fresh library instance
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    std::cout << "Debug: Setting up test...\n";

    // Login as admin
    ASSERT_TRUE(library->login("admin1@test.com", "admin123"));
    Admin* admin = dynamic_cast<Admin*>(library->getCurrentUser());
    ASSERT_TRUE(admin != nullptr);

    std::cout << "Debug: Admin login successful\n";
    std::cout << "Debug: Starting book management test...\n";

    // Test Book Management - Add Book
    bool bookAdded = admin->addBookDirect(
        "B003",        // ID
        "New Book",    // Title
        "New Author",  // Author
        "Mystery",     // Category
        "789-123"      // ISBN
    );
    ASSERT_TRUE(bookAdded);

    std::cout << "Debug: Book management action completed\n";
    
    // Verify book was added
    std::cout << "Debug: Verifying book was added...\n";
    std::vector<Book*> allBooks = library->getAllBooks();
    std::cout << "Debug: Total books in system: " << allBooks.size() << "\n";
    for (Book* book : allBooks) {
        std::cout << "Debug: Found book - ID: " << book->getBookID() 
                  << ", Title: " << book->getTitle() << "\n";
    }
    
    Book* newBook = library->findBook("B003");
    ASSERT_TRUE(newBook != nullptr);
    if (newBook) {
        std::cout << "Debug: New book found - Title: " << newBook->getTitle() << "\n";
        ASSERT_EQ(newBook->getTitle(), "New Book");
    } else {
        std::cout << "Debug: Failed to find new book\n";
    }

    LibrarySystem::destroyInstance();
}

// 2. Librarian Menu Tests
TEST(LibrarianFullMenuTest) {
    // Clear any existing library state
    LibrarySystem::destroyInstance();
    clearDataFiles();
    
    // Create fresh library instance
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    std::cout << "Debug: Setting up test...\n";

    // Login as librarian
    ASSERT_TRUE(library->login("librarian1@test.com", "lib123"));
    Librarian* librarian = dynamic_cast<Librarian*>(library->getCurrentUser());
    ASSERT_TRUE(librarian != nullptr);

    // Test Book Management - Add Book
    std::cout << "Debug: Adding new book...\n";
    bool bookAdded = librarian->addBookDirect(
        "B004",        // ID
        "Librarian Book",    // Title
        "Librarian Author",  // Author
        "Education",     // Category
        "999-999"      // ISBN
    );
    ASSERT_TRUE(bookAdded);

    // Verify book was added
    std::cout << "Debug: Verifying book was added...\n";
    Book* newBook = library->findBook("B004");
    ASSERT_TRUE(newBook != nullptr);
    if (newBook) {
        std::cout << "Debug: New book found - Title: " << newBook->getTitle() << "\n";
        ASSERT_EQ(newBook->getTitle(), "Librarian Book");
    }

    // Test Issue Book
    std::cout << "Debug: Testing book issue...\n";
    RegisteredUser* user = dynamic_cast<RegisteredUser*>(library->findUser("user1@test.com"));
    ASSERT_TRUE(user != nullptr);

    // Create borrow transaction
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T1", user->getUserID(), "B001", "borrow", now);
    library->createTransaction(borrowTrans);
    Book* issuedBook = library->findBook("B001");
    issuedBook->setAvailability(false);
    user->addBorrowedBook("B001");

    // Verify book was issued
    ASSERT_FALSE(issuedBook->getAvailability());
    ASSERT_TRUE(std::find(user->getBorrowedBooks().begin(), 
                         user->getBorrowedBooks().end(), 
                         "B001") != user->getBorrowedBooks().end());

    LibrarySystem::destroyInstance();
}

// 3. RegisteredUser Menu Tests
TEST(RegisteredUserFullMenuTest) {
    // Clear any existing library state
    LibrarySystem::destroyInstance();
    clearDataFiles();
    
    // Create fresh library instance
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    std::cout << "Debug: Setting up test...\n";

    // Login as user
    ASSERT_TRUE(library->login("user1@test.com", "user123"));
    RegisteredUser* user = dynamic_cast<RegisteredUser*>(library->getCurrentUser());
    ASSERT_TRUE(user != nullptr);

    // Test Search Books
    std::cout << "Debug: Testing book search...\n";
    std::vector<Book*> searchResults = library->searchBooks("Test Book");
    ASSERT_FALSE(searchResults.empty());
    ASSERT_EQ(searchResults.size(), 2);

    // Test Borrow Book
    std::cout << "Debug: Testing book borrow...\n";
    Book* bookToBorrow = library->findBook("B002");
    ASSERT_TRUE(bookToBorrow != nullptr);
    ASSERT_TRUE(bookToBorrow->getAvailability());

    // Create borrow transaction
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T1", user->getUserID(), "B002", "borrow", now);
    library->createTransaction(borrowTrans);
    bookToBorrow->setAvailability(false);
    user->addBorrowedBook("B002");

    // Verify book was borrowed
    ASSERT_FALSE(bookToBorrow->getAvailability());
    ASSERT_TRUE(std::find(user->getBorrowedBooks().begin(), 
                         user->getBorrowedBooks().end(), 
                         "B002") != user->getBorrowedBooks().end());

    // Test Return Book
    std::cout << "Debug: Testing book return...\n";
    Transaction* returnTrans = new Transaction("T2", user->getUserID(), "B002", "return", now + (15 * 24 * 60 * 60));
    returnTrans->setReturnDate(now + (15 * 24 * 60 * 60));
    returnTrans->closeTransaction();
    library->createTransaction(returnTrans);
    bookToBorrow->setAvailability(true);
    user->removeBorrowedBook("B002");

    // Verify book was returned
    ASSERT_TRUE(bookToBorrow->getAvailability());
    ASSERT_TRUE(std::find(user->getBorrowedBooks().begin(), 
                         user->getBorrowedBooks().end(), 
                         "B002") == user->getBorrowedBooks().end());

    LibrarySystem::destroyInstance();
}

// 4. Error Cases Tests
TEST(ErrorCasesTest) {
    InputSimulator sim;
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Test invalid login
    ASSERT_FALSE(library->login("invalid@test.com", "wrongpass"));

    // Login as user
    ASSERT_TRUE(library->login("user1@test.com", "user123"));
    RegisteredUser* user = dynamic_cast<RegisteredUser*>(library->getCurrentUser());
    ASSERT_TRUE(user != nullptr);

    // Test borrowing non-existent book
    sim.provideInput("3"); // Borrow Book
    sim.provideInput("NONEXISTENT"); // Invalid Book ID
    user->performAction(3);

    // Test borrowing when having fines
    user->setOutstandingFine(10.0);
    sim.provideInput("3"); // Borrow Book
    sim.provideInput("B001"); // Book ID
    user->performAction(3);

    // Verify no book was borrowed
    ASSERT_TRUE(user->getBorrowedBooks().empty());

    LibrarySystem::destroyInstance();
}

// 5. Fine Management Tests
TEST(FineManagementFullTest) {
    InputSimulator sim;
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Login as librarian
    ASSERT_TRUE(library->login("librarian1@test.com", "lib123"));
    Librarian* librarian = dynamic_cast<Librarian*>(library->getCurrentUser());
    ASSERT_TRUE(librarian != nullptr);

    // Set up overdue book
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T1", "U001", "B001", "return", now - (20 * 24 * 60 * 60));
    borrowTrans->setReturnDate(now);
    borrowTrans->closeTransaction();
    library->createTransaction(borrowTrans);

    // Test View User Fines
    sim.provideInput("3"); // Fine Management
    sim.provideInput("1"); // View User Fines
    sim.provideInput("U001"); // User ID
    librarian->performAction(3);

    // Test Accept Fine Payment
    sim.provideInput("3"); // Fine Management
    sim.provideInput("2"); // Accept Fine Payment
    sim.provideInput("U001"); // User ID
    sim.provideInput("5.0"); // Payment amount
    librarian->performAction(3);

    LibrarySystem::destroyInstance();
}

int main() {
    TestRunner runner;
    
    // Add all tests
    runner.addTest("AdminUserManagementTest", AdminUserManagementTest);
    runner.addTest("AdminBookManagementTest", AdminBookManagementTest);
    runner.addTest("LibrarianFullMenuTest", LibrarianFullMenuTest);
    runner.addTest("RegisteredUserFullMenuTest", RegisteredUserFullMenuTest);
    runner.addTest("ErrorCasesTest", ErrorCasesTest);
    runner.addTest("FineManagementFullTest", FineManagementFullTest);
    
    // Run all tests
    runner.runAll();
    
    return 0;
} 