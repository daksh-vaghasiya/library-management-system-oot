#include <gtest/gtest.h>
#include "../LibrarySystem.h"
#include "../Book.h"
#include "../Transaction.h"
#include "../RegisteredUser.h"
#include <ctime>

class LibrarySystemTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    
    void SetUp() override {
        library = LibrarySystem::getInstance();
    }
    
    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

// User Management Tests
TEST_F(LibrarySystemTest, UserLoginTest) {
    EXPECT_TRUE(library->login("admin@library.com", "admin123"));
    EXPECT_FALSE(library->login("wrong@email.com", "wrongpass"));
    EXPECT_FALSE(library->login("admin@library.com", "wrongpass"));
}

TEST_F(LibrarySystemTest, UserRoleTest) {
    library->login("admin@library.com", "admin123");
    EXPECT_EQ(library->getCurrentUser()->getRole(), "admin");
    
    library->login("librarian@library.com", "lib123");
    EXPECT_EQ(library->getCurrentUser()->getRole(), "librarian");
    
    library->login("alice@email.com", "pass123");
    EXPECT_EQ(library->getCurrentUser()->getRole(), "registered");
}

// Book Management Tests
TEST_F(LibrarySystemTest, BookAvailabilityTest) {
    Book* book = new Book("TEST1", "Test Book", "Test Author", "Test Category", "123-456");
    library->addBook(book);
    
    EXPECT_TRUE(book->getAvailability());
    book->setAvailability(false);
    EXPECT_FALSE(book->getAvailability());
}

TEST_F(LibrarySystemTest, BookSearchTest) {
    vector<Book*> results = library->searchBooks("Harry Potter");
    EXPECT_FALSE(results.empty());
    EXPECT_EQ(results[0]->getAuthor(), "J.K. Rowling");
}

// Transaction Tests
TEST_F(LibrarySystemTest, TransactionFineCalculationTest) {
    time_t now = time(nullptr);
    Transaction* transaction = new Transaction("T1", "U1", "B1", "borrow", now);
    
    // Set return date 3 days after due date
    time_t returnDate = transaction->getDueDate() + (3 * 24 * 60 * 60);
    transaction->setReturnDate(returnDate);
    transaction->calculateFine();
    
    EXPECT_EQ(transaction->getFineAmount(), 3.0);
    delete transaction;
}

TEST_F(LibrarySystemTest, TransactionOverdueTest) {
    time_t now = time(nullptr);
    Transaction* transaction = new Transaction("T1", "U1", "B1", "borrow", now);
    
    // Set current time to after due date
    EXPECT_TRUE(transaction->isOverdue());
    delete transaction;
}

// Registered User Tests
TEST_F(LibrarySystemTest, UserBorrowLimitTest) {
    RegisteredUser* user = new RegisteredUser("U1", "Test User", "test@email.com", "pass");
    
    // Add 5 books
    for (int i = 1; i <= 5; i++) {
        user->addBorrowedBook("B" + to_string(i));
    }
    
    EXPECT_EQ(user->getBorrowedBooks().size(), 5);
    delete user;
}

TEST_F(LibrarySystemTest, UserFineManagementTest) {
    RegisteredUser* user = new RegisteredUser("U1", "Test User", "test@email.com", "pass");
    
    user->setOutstandingFine(10.50);
    EXPECT_EQ(user->getOutstandingFine(), 10.50);
    
    user->setOutstandingFine(5.25);
    EXPECT_EQ(user->getOutstandingFine(), 5.25);
    delete user;
}

// Data Persistence Tests
TEST_F(LibrarySystemTest, DataPersistenceTest) {
    // Save current state
    library->saveData();
    
    // Modify some data
    Book* book = new Book("TEST2", "Test Book 2", "Test Author 2", "Test Category", "123-457");
    library->addBook(book);
    
    // Load data and verify
    library->loadData();
    Book* loadedBook = library->findBook("TEST2");
    EXPECT_NE(loadedBook, nullptr);
    EXPECT_EQ(loadedBook->getTitle(), "Test Book 2");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 