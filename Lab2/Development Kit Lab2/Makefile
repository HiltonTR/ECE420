CC = gcc
CFLAGS = -Wall

all: main1 main2 main3 main4 client

#------------------------------------------------------------------------------
main1: srv_shared.o common.o s1_worker.o 
	$(CC) $(CFLAGS) srv_shared.o common.o s1_worker.o -o main1 -lpthread

main2: srv_shared.o common.o s2_worker.o 
	$(CC) $(CFLAGS) srv_shared.o common.o s2_worker.o -o main2 -lpthread

main3: srv_shared.o common.o s3_worker.o 
	$(CC) $(CFLAGS) srv_shared.o common.o s3_worker.o -o main3 -lpthread

main4: srv_shared.o common.o s4_worker.o 
	$(CC) $(CFLAGS) srv_shared.o common.o s4_worker.o -o main4 -lpthread

client: client.c timer.h common.h common.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

# -----------------------------------------------------------------------------
common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o

srv_shared.o: srv_shared.c common.h
	$(CC) $(CFLAGS) -c srv_shared.c -o srv_shared.o -std=c99

s1_worker.o: s1_worker.c common.h
	$(CC) $(CFLAGS) -c s1_worker.c -o s1_worker.o -std=c99

s2_worker.o: s2_worker.c common.h
	$(CC) $(CFLAGS) -c s2_worker.c -o s2_worker.o -std=c99

s3_worker.o: s3_worker.c common.h
	$(CC) $(CFLAGS) -c s3_worker.c -o s3_worker.o -std=c99

s4_worker.o: s4_worker.c common.h
	$(CC) $(CFLAGS) -c s4_worker.c -o s4_worker.o -std=gnu99

# -----------------------------------------------------------------------------
clean: clean_obj clean_exe

clean_obj:
	rm -f common.o srv_shared.o s1_worker.o s2_worker.o s3_worker.o s4_worker.o

clean_exe:
	rm -f main1 main2 main3 main4
