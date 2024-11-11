# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -I./src/common -I./src/server/include -I./src/client/include -I/usr/include
LDFLAGS = -lsqlite3

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SERVERDIR = $(SRCDIR)/server

# Files
SERVER_SRC = $(wildcard $(SERVERDIR)/*.c $(SERVERDIR)/database/*.c $(SERVERDIR)/network/*.c $(SERVERDIR)/logger/*.c)
COMMON_SRC = $(wildcard $(SRCDIR)/common/*.c)
SERVER_OBJ = $(SERVER_SRC:%.c=%.o)
COMMON_OBJ = $(COMMON_SRC:%.c=%.o)
SERVER_EXE = $(BINDIR)/server

# Default target
all: $(SERVER_EXE)

# Build server
$(SERVER_EXE): $(SERVER_OBJ) $(COMMON_OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ) $(COMMON_OBJ) $(LDFLAGS)
	@echo "Server build complete!"

# Compile all .c files to .o files
%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BINDIR)/*.o $(BINDIR)/* $(OBJDIR)/*.o
	@echo "Cleanup complete!"
