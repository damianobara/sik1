#ifndef DUZE_ZADANIE1_HTTP_H
#define DUZE_ZADANIE1_HTTP_H

#include "err.h"

size_t handle_response_status(char **response, size_t *response_len, size_t *response_section, size_t *header_section);
size_t prepare_request_head(char *resource, char *request_head);
void prepare_close_header(char close_header[], size_t *close_header_len);
size_t handle_header(char **response, size_t *response_len, size_t *headers_section, size_t *data_section, size_t * chunked);
size_t process_data(char **response, size_t *response_len, size_t *data_processed, size_t *data_section, size_t chunked, size_t *chunk_time);

#endif
