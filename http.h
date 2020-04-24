#ifndef DUZE_ZADANIE1_HTTP_H
#define DUZE_ZADANIE1_HTTP_H

size_t is_status_200(char response[]);
size_t print_status(char response[]);
size_t prepare_request_head(char *resource, char *request_head, char *request_head_len);
void prepare_close_header(char close_header[], size_t *close_header_len);

#endif
