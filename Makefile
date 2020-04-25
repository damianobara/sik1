CC = gcc -Wall

all: testhttp_raw

testhttp_raw: testhttp_raw.c tcp.o http.o cookies.o err.o
	$(CC) tcp.o http.o cookies.o testhttp_raw.c -o main

tcp.o: tcp.c err.o
	$(CC) -c tcp.c err.h

http.o: http.c err.o
	$(CC) -c http.c err.h

cookies.o: cookies.c err.o
	$(CC) -c cookies.c err.h

errors.o: errors.c. errors.h
	$(CC) -c errors.c

clean:
	rm -f *.o
