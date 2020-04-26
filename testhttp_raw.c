#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "err.h"

#include "tcp.h"
#include "http.h"
#include "cookies.h"

#define MAX_HEADER_SIZE 8192
#define cookies buffer
#define cookies_len buffer_len
#define response buffer
#define response_len buffer_len
#define data    buffer_ptr
#define header  buffer_ptr

int main (int argc, char *argv[]) {
    char buffer[BUFSIZ];
    size_t sock, buffer_len, response_section, headers_section, data_section, continuation;
    size_t chunked, chunk_time, data_processed;
    FILE *cookies_file;

    if (argc != 4) fatal("Usage: %s connectio-host:connection-port file resource-adress", argv[0]);

    sock = set_connection(argv[1]);

    buffer_len = prepare_request_head(argv[3], buffer);
    send_data(sock, buffer, buffer_len);

    cookies_file = get_file_desc(argv[2]);
     while (read_cookies(&cookies_file, cookies, &cookies_len)) {
        send_data(sock, cookies, cookies_len);
    }
    // Close connection
    prepare_close_header(buffer, &buffer_len);
    send_data(sock, buffer, buffer_len);

    response_section = 1;
    headers_section = 0;
    data_section = 0;
    continuation = 0;
    memset(response, '\0', BUFSIZ);
    while ((response_len = read(sock, response, BUFSIZ)) > 0) {
        buffer_len = BUFSIZ;
        if (response_section && !handle_response_status(&response, &response_len, &response_section, &headers_section)) {
            return 0;
        }
        while(response_len && headers_section) {
            handle_header(&response, &response_len, &headers_section, &data_section, &chunked);
        }
        while (response_len && data_section) {
            process_data(response, response_len, &data_processed, &data_section, chunked, &chunk_time);
        }
        if (!data_section) {
            printf("%d", data_processed);
            break;
        }
    }
    close(sock);
    exit(EXIT_SUCCESS);

    return 0;
}

