#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "../LibrarySystem.h"
#include "../Book.h"
#include "../Transaction.h"
#include "../RegisteredUser.h"

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

// User Management Tests
TEST(UserLoginTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    ASSERT_TRUE(library->login("admin@library.com", "admin123"));
    ASSERT_FALSE(library->login("wrong@email.com", "wrongpass"));
    ASSERT_FALSE(library->login("admin@library.com", "wrongpass"));
    LibrarySystem::destroyInstance();
}

TEST(UserRoleTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    library->login("admin@library.com", "admin123");
    ASSERT_EQ(library->getCurrentUser()->getRole(), "admin");
    
    library->login("librarian@library.com", "lib123");
    ASSERT_EQ(library->getCurrentUser()->getRole(), "librarian");
    
    library->login("alice@email.com", "pass123");
    ASSERT_EQ(library->getCurrentUser()->getRole(), "registered");
    LibrarySystem::destroyInstance();
}

// Book Management Tests
TEST(BookAvailabilityTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    Book* book = new Book("TEST1", "Test Book", "Test Author", "Test Category", "123-456");
    library->addBook(book);
    
    ASSERT_TRUE(book->getAvailability());
    book->setAvailability(false);
    ASSERT_FALSE(book->getAvailability());
    LibrarySystem::destroyInstance();
}

TEST(BookSearchTest) {
    LibrarySystem* library = LibrarySystem::getInstance();
    std::vector<Book*> results = library->searchBooks("Harry Potter");
    ASSERT_FALSE(results.empty());
    ASSERT_EQ(results[0]->getAuthor(), "J.K. Rowling");
    LibrarySystem::destroyInstance();
}

// Transaction Tests
TEST(TransactionFineCalculationTest) {
    time_t now = time(nullptr);
    Transaction* transaction = new Transaction("T1", "U1", "B1", "return", now - (17 * 24 * 60 * 60));  // Created 17 days ago
    
    // Due date was 3 days ago (14 days from creation)
    transaction->setReturnDate(now);  // Returning now
    transaction->closeTransaction();  // This will calculate the fine
    
    double fine = transaction->getFineAmount();
    std::cout << "\nDebug: Fine amount = " << fine << ", Expected = 3.0\n";
    ASSERT_EQ(fine, 3.0);
    delete transaction;
}

TEST(TransactionOverdueTest) {
    time_t now = time(nullptr);
    Transaction* transaction = new Transaction("T1", "U1", "B1", "borrow", now - (15 * 24 * 60 * 60));
    ASSERT_TRUE(transaction->isOverdue());
    delete transaction;
}

// Registered User Tests
TEST(UserBorrowLimitTest) {
    RegisteredUser* user = new RegisteredUser("U1", "Test User", "test@email.com", "pass");
    
    // Add 5 books
    for (int i = 1; i <= 5; i++) {
        user->addBorrowedBook("B" + std::to_string(i));
    }
    
    ASSERT_EQ(user->getBorrowedBooks().size(), 5);
    delete user;
}

TEST(UserFineManagementTest) {
    RegisteredUser* user = new RegisteredUser("U1", "Test User", "test@email.com", "pass");
    
    user->setOutstandingFine(10.50);
    ASSERT_EQ(user->getOutstandingFine(), 10.50);
    
    user->setOutstandingFine(5.25);
    ASSERT_EQ(user->getOutstandingFine(), 5.25);
    delete user;
}

int main() {
    TestRunner runner;
    
    // Add all tests
    runner.addTest("UserLoginTest", UserLoginTest);
    runner.addTest("UserRoleTest", UserRoleTest);
    runner.addTest("BookAvailabilityTest", BookAvailabilityTest);
    runner.addTest("BookSearchTest", BookSearchTest);
    runner.addTest("TransactionFineCalculationTest", TransactionFineCalculationTest);
    runner.addTest("TransactionOverdueTest", TransactionOverdueTest);
    runner.addTest("UserBorrowLimitTest", UserBorrowLimitTest);
    runner.addTest("UserFineManagementTest", UserFineManagementTest);
    
    // Run all tests
    runner.runAll();
    
    return 0;
} 