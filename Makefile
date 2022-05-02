.DEFAULT_GOAL := all
.PHONY: clean

SDIR=./src
IDIR=./include
ODIR=./.obj

CC=gcc
CFLAGS=-I$(IDIR) -lm -lpthread -g -O3
WFLAGS = -Wall -Wextra -Wpedantic

HED=pointer.h node.h los.h reservoir.h
OBJ=node.o pointer.o los.o reservoir.o

_HED=$(patsubst %,$(IDIR)/%,$(HED))
_OBJ=$(patsubst %,$(ODIR)/%,$(OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS) $(WFLAGS)

$(ODIR)/main.o: $(SDIR)/main.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS) $(WFLAGS)

$(ODIR)/main_no_sample.o: $(SDIR)/main.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS) $(WFLAGS) -D NO_SAMPLE

all: $(_OBJ) $(ODIR)/main.o $(ODIR)/main_no_sample.o
	$(CC) -o main $(_OBJ) $(ODIR)/main.o $(CFLAGS)
	$(CC) -o main_no_sample $(_OBJ) $(ODIR)/main_no_sample.o $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
	rm main
	rm main_no_sample