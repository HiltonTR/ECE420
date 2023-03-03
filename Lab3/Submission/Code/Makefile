all: main IO datagen serial

main: main.c Lab3IO.c
	gcc -w -o main main.c Lab3IO.c -fopenmp -lm -O3

IO: main.c Lab3IO.c
	gcc -o main main.c Lab3IO.c -fopenmp -lm

datagen: datagen.c
	gcc -o datagen datagen.c Lab3IO.c

serial:
	gcc -o serialtester serialtester.c Lab3IO.c -lm

clean:
	rm datagen main data_input data_output serialtester *_nt_*
