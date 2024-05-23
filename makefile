# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -g

# Target Executable
TARGET = out

# Source Directory
SRC_DIR = src

# Include Directory
INC_DIR = include

# Object Directory
OBJ_DIR = obj

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default rule to build the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean up object files and the target executable
.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
	rmdir obj