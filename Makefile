.DEFAULT_GOAL := all
.PHONY: clean

SDIR=./src
IDIR=./include
ODIR=./.obj

CC=gcc
CFLAGS=-I$(IDIR) -lm -lpthread -g -O3
WFLAGS= -Wall -Wextra -Wpedantic
DEFINES=

HED=pointer.h node.h los.h reservoir.h args.h
OBJ=node.o pointer.o los.o reservoir.o args.o

_HED=$(patsubst %,$(IDIR)/%,$(HED))
_OBJ=$(patsubst %,$(ODIR)/%,$(OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS) $(WFLAGS) $(DEFINES)

$(ODIR)/%.no_sample.o: $(SDIR)/%.c $(_HED)
	$(CC) -c -o $@ $< $(CFLAGS) $(WFLAGS) $(DEFINES) -D NO_SAMPLE

sample: $(_OBJ) $(ODIR)/main.o
	$(CC) -o main $(_OBJ) $(ODIR)/main.o $(CFLAGS)

no_sample: $(_OBJ:.o=.no_sample.o) $(ODIR)/main.no_sample.o
	$(CC) -o main_no_sample $(_OBJ:.o=.no_sample.o) $(ODIR)/main.no_sample.o $(CFLAGS)

all: sample no_sample

clean:
	rm -f $(ODIR)/*.o
	rm main
	rm main_no_sample