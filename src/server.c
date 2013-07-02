#include "server.h"

#include <stdlib.h>

#include "synctime.h"

int start_server(char *mcast, short port)
{
    int ret = EXIT_FAILURE;

    ret = start_sync_server(port);

    return ret;
}
