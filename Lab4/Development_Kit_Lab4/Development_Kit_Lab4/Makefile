CC = gcc
CFLAGS = -Wall -std=gnu99

all: main

#------------------------------------------------------------------------------
main: main.c Lab4_IO.o timer.h
	mpicc main.c Lab4_IO.c -o main -lm

serialtester: serialtester.c Lab4_IO.c timer.h
	gcc serialtester.c Lab4_IO.c -o serialtester -lm 

datatrim: datatrim.c
	gcc datatrim.c -o datatrim

everything: main serialtester datatrim

# -----------------------------------------------------------------------------
clean:
	rm -f main datatrim serialtester data_input_link data_input_meta data_output
