#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "cookies.h"

//todo change me
#define HOST_SIZE 1000
#define PORT_SIZE 100
#define REQUEST_HEAD_LEN 1000

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


void read_to_next_line(char **response, size_t *response_len) {
    char * response_ptr = *response;
    response_ptr = strstr(response_ptr, "\r\n");
    if (response_ptr == 0) {
        *response_len = 0;
        //todo check
        *response += *response_len;
    }
    *response_len -= (response_ptr - *response);
    *response = response_ptr;
}

size_t handle_response_status(char **response, size_t *response_len, size_t *response_section, size_t *header_section) {
    char *response_ptr = *response;
    response_ptr = strstr(response_ptr, "200 OK");
    if (response_ptr == 0) {
        //todo change me
        printf("%s", *response);
        return 0;
    }
    if (response_ptr != *response) {
        fatal ("Response does not start with status");
    }
    *response = response_ptr;
    read_to_next_line(response, response_len);
    *response_section = 0;
    *header_section = 1;
    return 1;
}

size_t handle_header(char **response, size_t *response_len, size_t *headers_section, size_t *data_section, size_t *chunked) {
    char *chunked_ptr, *response_ptr, *start_ptr;
    if (!chunked) {
        chunked_ptr = *response;
        chunked_ptr = strstr(chunked_ptr, "Transfer-Encoding: chunked");
        if (chunked_ptr != 0 && chunked_ptr != *response) {
            fatal("Header section line does not start with header.");
        }
        else if (chunked_ptr == *response) {
            *chunked = 1;
        }
    }
    handle_cookie(response, response_len);
    response_ptr = *response;
    response_ptr = strstr(response_ptr, "\r\n");
    if (response_ptr == 0) {
        fatal("Header section longer than 8192 bytes \n");
    }
    start_ptr = *response;
    *response_len = (response_ptr + 2) - * response;
    *response = response_ptr + 2;
    if (response_ptr == start_ptr) {
        *headers_section = 0;
        *data_section = 1;
        return 0;
    }
    return 1;
}

size_t process_data(char **response, size_t *response_len, size_t *data_processed, size_t *data_section, size_t chunked, size_t *chunk_time) {
    size_t counter = 0;
    char *response_ptr = *response;
    size_t response_ptr_len = 0;
    read_to_next_line(&response_ptr, &response_ptr_len);
    if (chunked && *chunk_time) {
        counter = atoi(*response);
        *chunk_time = 0;
    }
    else if (chunked) {
        *chunk_time = 1;
    }
    else {
        counter = (response_ptr - 2) - *response;
    }
    *data_processed += counter;
    if (response_ptr_len == 0) {
        *response_len = 0;
        return 0;
    }
    if (response_ptr == *response + 2) {
        *data_section = 0;
    }
    *response = response_ptr;
    *response_len = response_ptr_len;
    *data_processed += counter;
    return 1;
}



void prepare_close_header(char close_header[], size_t *close_header_len) {
    memset(close_header, '\0', *close_header_len);
    strcpy(close_header, "\nConnection: close\n\n\0");
    *close_header_len = strlen(close_header);
}

