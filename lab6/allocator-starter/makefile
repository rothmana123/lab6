# Makefile for custom memory allocator

CC = cc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -shared
TARGET_SO = allocator.so
TARGET_TEST = allocator_test

all: $(TARGET_SO) $(TARGET_TEST)

# Build the shared object
$(TARGET_SO): allocator.c trace.h logger.h
	$(CC) $(CFLAGS) -o $(TARGET_SO) allocator.c $(LDFLAGS)

# Build the allocator test binary
$(TARGET_TEST): allocator-test.c
	$(CC) -o $(TARGET_TEST) allocator-test.c

# Clean build artifacts
clean:
	rm -f $(TARGET_SO) $(TARGET_TEST)

