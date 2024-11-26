TARGET = main

SOURCES = main.c

CC = gcc

CFLAGS = -Wall -Wextra -std=c11

LDFLAGS = -lgmp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
