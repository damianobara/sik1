#include "err.h"
#include "tcp.h"
#include "http.h"
#include "cookies.h"


int main (int argc, char *argv[]) {
    tcp_test();
    http_test();
    cookies_test();
    err_test("main");
    return 0;
}

