CC = tcc

TARGET = ntar
SRC = ntar.c microtar.c os.c

$(TARGET): $(SRC) Makefile
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm $(TARGET)
