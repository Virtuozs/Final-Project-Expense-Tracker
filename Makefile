# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I./include
LDFLAGS = -lsqlite3

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files and output binary
SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/database.cpp $(SRC_DIR)/report.cpp $(SRC_DIR)/budget.cpp $(SRC_DIR)/utilities.cpp $(SRC_DIR)/expense.cpp
OUTPUT = $(BUILD_DIR)/expense_tracker.exe

# Default target
all: $(OUTPUT)

# Build target
$(OUTPUT): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Clean up
clean:
	rm -f $(OUTPUT)

.PHONY: all clean
