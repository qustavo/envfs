CC=gcc
CFLAGS=-D_FILE_OFFSET_BITS=64 -Wall
LFLAGS=-lfuse
BIN=envfs

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): envfs.o
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -rf *.o $(BIN)
