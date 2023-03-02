all: main IO datagen serial

main: main.c Lab3IO.c
	gcc -w -o main main.c Lab3IO.c -fopenmp -lm -O3

IO: main.c Lab3IO.c
	gcc -o main main.c Lab3IO.c -fopenmp -lm

static: main_static.c Lab3IO.c
	gcc -w -o main_static main_static.c Lab3IO.c -fopenmp -lm -O3

static: main_static.c Lab3IO.c
	gcc -o main_static main_static.c Lab3IO.c -fopenmp -lm	

dynamic: main_static.c Lab3IO.c
	gcc -w -o main_dynamic main_dynamic.c Lab3IO.c -fopenmp -lm -O3

dynamic: main_static.c Lab3IO.c
	gcc -o main_dynamic main_dynamic.c Lab3IO.c -fopenmp -lm	

runtime: main_static.c Lab3IO.c
	gcc -w -o main_runtime main_runtime.c Lab3IO.c -fopenmp -lm -O3

runtime: main_static.c Lab3IO.c
	gcc -o main_runtime main_runtime.c Lab3IO.c -fopenmp -lm	

guided: main_static.c Lab3IO.c
	gcc -w -o main_guided main_guided.c Lab3IO.c -fopenmp -lm -O3

guided: main_static.c Lab3IO.c
	gcc -o main_guided main_guided.c Lab3IO.c -fopenmp -lm	

steph: steph.c Lab3IO.c timer.h
	gcc -w -o steph steph.c Lab3IO.c -fopenmp -lm

datagen: datagen.c
	gcc -o datagen datagen.c Lab3IO.c

steph: steph.c Lab3IO.c
	gcc -w -o steph steph.c Lab3IO.c -fopenmp -lm

serial:
	gcc -o serialtester serialtester.c Lab3IO.c -lm

clean:
	rm datagen main main_static main_dynamic data_input data_output serialtester
