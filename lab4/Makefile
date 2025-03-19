# Compiler
CC = gcc
CFLAGS = -Wall

# Executable name
TARGET = lab4

# Source file
SRC = lab4.c

# Default rule to compile the program
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Run test script
test: $(TARGET)
	./test.sh

# Memory check using Valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) MemAvailable ': ' /proc/meminfo

# Clean up compiled files
clean:
	rm -f $(TARGET) output.txt
