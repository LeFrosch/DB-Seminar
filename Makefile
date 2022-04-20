SDIR=src
IDIR=include
ODIR=.obj

CC=gcc
CFLAGS=-I$(IDIR)

HED=
OBJ=main.o

_HED=$(patsubst %,$(IDIR)/%,$(HED))
_OBJ=$(patsubst %,$(ODIR)/%,$(OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
	rm main

.PHONY: clean
