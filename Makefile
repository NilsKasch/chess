CC=gcc
CFLAGS=
LDFLAGS=
EXEC=chess
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean, all, clean-all

clean:
	@rm -rf *.o *.mod *.smod $(EXEC)