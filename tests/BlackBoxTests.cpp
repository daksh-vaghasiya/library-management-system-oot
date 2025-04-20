BlackBoxTests.cpp :


#include <gtest/gtest.h>
#include "../LibrarySystem.h"
#include "../Admin.h"
#include "../Book.h"
#include "../RegisteredUser.h"
#include "../Librarian.h"
#include "../Transaction.h"
#include <algorithm>

//
class AdminBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    Admin* admin;

    void SetUp() override {
        library = LibrarySystem::getInstance();
        // Add test admin
        library->addUser(new Admin("A101", "Admin One", "admin1@test.com", "admin123"));
        library->login("admin1@test.com", "admin123");  
    
        admin = dynamic_cast<Admin*>(library->getCurrentUser());
    }

    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

// Test Admin Authentication
TEST_F(AdminBlackBoxTest, AdminAuthentication) {
    EXPECT_TRUE(admin != nullptr);
    EXPECT_EQ(admin->getRole(), "admin");
    EXPECT_EQ(admin->getName(), "Admin One");
}

// Test Admin Book Management
TEST_F(AdminBlackBoxTest, AdminBookManagement) {
    // Test adding a book
    bool bookAdded = admin->addBookDirect(
        "B101",
        "Programming Basics",
        "John Smith",
        "Computer Science",
        "111-222-333"
    );
    EXPECT_TRUE(bookAdded);

    // Verify book was added
    Book* book = library->findBook("B101");
    EXPECT_TRUE(book != nullptr);
    EXPECT_EQ(book->getTitle(), "Programming Basics");
    EXPECT_EQ(book->getAuthor(), "JOhn Smith");
    
    // Test duplicate book
    bool duplicateBook = admin->addBookDirect(
        "B101",
        "Duplicate Book",
        "Another Author",
        "Non-Fiction",
        "789-012"
    );
    EXPECT_FALSE(duplicateBook);
}

// Test Admin User Management
TEST_F(AdminBlackBoxTest, AdminUserManagement) {
    // Test adding a new user
    bool userAdded = admin->addUserDirect(
        "U101",
        "User One",
        "user1@test.com",
        "pass123",
        "registered"
    );
    EXPECT_TRUE(userAdded);

    // Verify user was added
    User* user = library->findUser("user1@test.com");
    EXPECT_TRUE(user != nullptr);
    EXPECT_EQ(user->getRole(), "registered");
    
    // Test adding duplicate user
    bool duplicateUser = admin->addUserDirect(
        "U101",
        "Duplicate User",
        "user1@test.com",
        "pass123",
        "registered"
    );
    EXPECT_FALSE(duplicateUser);

    // Test adding user with invalid role
    bool invalidRole = admin->addUserDirect(
        "U102",
        "Invalid Role User",
        "invalid@test.com",
        "pass123",
        "invalid_role"
    );
    EXPECT_FALSE(invalidRole);
}

// Test Admin Multiple Operations
TEST_F(AdminBlackBoxTest, AdminMultipleOperations) {
    // Add a librarian
    bool librarianAdded = admin->addUserDirect(
        "L101",
        "Librarian One",
        "lib1@test.com",
        "lib123",
        "librarian"
    );
    EXPECT_TRUE(librarianAdded);

    // Add a book
    bool bookAdded = admin->addBookDirect(
        "B201",
        "Advanced Book",
        "Professional Author",
        "Education",
        "345-678"
    );
    EXPECT_TRUE(bookAdded);

    // Verify both operations were successful
    User* librarian = library->findUser("lib1@test.com");
    Book* book = library->findBook("B201");
    
    EXPECT_TRUE(librarian != nullptr);
    EXPECT_TRUE(book != nullptr);
    EXPECT_EQ(librarian->getRole(), "librarian");
    EXPECT_EQ(book->getTitle(), "Advanced Book");
}

class LibrarianBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    Librarian* librarian;
    RegisteredUser* testUser;

    void SetUp() override {
        library = LibrarySystem::getInstance();
        
        // Add test librarian
        library->addUser(new Librarian("L123", "Test Librarian", "librarian123@test.com", "lib123"));
        library->login("librarian123@test.com", "lib123");
        librarian = dynamic_cast<Librarian*>(library->getCurrentUser());
        
        // Add test user
        library->addUser(new RegisteredUser("U123", "Test User", "user123@test.com", "user123"));
        testUser = dynamic_cast<RegisteredUser*>(library->findUser("user123@test.com"));
    }

    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

