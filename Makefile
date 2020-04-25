CC = gcc -Wall

all: testhttp_raw

testhttp_raw: testhttp_raw.c tcp.o http.o cookies.o err.o
	$(CC) tcp.o http.o cookies.o testhttp_raw.c -o testhttp_raw

tcp.o: tcp.c err.o
	$(CC) -c tcp.c err.h

http.o: http.c err.o
	$(CC) -c http.c err.h

cookies.o: cookies.c err.o
	$(CC) -c cookies.c err.h

err.o: err.c err.h
	$(CC) -c err.c

clean:
	rm -f *.o
