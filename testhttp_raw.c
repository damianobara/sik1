/*
 Program uruchamiamy z dwoma parametrami: nazwa serwera i numer jego portu.
 Program spróbuje połączyć się z serwerem, po czym będzie od nas pobierał
 linie tekstu i wysyłał je do serwera.  Wpisanie BYE kończy pracę.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "err.h"

#define BUFFER_SIZE      1024

static const char bye_string[] = "BYE";

int main (int argc, char *argv[]) {
    int rc;
    int sock;
    struct addrinfo addr_hints, *addr_result;
    char line[BUFFER_SIZE];

    /* Kontrola dokumentów ... */
    if (argc != 3) {
        fatal("Usage: %s host port", argv[0]);
    }

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        syserr("socket");
    }

    /* Trzeba się dowiedzieć o adres internetowy serwera. */
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_flags = 0;
    addr_hints.ai_family = AF_INET;
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;

    rc =  getaddrinfo(argv[1], argv[2], &addr_hints, &addr_result);
    if (rc != 0) {
        fprintf(stderr, "rc=%d\n", rc);
        syserr("getaddrinfo: %s", gai_strerror(rc));
    }

    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
        syserr("connect");
    }
    freeaddrinfo(addr_result);

    do {
        printf("line:");
        fgets(line, sizeof line, stdin);
        if (write(sock, line, strlen (line)) < 0)
            syserr("writing on stream socket");
    }
    while (strncmp(line, bye_string, sizeof bye_string - 1));
    if (close(sock) < 0)
        syserr("closing stream socket");

    return 0;
}

