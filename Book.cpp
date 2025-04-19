using namespace std;
#include <bits/stdc++.h>
#include "Book.h"

Book::Book(const string& id, const string& t, const string& a,
           const string& c, const string& i)
    : bookID(id), title(t), author(a), category(c), isbn(i), isAvailable(true) {}

// Getters
string Book::getBookID() const { return bookID; }
string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getCategory() const { return category; }
string Book::getISBN() const { return isbn; }
bool Book::getAvailability() const { return isAvailable; }

// Setters
void Book::setTitle(const string& t) { title = t; }
void Book::setAuthor(const string& a) { author = a; }
void Book::setCategory(const string& c) { category = c; }
void Book::setAvailability(bool available) { isAvailable = available; }

// Display
void Book::displayDetails() const {
    cout << "\nBook Details:\n"
         << "ID: " << bookID << "\n"
         << "Title: " << title << "\n"
         << "Author: " << author << "\n"
         << "Category: " << category << "\n"
         << "ISBN: " << isbn << "\n"
         << "Status: " << (isAvailable ? "Available" : "Borrowed") << "\n";
} 