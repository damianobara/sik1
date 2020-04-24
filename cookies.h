#ifndef DUZE_ZADANIE1_COOKIES_H
#define DUZE_ZADANIE1_COOKIES_H

size_t handle_cookie(char header[], size_t header_len);
size_t read_cookies(FILE **cookies_file, char cookies[], size_t *cookies_string_len);

#endif
