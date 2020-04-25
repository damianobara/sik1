CC = gcc -Wall
TARGETS = testhttp_raw

all: $(TARGETS) 

err.o: err.h

tcp.o: tcp.h err.h

http.o: http.h err.h

cookies: cookies.h err.h

testhttp_raw.o: testhttp_raw.c tcp.h http.h  err.h

testhttp_raw: testhttp_raw.o tcp.o http.o cookies.o err.o
