#pragma once
using namespace std;
#include <bits/stdc++.h>

class Book {
private:
    string bookID;
    string title;
    string author;
    string category;
    string isbn;
    bool isAvailable;

public:
    Book(const string& id, const string& t, const string& a,
         const string& c, const string& i);
    
    // Getters
    string getBookID() const;
    string getTitle() const;
    string getAuthor() const;
    string getCategory() const;
    string getISBN() const;
    bool getAvailability() const;
    
    // Setters
    void setTitle(const string& t);
    void setAuthor(const string& a);
    void setCategory(const string& c);
    void setAvailability(bool available);
    
    // Display
    void displayDetails() const;
}; 