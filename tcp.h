#ifndef DUZE_ZADANIE1_TCP_H
#define DUZE_ZADANIE1_TCP_H

#include "err.h"


void set_addr_hints(struct addrinfo *addr_hints);
size_t set_connection(char host_port[]);
size_t send_data(size_t socket, char buffer[], size_t buffer_size);

#endif