// Test Book Management
TEST_F(LibrarianBlackBoxTest, BookManagement) {
    // Test adding a book
    bool bookAdded = librarian->addBookDirect(
        "B456",
        "Test Book",
        "Test Author",
        "Fiction",
        "111-222"
    );
    EXPECT_TRUE(bookAdded);
    
    // Verify book was added
    Book* book = library->findBook("B456");
    EXPECT_TRUE(book != nullptr);
    EXPECT_EQ(book->getTitle(), "Test Book");
    
    // Test adding duplicate book
    bookAdded = librarian->addBookDirect(
        "B456",
        "Duplicate Book",
        "Another Author",
        "Non-Fiction",
        "333-444"
    );
    EXPECT_FALSE(bookAdded);
}

// Test Book Issue and Return
TEST_F(LibrarianBlackBoxTest, BookIssueAndReturn) {
    // Add a test book
    librarian->addBookDirect("B321", "Issue Test Book", "Author", "Fiction", "555-666");
    Book* book = library->findBook("B321");
    EXPECT_TRUE(book != nullptr);
    EXPECT_TRUE(book->getAvailability());
    
    // Test issuing book
    time_t now = time(nullptr);
    Transaction* issueTrans = new Transaction("T100", testUser->getUserID(), "B321", "borrow", now);
    library->createTransaction(issueTrans);
    book->setAvailability(false);
    testUser->addBorrowedBook("B321");
    
    EXPECT_FALSE(book->getAvailability());
    EXPECT_TRUE(std::find(testUser->getBorrowedBooks().begin(), 
                         testUser->getBorrowedBooks().end(), 
                         "B321") != testUser->getBorrowedBooks().end());
    
    // Test returning book
    time_t returnTime = now + (7 * 24 * 60 * 60); // 7 days later
    Transaction* returnTrans = new Transaction("T101", testUser->getUserID(), "B321", "return", returnTime);
    returnTrans->setReturnDate(returnTime);
    library->createTransaction(returnTrans);
    book->setAvailability(true);
    testUser->removeBorrowedBook("B321");
    
    EXPECT_TRUE(book->getAvailability());
    EXPECT_TRUE(std::find(testUser->getBorrowedBooks().begin(), 
                         testUser->getBorrowedBooks().end(), 
                         "B321") == testUser->getBorrowedBooks().end());
}

// Test Fine Management
TEST_F(LibrarianBlackBoxTest, FineManagement) {
    // Add test book and issue it
    librarian->addBookDirect("B213", "Fine Test Book", "Author", "Fiction", "777-888");
    Book* book = library->findBook("B213");
    
    // Issue book
    time_t issueTime = time(nullptr) - (20 * 24 * 60 * 60); // 20 days ago
    Transaction* issueTrans = new Transaction("T102", testUser->getUserID(), "B213", "borrow", issueTime);
    library->createTransaction(issueTrans);
    book->setAvailability(false);
    testUser->addBorrowedBook("B213");
    
    // Return book late (after 20 days)
    time_t returnTime = time(nullptr);
    Transaction* returnTrans = new Transaction("T103", testUser->getUserID(), "B213", "return", returnTime);
    returnTrans->setReturnDate(returnTime);
    returnTrans->closeTransaction();
    library->createTransaction(returnTrans);
    
    // Verify fine was applied (20 - 14 = 6 days late, $1 per day)
    EXPECT_GT(returnTrans->getFineAmount(), 0);
    EXPECT_EQ(returnTrans->getFineAmount(), 6.0);
}

// Library System Tests
class LibrarySystemBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;

    void SetUp() override {
        library = LibrarySystem::getInstance();
        library->addUser(new Admin("A1234", "Test Admin", "admin1234@test.com", "admin1234"));
    }

    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

TEST_F(LibrarySystemBlackBoxTest, UserAuthentication) {
    EXPECT_TRUE(library->login("admin1234@test.com", "admin1234"));
    EXPECT_FALSE(library->login("wrong@email.com", "wrongpass"));
    
    library->logout();
    EXPECT_EQ(library->getCurrentUser(), nullptr);
}

TEST_F(LibrarySystemBlackBoxTest, BookManagement) {
    // Test book addition and search
    Book* book = new Book("B1010", "Test Book100", "Test Author", "Fiction", "123-456");
    library->addBook(book);
    
    Book* foundBook = library->findBook("B1010");
    EXPECT_NE(foundBook, nullptr);
    EXPECT_EQ(foundBook->getTitle(), "Test Book100");
    
    // Test book removal
    library->removeBook("B1010");
    EXPECT_EQ(library->findBook("B1010"), nullptr);
}

TEST_F(LibrarySystemBlackBoxTest, UserManagement) {
    RegisteredUser* user = new RegisteredUser("U0010", "Test User0010", "user0010@test.com", "user123");
    library->addUser(user);
    
    User* foundUser = library->findUser("user0010@test.com");
    EXPECT_NE(foundUser, nullptr);
    EXPECT_EQ(foundUser->getName(), "Test User0010");
    
    library->removeUser("user0010@test.com");
    EXPECT_EQ(library->findUser("user0010@test.com"), nullptr);
}

