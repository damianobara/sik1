CC = gcc -Wall

all: testhttp_raw

testhttp_raw: testhttp_raw.c tcp.h http.h cookies.h err.h
	$(CC) testhttp_raw.c -o testhttp_raw

tcp.o: tcp.c err.h
	$(CC) -c tcp.c

http.o: http.c err.h
	$(CC) -c http.c

cookies.o: cookies.c err.h
	$(CC) -c cookies.c

err.o: err.c err.h
	$(CC) -c err.c

clean:
	rm -f *.o
