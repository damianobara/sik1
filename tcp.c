#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#define HOST_SIZE      256
#define PORT_SIZE      16

int parse_host_port(char *host, char *port, char *host_port){
    char *delimiter = strchr(host_port,':');
    int host_len = delimiter - host_port;
    if (host_len < 1) {
        fatal("host");
    }
    strncpy(host, host_port, host_len);
    host[host_len] = '\0';
    strcpy(port, host_port + host_len + 1);
    if (strlen(port) < 1) {
        fatal("port");
    }
}

void set_addr_hints(struct addrinfo *addr_hints) {
    memset(addr_hints, 0, sizeof(struct addrinfo));
    addr_hints->ai_flags = 0;
    addr_hints->ai_family = AF_INET;
    addr_hints->ai_socktype = SOCK_STREAM;
    addr_hints->ai_protocol = IPPROTO_TCP;
}

int set_connection(char host_port[]) {
    char host[HOST_SIZE];
    char port[PORT_SIZE];
    int rc;
    int sock;
    struct addrinfo addr_hints, *addr_result;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        syserr("socket");
    }

    //Set addr_hints
    set_addr_hints(&addr_hints);

    //Parse host:port
    parse_host_port(host, port, host_port);

    //Open file

    rc =  getaddrinfo(host, port, &addr_hints, &addr_result);
    if (rc != 0) {
        fprintf(stderr, "rc=%d\n", rc);
        syserr("getaddrinfo: %s", gai_strerror(rc));
    }

    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
        syserr("connect");
    }
    freeaddrinfo(addr_result);
    return sock;
}

int send_data(size_t socket, buffer, buffer_size) {
    size_t nbytes_last, nbytes_total = 0;
    nbytes_total = 0;
    while (nbytes_total < buffer) {
        nbytes_last = write(sock, cookies + nbytes_total, cookies_len - nbytes_total);
        if (nbytes_last == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        nbytes_total += nbytes_last;
    }
}