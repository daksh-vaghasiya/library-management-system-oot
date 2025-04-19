# Library Management System

A comprehensive C++ library management system that allows librarians and users to manage books, transactions, and fines efficiently.

## Features

- User authentication and role-based access control
- Book management (add, update, remove, search)
- Transaction handling (borrow, return)
- Fine calculation and payment processing
- Search functionality for books and users
- Data persistence using file storage

## Prerequisites

- C++17 compatible compiler
- Make (optional, for build automation)

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/library-management-system.git
cd library-management-system
```

2. Compile the project:
```bash
g++ -std=c++17 *.cpp -o library_system
```

Or if you have Make installed:
```bash
make
```

## Running Tests

The project includes both white box and black box tests:

```bash
g++ -std=c++17 tests/*.cpp -o automated_tests
./automated_tests
```

## Usage

1. Start the application:
```bash
./library_system
```

2. Log in with your credentials
3. Use the menu-driven interface to perform various operations

## Project Structure

- `src/` - Source files
- `data/` - Data storage files
- `tests/` - Test files
- `include/` - Header files

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request 