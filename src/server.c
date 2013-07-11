#include "server.h"

#include <stdlib.h>

#include "synctime.h"

int start_server(char *mcast, short port)
{
    int ret = EXIT_FAILURE;

    ret = start_sync_server(port);

    ret = start_udp_bcst_server("192.168.0.255", port);

    return ret;
}
