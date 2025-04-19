#include <gtest/gtest.h>
#include "../LibrarySystem.h"
#include <thread>
#include <chrono>

// Test Suite for Library Management System
class LibraryBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    
    void SetUp() override {
        library = LibrarySystem::getInstance();
    }
    
    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

// Authentication Tests
TEST_F(LibraryBlackBoxTest, ValidUserLogin) {
    EXPECT_TRUE(library->login("admin@library.com", "admin123"));
    EXPECT_TRUE(library->login("librarian@library.com", "lib123"));
    EXPECT_TRUE(library->login("alice@email.com", "pass123"));
}

TEST_F(LibraryBlackBoxTest, InvalidUserLogin) {
    EXPECT_FALSE(library->login("nonexistent@email.com", "password"));
    EXPECT_FALSE(library->login("admin@library.com", "wrongpass"));
    EXPECT_FALSE(library->login("", ""));
}

// Book Management Flow Tests
TEST_F(LibraryBlackBoxTest, BookBorrowingFlow) {
    // Login as librarian
    ASSERT_TRUE(library->login("librarian@library.com", "lib123"));
    
    // Find an available book
    vector<Book*> books = library->getAllBooks();
    Book* availableBook = nullptr;
    for (Book* book : books) {
        if (book->getAvailability()) {
            availableBook = book;
            break;
        }
    }
    ASSERT_NE(availableBook, nullptr);
    
    // Issue book to user
    string bookID = availableBook->getBookID();
    string userID = "U001";  // Alice's ID
    
    // Create borrow transaction
    time_t now = time(nullptr);
    Transaction* transaction = new Transaction("T_TEST", userID, bookID, "borrow", now);
    library->createTransaction(transaction);
    
    // Verify book is no longer available
    EXPECT_FALSE(availableBook->getAvailability());
    
    // Return book
    transaction = new Transaction("T_TEST_RETURN", userID, bookID, "return", now + (15 * 24 * 60 * 60));
    library->createTransaction(transaction);
    
    // Verify book is available again
    EXPECT_TRUE(availableBook->getAvailability());
}

// Fine Management Flow Tests
TEST_F(LibraryBlackBoxTest, FineCalculationFlow) {
    // Create an overdue transaction
    time_t now = time(nullptr);
    time_t borrowDate = now - (20 * 24 * 60 * 60);  // 20 days ago
    Transaction* transaction = new Transaction("T_FINE_TEST", "U001", "B001", "borrow", borrowDate);
    
    // Set return date to now (6 days overdue)
    transaction->setReturnDate(now);
    transaction->closeTransaction();
    
    // Verify fine calculation
    EXPECT_EQ(transaction->getFineAmount(), 6.0);  // $1 per day
    delete transaction;
}

// Search Functionality Tests
TEST_F(LibraryBlackBoxTest, BookSearchFunctionality) {
    // Search by title
    vector<Book*> results = library->searchBooks("Harry Potter");
    EXPECT_FALSE(results.empty());
    
    // Search by author
    results = library->searchBooks("Tolkien");
    EXPECT_FALSE(results.empty());
    
    // Search by category
    results = library->searchBooks("Science Fiction");
    EXPECT_FALSE(results.empty());
    
    // Search with no results
    results = library->searchBooks("NonexistentBook123");
    EXPECT_TRUE(results.empty());
}

// Boundary Tests
TEST_F(LibraryBlackBoxTest, BorrowingLimitBoundary) {
    // Login as a registered user
    ASSERT_TRUE(library->login("alice@email.com", "pass123"));
    User* user = library->getCurrentUser();
    RegisteredUser* regUser = dynamic_cast<RegisteredUser*>(user);
    ASSERT_NE(regUser, nullptr);
    
    // Try to borrow more than 5 books
    vector<Book*> books = library->getAllBooks();
    int count = 0;
    for (Book* book : books) {
        if (book->getAvailability() && count < 6) {
            regUser->addBorrowedBook(book->getBookID());
            count++;
        }
    }
    
    // Verify borrowing limit
    EXPECT_LE(regUser->getBorrowedBooks().size(), 5);
}

// Performance Tests
TEST_F(LibraryBlackBoxTest, SearchPerformance) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform 100 searches
    for (int i = 0; i < 100; i++) {
        library->searchBooks("Fiction");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Search should complete within 1 second
    EXPECT_LT(duration.count(), 1000);
}

// Data Consistency Tests
TEST_F(LibraryBlackBoxTest, DataConsistencyAfterOperations) {
    size_t initialBookCount = library->getAllBooks().size();
    size_t initialUserCount = library->getAllUsers().size();
    
    // Perform various operations
    library->login("admin@library.com", "admin123");
    Book* newBook = new Book("TEST_BOOK", "Test Title", "Test Author", "Test Category", "123-TEST");
    library->addBook(newBook);
    
    // Verify counts
    EXPECT_EQ(library->getAllBooks().size(), initialBookCount + 1);
    EXPECT_EQ(library->getAllUsers().size(), initialUserCount);
    
    // Clean up
    library->removeBook("TEST_BOOK");
    EXPECT_EQ(library->getAllBooks().size(), initialBookCount);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 