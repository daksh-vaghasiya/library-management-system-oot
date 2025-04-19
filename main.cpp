#include <iostream>
#include "LibrarySystem.h"

int main() {
    try {
        LibrarySystem* library = LibrarySystem::getInstance();
        
        while (true) {
            library->displayMainMenu();
            int choice;
            std::cin >> choice;
            
            switch (choice) {
                case 1: {
                    std::string email, password;
                    std::cout << "Enter email: ";
                    std::cin >> email;
                    std::cout << "Enter password: ";
                    std::cin >> password;
                    
                    if (library->login(email, password)) {
                        User* user = library->getCurrentUser();
                        while (true) {
                            user->displayMenu();
                            int userChoice;
                            std::cin >> userChoice;
                            
                            // Check for logout based on user role
                            bool shouldLogout = false;
                            if (user->getRole() == "admin" && userChoice == 3) {
                                shouldLogout = true;
                            } else if (user->getRole() == "librarian" && userChoice == 5) {
                                shouldLogout = true;
                            } else if (user->getRole() == "registered" && userChoice == 13) {
                                shouldLogout = true;
                            }
                            
                            if (shouldLogout) {
                                library->logout();
                                break;
                            }
                            user->performAction(userChoice);
                        }
                    } else {
                        std::cout << "Login failed. Please try again.\n";
                    }
                    break;
                }
                case 2:
                    std::cout << "Thank you for using the Library Management System. Goodbye!\n";
                    LibrarySystem::destroyInstance();
                    return 0;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        LibrarySystem::destroyInstance();
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred.\n";
        LibrarySystem::destroyInstance();
        return 1;
    }
    
    LibrarySystem::destroyInstance();
    return 0;
} 