#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"


//todo change me
#define HOST_SIZE 1000
#define PORT_SIZE 100
#define REQUEST_HEAD_LEN 1000

size_t is_status_200(char response[]) {
    return  strstr(response, "200 OK") != 0;
}

size_t print_status(char response[]) {
    print("%s", response);
}

void parse_request_resourse(char resource[], char target[], char host[]) {
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


size_t get_header(char response[],size_t response_len, char **header, size_t *header_len, size_t max_header_size, size_t *headers_section, size_t *data_section) {
    //todo
    return 1;

}

size_t is_chunked(char header[], size_t max_header_size) {
    //todo
    return 1;
}

size_t get_chunked_data_size(char response[], size_t response_len) {
    //todo
    return 1;
}

size_t process_data(char response[], size_t response_len, char *data_ptr, size_t *data_len, size_t data_processed, size_t *data_section) {
    //todo
    return 1;
}


size_t prepare_request_head(char *resource, char *request_head) {
    int request_head_len = 0;
    char host[HOST_SIZE];
    char target[HOST_SIZE];
    char get_header_template[] = "GET %s HTTP/1.1\n";
    char host_header_template[] = "Host: %s\n";
    char cookie_header_head[] = "Cookie: ";
    parse_request_resourse(resource, target, host);
    printf( "HOST %s\n", host);
    printf("TARGET %s\n", target);

    request_head_len = snprintf(request_head, REQUEST_HEAD_LEN, get_header_template, target);
    request_head_len += snprintf(request_head + request_head_len, REQUEST_HEAD_LEN, host_header_template, host);
    request_head_len += snprintf(request_head + request_head_len, REQUEST_HEAD_LEN, cookie_header_head);
//    request_head_len += snprintf(request_head + request_head_len, REQUEST_HEAD_LEN, connection_close_header);

    printf("%s", request_head);
    return request_head_len;
}

void prepare_close_header(char close_header[], size_t *close_header_len) {
    close_header = "\nConnection: close\n\n";
    *close_header_len = 0;
}

