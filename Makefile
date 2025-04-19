CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = library_system
TEST_TARGET = automated_tests

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TEST_SRCS = $(wildcard tests/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(filter-out main.o, $(OBJS))
	$(CXX) $^ -o $(TEST_TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)

# Create data directory if it doesn't exist
data:
	mkdir -p data 