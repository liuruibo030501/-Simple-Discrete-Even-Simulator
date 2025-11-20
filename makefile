# Compiler
CC = gcc

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Output executable
TARGET = $(BUILD_DIR)/sdes

# Find all .c files under src/
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Convert each .c file to corresponding .o file inside build/
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler flags
CFLAGS = -Wall -Wextra -I$(INC_DIR)

# Default rule
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $(TARGET)

# Compile each .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
