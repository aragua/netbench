#include "client.h"

#include <stdlib.h>


int start_client(char *sync, char *mcast, short port)
{
    int ret = EXIT_FAILURE;

    ret = start_sync_client( sync, port);

    return ret;
}
