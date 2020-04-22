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
#define HOST_SIZE      256
#define PORT_SIZE      16
#define MAX_REQUEST_HEAD_LEN      1024
#define COOKIES_MAX_SIZE    204800

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

int parse_request_resourse(char resource[], char target[], char host[]) {
    if (strncmp(resource, "http://", strlen("http://")) != 0) {
        fatal("not http");
    }
    if (strncmp(resource, "https://", strlen("https://")) == 0) {
        fatal("https");
    }
    char *target_start = strchr(resource + strlen("http://"), '/');
    if (!target_start) {
        strcpy(host, resource + strlen("http://"));
        strcpy(target, "/\0");
    }
    else {
        strncpy(host, resource + strlen("http://"), target_start - (resource + strlen("http://")));
        strcpy(target, target_start);
    }
}

FILE *get_file_desc(char *file_name){
    FILE *cookies_file;
    if (!(cookies_file = fopen(file_name, "r"))) {
        fclose(cookies_file);
        fatal("Nie ma takiego pliku %s", file_name);
    }
    return cookies_file;
}

int prepare_request_head(char *request_head, char *resource) {
    int request_head_len = 0;
    char host[HOST_SIZE];
    char target[HOST_SIZE];
    char get_header_template[] = "GET %s HTTP/1.1\n";
    char host_header_template[] = "Host: %s\n";
    char cookie_header_head[] = "Cookie: ";
    parse_request_resourse(resource, target, host);
    printf( "HOST %s\n", host);
    printf("TARGET %s\n", target);

    request_head_len = snprintf(request_head, MAX_REQUEST_HEAD_LEN, get_header_template, target);
    request_head_len += snprintf(request_head + request_head_len, MAX_REQUEST_HEAD_LEN, host_header_template, host);
    request_head_len += snprintf(request_head + request_head_len, MAX_REQUEST_HEAD_LEN, cookie_header_head);
//    request_head_len += snprintf(request_head + request_head_len, MAX_REQUEST_HEAD_LEN, connection_close_header);

    printf("%s", request_head);
    return request_head_len;
}

int read_cookies(FILE **cookies_file, char cookies[]) {
    int cookies_len = 0;
    char * cookie_line = NULL;
    size_t cookie_line_len = 0;
    ssize_t read, len;
    int read_sth = 0;
    while ((read = getline(&cookie_line, &len, *cookies_file)) != -1) {
        read_sth = 1;
        cookie_line_len = strlen(cookie_line);
        if (cookie_line_len < COOKIES_MAX_SIZE - cookies_len) {
            strcpy(cookies + cookies_len, cookie_line);
            cookies[cookies_len + cookie_line_len - 1] = ';';
            cookies[cookies_len + cookie_line_len] = ' ';
            cookies_len += cookie_line_len + 1;
        }
        else{
            fputs(cookie_line, *cookies_file);
            break;
        }
    }
    printf("%s", cookies);
    return read_sth && (cookie_line_len < COOKIES_MAX_SIZE - cookies_len);
}

int main (int argc, char *argv[]) {
    int rc;
    int sock;
    struct addrinfo addr_hints, *addr_result;
    char request_head[MAX_REQUEST_HEAD_LEN];
    char host[HOST_SIZE];
    char port[PORT_SIZE];
    char buffer[BUFFER_SIZE];
    FILE *cookies_file;
    char cookies[COOKIES_MAX_SIZE];

//    char a[HOST_SIZE];
//    char b [HOST_SIZE];
//    parse_request_resourse(argv[3], a, b);

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
    cookies_file = get_file_desc(argv[2]);

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

    //Prepare request_head
    int request_head_len = prepare_request_head(request_head, argv[3]);
    write(sock, request_head, request_head_len);

    int nbytes_last;
    int nbytes_total = 0;
    int cookies_len;
    while (read_cookies(&cookies_file, cookies)){
        cookies_len = strlen(cookies);
        printf("%s\n", cookies);
        while (nbytes_total < cookies_len) {
            nbytes_last = write(sock, cookies + nbytes_total, cookies_len - nbytes_total);
            if (nbytes_last == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            nbytes_total += nbytes_last;
        }
    }
    //TODO ZMIENIC REQUEST
    // Close connection
    char connection_close_header[] = "\nConnection: close\n\n";
    write(sock, connection_close_header, strlen(connection_close_header));

    printf("READ PART\n");
    /* Read the response. */
    fprintf(stderr, "debug: before first read\n");
    memset(buffer, 0, sizeof(buffer));
    nbytes_total = read(sock, buffer, BUFSIZ);
    if (nbytes_total == -1) {
        fatal("read");
    }
    char *token = NULL;
    token = strtok(buffer, "\n");
    // Not 200 OK
    if (strstr(buffer, "200 OK") == 0) {
        printf("%s", token);
        return 0;
    }
    // 200 OK
    int set_cookie_start = strstr(buffer, "Set-Cookie:");
    printf("%s", set_cookie_start);

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

