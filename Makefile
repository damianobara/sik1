CC = gcc -Wall -Wextra

all: testhttp_raw

testhttp_raw: testhttp_raw.c err.o cookies.o tcp.o http.o
	$(CC) err.o cookies.o tcp.o http.o testhttp_raw.c -o testhttp_raw

cookies.o: cookies.c cookies.h
	$(CC) -c cookies.c

tcp.o: tcp.c tcp.h
	$(CC) -c tcp.c

http.o: http.c http.h
	$(CC) -c http.c


err.o: err.c err.h 
	$(CC) -c err.c