C=gcc -g
CFLAGS=-Wall -lm
LFLAGS=-pthread


main: main.c lab1_IO.h
	$(C) $(CFLAGS) $(LFLAGS) main.c lab1_IO.c -o main -std=c99

matrixgen:
	$(C) $(CFLAGS) matrixgen.c -o matrixgen

serialtester:
	$(C) $(CFLAGS) serialtester.c lab1_IO.c -o serialtester

clean: @rm -f *.0 main matrixgen serialtester