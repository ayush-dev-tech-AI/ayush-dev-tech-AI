CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
TARGET  = server

all: $(TARGET)

$(TARGET): server.c
	$(CC) $(CFLAGS) -o $(TARGET) server.c

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
