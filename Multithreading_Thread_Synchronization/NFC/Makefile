# Makefile for Publisher-Subscriber Notification Mechanism Demo
# This Makefile builds the integrated route table notification system

# Compiler and flags
DEBUG_FLAG =
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2 $(DEBUG_FLAG)
LDFLAGS = -pthread

# Directories
DATA_DIR = data
GLTHREAD_DIR = gtheard
BUILD_DIR = build

# Include directories
INCLUDES = -I$(DATA_DIR) -I$(GLTHREAD_DIR)

# Source files
GLTHREAD_SRCS = $(GLTHREAD_DIR)/glthread.c
ROUTE_TABLE_SRCS = $(DATA_DIR)/route_table.c
SUBSCRIBER_SRCS = $(DATA_DIR)/route_table_subscriber.c
MAIN_DEMO_SRCS = $(DATA_DIR)/main_demo.c

# Object files
GLTHREAD_OBJS = $(BUILD_DIR)/glthread.o
ROUTE_TABLE_OBJS = $(BUILD_DIR)/route_table.o
SUBSCRIBER_OBJS = $(BUILD_DIR)/route_table_subscriber.o
MAIN_DEMO_OBJS = $(BUILD_DIR)/main_demo.o

# All object files
ALL_OBJS = $(GLTHREAD_OBJS) $(ROUTE_TABLE_OBJS) $(SUBSCRIBER_OBJS) $(MAIN_DEMO_OBJS)

# Target executable
MAIN_DEMO = $(BUILD_DIR)/main_demo

# Default target
all: $(BUILD_DIR) $(MAIN_DEMO)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Main demo executable (integrated publisher-subscriber)
$(MAIN_DEMO): $(ALL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Object file rules
$(BUILD_DIR)/glthread.o: $(GLTHREAD_SRCS) $(GLTHREAD_DIR)/glthread.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/route_table.o: $(ROUTE_TABLE_SRCS) $(DATA_DIR)/route_table.h $(GLTHREAD_DIR)/glthread.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/route_table_subscriber.o: $(SUBSCRIBER_SRCS) $(DATA_DIR)/route_table_subscriber.h $(DATA_DIR)/route_table.h $(GLTHREAD_DIR)/glthread.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main_demo.o: $(MAIN_DEMO_SRCS) $(DATA_DIR)/route_table.h $(DATA_DIR)/route_table_subscriber.h $(GLTHREAD_DIR)/glthread.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean target
clean:
	rm -rf $(BUILD_DIR)

# Rebuild target
rebuild: clean all

# Run target
run: $(MAIN_DEMO)
	./$(MAIN_DEMO)

# Install target (optional)
install: all
	mkdir -p /usr/local/bin
	cp $(MAIN_DEMO) /usr/local/bin/route_table_demo

# Debug target for development
debug: CFLAGS += -DDEBUG -g3
debug: all

# Release target for optimized build
release: CFLAGS += -O3 -DNDEBUG
release: all

# Phony targets
.PHONY: all clean rebuild run install help debug release main_demo

# Add alias for main_demo
main_demo: $(MAIN_DEMO)
