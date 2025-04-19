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

// Test Framework
#define TEST(name) void name()
#define ASSERT_TRUE(x) assert(x)
#define ASSERT_FALSE(x) assert(!(x))
#define ASSERT_EQ(x, y) assert((x) == (y))
#define EXPECT_FALSE(x) assert(!(x))
#define EXPECT_TRUE(x) assert(x)
#define EXPECT_EQ(x, y) assert((x) == (y))
#define EXPECT_NE(x, y) assert((x) != (y))

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
void setupTestData(LibrarySystem* library) {
    // Add test users
    library->addUser(new Admin("A001", "Test Admin", "admin@test.com", "admin123"));
    library->addUser(new Librarian("L001", "Test Librarian", "librarian@test.com", "lib123"));
    library->addUser(new RegisteredUser("U001", "Test User", "user@test.com", "user123"));
    
    // Add test books
    library->addBook(new Book("B001", "Test Book 1", "Author 1", "Fiction", "123-456"));
    library->addBook(new Book("B002", "Test Book 2", "Author 2", "Non-Fiction", "456-789"));
}

// 1. Authentication Tests
TEST(AuthenticationTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Valid login tests
    ASSERT_TRUE(library->login("admin@test.com", "admin123"));
    ASSERT_EQ(library->getCurrentUser()->getRole(), "admin");
    library->logout();

    ASSERT_TRUE(library->login("librarian@test.com", "lib123"));
    ASSERT_EQ(library->getCurrentUser()->getRole(), "librarian");
    library->logout();

    // Invalid login tests
    ASSERT_FALSE(library->login("", "")); // Empty credentials
    ASSERT_FALSE(library->login("admin@test.com", "wrongpass")); // Wrong password
    ASSERT_FALSE(library->login("nonexistent@test.com", "pass")); // Non-existent user

    LibrarySystem::destroyInstance();
}

// 2. Admin Menu Tests
TEST(AdminMenuTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);
    library->login("admin@test.com", "admin123");
    Admin* admin = dynamic_cast<Admin*>(library->getCurrentUser());
    ASSERT_TRUE(admin != nullptr);

    // Test User Management
    // Add User
    admin->performAction(1); // User Management
    // Test with valid input
    // Test with duplicate email
    // Test with invalid role

    // Remove User
    // Test removing existing user
    // Test removing non-existent user
    // Test removing last admin

    // Modify User Role
    // Test valid role modification
    // Test invalid role
    // Test modifying last admin

    LibrarySystem::destroyInstance();
}

// 3. Librarian Menu Tests
TEST(LibrarianMenuTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);
    library->login("librarian@test.com", "lib123");
    Librarian* librarian = dynamic_cast<Librarian*>(library->getCurrentUser());
    ASSERT_TRUE(librarian != nullptr);

    // Test Book Management
    // Add Book
    // Test with valid input
    // Test with duplicate ID
    // Test with empty fields

    // Issue Book
    // Test valid issue
    // Test to unavailable book
    // Test to user with outstanding fines
    // Test to user with max books
    // Test non-existent book/user

    // Accept Return
    // Test valid return
    // Test overdue return with fine
    // Test non-borrowed book return

    LibrarySystem::destroyInstance();
}

// 4. RegisteredUser Menu Tests
TEST(RegisteredUserMenuTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);
    library->login("user@test.com", "user123");
    RegisteredUser* user = dynamic_cast<RegisteredUser*>(library->getCurrentUser());
    ASSERT_TRUE(user != nullptr);

    // Search Books
    // Test valid search
    // Test empty search
    // Test non-existent book

    // Borrow Book
    // Test valid borrow
    // Test unavailable book
    // Test with outstanding fines
    // Test exceeding limit

    // Return Book
    // Test valid return
    // Test non-borrowed book
    // Test overdue book

    LibrarySystem::destroyInstance();
}

// 5. Book Management Tests
TEST(BookManagementTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Add Book
    Book* newBook = new Book("B003", "New Book", "New Author", "Mystery", "789-123");
    library->addBook(newBook);
    ASSERT_TRUE(library->findBook("B003") != nullptr);

    // Remove Book
    library->removeBook("B003");
    ASSERT_TRUE(library->findBook("B003") == nullptr);

    // Search Books
    std::vector<Book*> results = library->searchBooks("Test");
    ASSERT_FALSE(results.empty());
    ASSERT_EQ(results.size(), 2); // Should find both test books

    LibrarySystem::destroyInstance();
}

// 6. Transaction Tests
TEST(TransactionTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Create borrow transaction
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T1", "U001", "B001", "borrow", now);
    library->createTransaction(borrowTrans);

    // Create return transaction
    Transaction* returnTrans = new Transaction("T2", "U001", "B001", "return", now + (15 * 24 * 60 * 60));
    returnTrans->setReturnDate(now + (15 * 24 * 60 * 60));
    returnTrans->closeTransaction();
    library->createTransaction(returnTrans);

    // Verify transactions
    std::vector<Transaction*> userTrans = library->getUserTransactions("U001");
    ASSERT_EQ(userTrans.size(), 2);

    LibrarySystem::destroyInstance();
}

// 7. Fine Management Tests
TEST(FineManagementTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    RegisteredUser* user = dynamic_cast<RegisteredUser*>(library->findUser("user@test.com"));
    ASSERT_TRUE(user != nullptr);

    // Set and verify fine
    user->setOutstandingFine(10.50);
    ASSERT_EQ(user->getOutstandingFine(), 10.50);

    // Test partial payment
    user->setOutstandingFine(user->getOutstandingFine() - 5.25);
    ASSERT_EQ(user->getOutstandingFine(), 5.25);

    // Test complete payment
    user->setOutstandingFine(0.0);
    ASSERT_EQ(user->getOutstandingFine(), 0.0);

    LibrarySystem::destroyInstance();
}

// 8. Edge Cases and Error Handling
TEST(EdgeCasesTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    setupTestData(library);

    // Test with very long strings
    std::string longString(1000, 'a');
    Book* book = new Book("LONG1", longString, longString, longString, longString);
    library->addBook(book);

    // Test with special characters
    Book* specialBook = new Book("SPEC1", "Book!@#$%", "Author&*()", "Cat^&*", "123!@#");
    library->addBook(specialBook);

    // Test with empty strings
    Book* emptyBook = new Book("", "", "", "", "");
    library->addBook(emptyBook);

    LibrarySystem::destroyInstance();
}

int main() {
    TestRunner runner;
    
    // Add all tests
    runner.addTest("AuthenticationTest", AuthenticationTest);
    runner.addTest("AdminMenuTest", AdminMenuTest);
    runner.addTest("LibrarianMenuTest", LibrarianMenuTest);
    runner.addTest("RegisteredUserMenuTest", RegisteredUserMenuTest);
    runner.addTest("BookManagementTest", BookManagementTest);
    runner.addTest("TransactionTest", TransactionTest);
    runner.addTest("FineManagementTest", FineManagementTest);
    runner.addTest("EdgeCasesTest", EdgeCasesTest);
    
    // Run all tests
    runner.runAll();
    
    return 0;
} 