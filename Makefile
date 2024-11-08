
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 gstreamer-1.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 gstreamer-1.0 sqlite3`

SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

all: server client

server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

client: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) server client

.PHONY: all clean