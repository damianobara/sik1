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
#define HOST_SIZE      1024
#define PORT_SIZE      32
#define MAX_REQUEST_LEN      1024

static const char bye_string[] = "BYE";

void set_addr_hints(struct addrinfo *addr_hints) {
    memset(addr_hints, 0, sizeof(struct addrinfo));
    addr_hints->ai_flags = 0;
    addr_hints->ai_family = AF_INET;
    addr_hints->ai_socktype = SOCK_STREAM;
    addr_hints->ai_protocol = IPPROTO_TCP;
}

int parse_host_port(char *host, char *port, char *host_port){
    char *delimiter = strchr(host_port,':');
    int host_len = delimiter - host_port;
    strncpy(host, host_port, host_len);
    host[host_len] = '\0';
    printf("%s\n", host);
    strcpy(port, host_port + host_len + 1);
    printf("%s\n", port);
}

FILE *get_file(file_name){
    FILE *cookies_file;
    if ((cookies_file = fopen(file_name, "r"))) {
        fclose(cookies_file);
        fatal("Nie ma takiego pliku %s", file_name);
    }
    return cookies_file;
}

int main (int argc, char *argv[]) {
    int rc;
    int sock;
    struct addrinfo addr_hints, *addr_result;
    char request[MAX_REQUEST_LEN];
    char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\n\r\n";
    int request_len;
    char host[HOST_SIZE];
    char port[PORT_SIZE];
    char buffer[BUFFER_SIZE];

    /* Kontrola dokumentów ... */
    if (argc != 4) {
        fatal("Usage: %s connectio-host:connection-port file resource-adress", argv[0]);
    }

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        syserr("socket");
    }

    //Set addr_hints
    set_addr_hints(&addr_hints);

    //Parse host:port
    parse_host_port(host, port, argv[1]);

    //Open file
    FILE *cookies_file = get_file(argv[2]);

    rc =  getaddrinfo(host, port, &addr_hints, &addr_result);
    if (rc != 0) {
        fprintf(stderr, "rc=%d\n", rc);
        syserr("getaddrinfo: %s", gai_strerror(rc));
    }

    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
        syserr("connect");
    }
    freeaddrinfo(addr_result);

    //Most interesting part

    //Prepare request
    request_len = snprintf(request, MAX_REQUEST_LEN, request_template, host);
    if (request_len >= MAX_REQUEST_LEN) {
        //TODO add handling chunk request ???
        fatal("request length large: %d\n", request_len);
    }

    int nbytes_last;
    int nbytes_total = 0;
    while (nbytes_total < request_len) {
        nbytes_last = write(sock, request + nbytes_total, request_len - nbytes_total);
        if (nbytes_last == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        nbytes_total += nbytes_last;
    }

    /* Read the response. */
    fprintf(stderr, "debug: before first read\n");
    while ((nbytes_total = read(sock, buffer, BUFSIZ)) > 0) {
        fprintf(stderr, "debug: after a read\n");
        write(STDOUT_FILENO, buffer, nbytes_total);
    }
    fprintf(stderr, "debug: after last read\n");
    if (nbytes_total == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sock);
    exit(EXIT_SUCCESS);

    return 0;
}

