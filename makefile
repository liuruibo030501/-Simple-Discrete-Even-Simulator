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
CFLAGS = -Wall -Wextra -g -I$(INC_DIR)
# UPDATE YVETTA: Add -g to print debug information

# Default rule
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo Linking $(TARGET)
	$(CC) $(OBJS) -o $(TARGET) -lm
# UPDATE YVETTA: 1.Added -lm to explicitly link the math library (libm) 2. Added @echo for better build output

# Compile each .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@
#UPDATE YVETTA: Added @echo for better build output

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
