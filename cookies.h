#ifndef DUZE_ZADANIE1_COOKIES_H
#define DUZE_ZADANIE1_COOKIES_H

#include "err.h"

FILE *get_file_desc(char *file_name);
size_t handle_cookie(char **response, size_t *response_len);
size_t read_cookies(FILE **cookies_file, char cookies[], size_t *cookies_string_len);
size_t is_blank(char *c);

#endif
