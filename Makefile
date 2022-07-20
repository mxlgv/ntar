CC = gcc

TARGET = ntar
SRC = ntar.c microtar.c

CFLAGS = -Os -Wl,-strip-all

$(TARGET): $(SRC) Makefile
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm $(TARGET)