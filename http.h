#ifndef DUZE_ZADANIE1_HTTP_H
#define DUZE_ZADANIE1_HTTP_H

#include "err.h"

size_t is_status_200(char response[]);
size_t print_status(char response[]);
size_t prepare_request_head(char *resource, char *request_head);
void prepare_close_header(char close_header[], size_t *close_header_len);
size_t get_header(char response[],size_t response_len, char **header, size_t *header_len, size_t max_header_size, size_t *headers_section, size_t *data_section);
size_t is_chunked(char header[], size_t max_header_size);
size_t get_chunked_data_size(char response[], size_t response_len);
size_t process_data(char response[], size_t response_len, char *data_ptr, size_t *data_len, size_t data_processed, size_t *data_section);


#endif