// Transaction Tests
class TransactionBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    RegisteredUser* user;
    Book* book;

    void SetUp() override {
        library = LibrarySystem::getInstance();
        user = new RegisteredUser("U0020", "Transaction User", "trans@test.com", "pass123");
        library->addUser(user);
        book = new Book("B2010", "Transaction Book", "Author", "Fiction", "789-012");
        library->addBook(book);
    }

    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

TEST_F(TransactionBlackBoxTest, BorrowTransaction) {
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T10", user->getUserID(), book->getBookID(), "borrow", now);
    library->createTransaction(borrowTrans);
    
    vector<Transaction*> userTrans = library->getUserTransactions(user->getUserID());
    EXPECT_FALSE(userTrans.empty());
    EXPECT_EQ(userTrans[0]->getType(), "borrow");
}

TEST_F(TransactionBlackBoxTest, ReturnTransaction) {
    // First create borrow transaction
    time_t now = time(nullptr);
    Transaction* borrowTrans = new Transaction("T20", user->getUserID(), book->getBookID(), "borrow", now);
    library->createTransaction(borrowTrans);
    
    // Create return transaction
    time_t returnTime = now + (15 * 24 * 60 * 60); // 15 days later
    Transaction* returnTrans = new Transaction("T30", user->getUserID(), book->getBookID(), "return", returnTime);
    returnTrans->setReturnDate(returnTime);
    returnTrans->closeTransaction();
    library->createTransaction(returnTrans);
    
    // Verify fine calculation for overdue return
    EXPECT_EQ(returnTrans->getFineAmount(), 1.0); // 1 day overdue = $1 fine
}

TEST_F(TransactionBlackBoxTest, OverdueCheck) {
    time_t now = time(nullptr);
    Transaction* trans = new Transaction("T40", user->getUserID(), book->getBookID(), "borrow", now - (20 * 24 * 60 * 60));
    EXPECT_TRUE(trans->isOverdue());
    delete trans;
}

// RegisteredUser Tests
class RegisteredUserBlackBoxTest : public ::testing::Test {
protected:
    LibrarySystem* library;
    RegisteredUser* user;

    void SetUp() override {
        library = LibrarySystem::getInstance();
        user = new RegisteredUser("U0030", "Test User", "test.user0030@test.com", "pass123");
        library->addUser(user);
    }

    void TearDown() override {
        LibrarySystem::destroyInstance();
    }
};

TEST_F(RegisteredUserBlackBoxTest, BorrowLimit) {
    // Test borrowing limit (max 5 books)
    for (int i = 1; i <= 5; i++) {
        string bookId = "B" + to_string(i);
        user->addBorrowedBook(bookId);
    }
    EXPECT_EQ(user->getBorrowedBooks().size(), 5);
}

TEST_F(RegisteredUserBlackBoxTest, FineManagement) {
    // Test fine management
    double initialFine = 10.50;
    user->setOutstandingFine(initialFine);
    EXPECT_EQ(user->getOutstandingFine(), initialFine);
    
    // Test fine payment
    double payment = 5.25;
    user->setOutstandingFine(initialFine - payment);
    EXPECT_EQ(user->getOutstandingFine(), initialFine - payment);
}

TEST_F(RegisteredUserBlackBoxTest, BookOperations) {
    // Test book borrowing and returning
    string bookId = "TEST001";
    user->addBorrowedBook(bookId);
    
    auto borrowedBooks = user->getBorrowedBooks();
    EXPECT_TRUE(find(borrowedBooks.begin(), borrowedBooks.end(), bookId) != borrowedBooks.end());
    
    user->removeBorrowedBook(bookId);
    borrowedBooks = user->getBorrowedBooks();
    EXPECT_TRUE(find(borrowedBooks.begin(), borrowedBooks.end(), bookId) == borrowedBooks.end());
}

// User Tests
class UserBlackBoxTest : public ::testing::Test {
protected:
    User* user;

    void SetUp() override {
        user = new RegisteredUser("U0040", "Base User", "base.user0040@test.com", "pass123");
    }

    void TearDown() override {
        delete user;
    }
};

TEST_F(UserBlackBoxTest, Authentication) {
    // Test valid login
    EXPECT_TRUE(user->login("base.user0040@test.com", "pass123"));
    
    // Test invalid login
    EXPECT_FALSE(user->login("base.user0040@test.com", "wrongpass"));
    EXPECT_FALSE(user->login("wrong@email.com", "pass123"));
}

TEST_F(UserBlackBoxTest, UserInformation) {
    // Test user information getters
    EXPECT_EQ(user->getUserID(), "U0040");
    EXPECT_EQ(user->getName(), "Base User");
    EXPECT_EQ(user->getEmail(), "base.user0040@test.com");
    EXPECT_EQ(user->getRole(), "registered");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}