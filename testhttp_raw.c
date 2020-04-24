#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "err.h"

#define MAX_HEADER_SIZE 8192
#define cookies buffer
#define cookies_len buffer_len
#define response buffer
#define response_len buffer_len
#define data    buffer_ptr
#define header  buffer_ptr

int main (int argc, char *argv[]) {
    char buffer[BUFSIZ];
    char *buffer_ptr;
    size_t sock, buffer_len, data_section, headers_section, chunked, header_len, data_processed, chunked_data_size;

    if (argc != 4) fatal("Usage: %s connectio-host:connection-port file resource-adress", argv[0]);

    sock = set_connection(argv[1]);

    FILE *a;
    //Prepare request_head
    buffer_len = prepare_request_head(argv[3], buffer, &buffer_len);
    send_data(sock, buffer, buffer_len);

     while (read_cookies(&cookies_file, cookies, &cookies_len)) {
        send_data(sock, cookies, cookies_len);
    }
    // Close connection
    prepare_close_header(buffer, buffer_len);
    send_data(sock, buffer, buffer_len);

    headers_section = 1;
    data_section = 0;
    while ((response_len = read(sock, response, BUFSIZ)) > 0) {
        if (!is_status_200(response)) {
            print_status(response);
            return 0;
        }

        while(headers_section && get_header(response, response_len, &header, &header_len, MAX_HEADER_SIZE, &headers_section, &data_section)) {
            chunked = is_chunked(header, MAX_HEADER_SIZE);
            handle_cookie(header, header_len);
        }
        if (data_section && chunked) {
            chunked_data_size = get_chunked_data_size(response, response_len);
        }
        while(data_section && process_data(response, response_len, &data, &data_len, &data_processed, data_section));
    }
    close(sock);
    exit(EXIT_SUCCESS);

    return 0;
}

