CC = gcc
CFLAGS = -Wall

all: main1

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o

main1.o: srv_shared.c common.h
	$(CC) $(CFLAGS) -c srv_shared.c -o main1.o

s1_worker.o: s1_worker.c common.h
	$(CC) $(CFLAGS) -c s1_worker.c -o s1_worker.o

main1: main1.o s1_worker.o common.o
	$(CC) $(CFLAGS) main1.o s1_worker.o common.o -o main1 -lpthread

clean:
	rm -f main1.o s1_worker.o main1