TARGET = rsa

SOURCES = main.c

CC = gcc

CFLAGS = -g -Wall -Wextra -std=c99 -O3

LDFLAGS = -lgmp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
