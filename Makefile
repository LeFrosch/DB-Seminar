SDIR=./src
IDIR=./include
ODIR=./.obj

CC=gcc
CFLAGS=-I$(IDIR) -lm

HED=pointer.h
OBJ=main.o pointer.o

_HED=$(patsubst %,$(IDIR)/%,$(HED))
_OBJ=$(patsubst %,$(ODIR)/%,$(OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(_OBJ)
	$(CC) -o main $^ $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
	rm main

.PHONY: clean
