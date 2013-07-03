#include "synctime.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/time.h>

typedef struct{
    u_int64_t sec;
    u_int64_t usec;
} __attribute__((packed)) timestamp_t;

#define TIMESTAMP_NUMBER 10
static timestamp_t timestamp_buffer[TIMESTAMP_NUMBER];
#define BUFFER_SIZE (sizeof(timestamp_t) * TIMESTAMP_NUMBER)


timestamp_t timestamp()
{
    struct timeval ts;
    timestamp_t ret;
    gettimeofday( &ts, NULL );
    ret.sec = (u_int64_t)ts.tv_sec;
    ret.usec = (u_int64_t)ts.tv_usec;
    return ret;
}

int start_sync_server( short port )
{
    int sockd, newsockd, cliLen, yes = 1;

    struct sockaddr_in cliAddr, servAddr;

    /* create socket */
    sockd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockd<0) {
        perror("cannot open socket ");
        return EXIT_FAILURE;
    }

    /* bind server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    if ( setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
    {
        perror("setsockopt");
        return EXIT_FAILURE;
    }

    if(bind(sockd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0)
    {
        perror("cannot bind port ");
        return EXIT_FAILURE;
    }

    listen(sockd,1);

    cliLen = sizeof(cliAddr);
    newsockd = accept(sockd, (struct sockaddr *) &cliAddr, &cliLen);
    if(newsockd<0)
    {
        perror("cannot accept connection ");
        return EXIT_FAILURE;
    }

    /* Do treatment ... */
    read( newsockd, timestamp_buffer, BUFFER_SIZE );
    timestamp_buffer[1] = timestamp();
    timestamp_buffer[2] = timestamp();
    write( newsockd, timestamp_buffer, BUFFER_SIZE );
    read( newsockd, timestamp_buffer, BUFFER_SIZE );
    timestamp_buffer[5] = timestamp();
    timestamp_buffer[6] = timestamp();
    write( newsockd, timestamp_buffer, BUFFER_SIZE );

    close(newsockd);
    close(sockd);

    return EXIT_SUCCESS;
}


int start_sync_client( char * server_addr, short port )
{
    int sockd, loop;

    struct sockaddr_in servAddr;

    /* create socket */
    sockd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockd<0) {
        perror("cannot open socket ");
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(server_addr);
    servAddr.sin_port = htons(port);

    /* connect to server */
    if ( connect(sockd, (struct sockaddr *) &servAddr, sizeof(servAddr)) <0) {
        perror("cannot connect ");
        exit(1);
    }

    memset( timestamp_buffer, 0, BUFFER_SIZE );

    timestamp_buffer[0] = timestamp();
    write( sockd, timestamp_buffer, BUFFER_SIZE );
    read( sockd, timestamp_buffer, BUFFER_SIZE );
    timestamp_buffer[3] = timestamp();
    timestamp_buffer[4] = timestamp();
    write( sockd, timestamp_buffer, BUFFER_SIZE );
    read( sockd, timestamp_buffer, BUFFER_SIZE );
    timestamp_buffer[7] = timestamp();

    /* treat timestamp_buffer */
    printf("Synchronisation result:\n");
    for ( loop = 0 ; loop < 8 ; loop++ )
    {
        printf("%d: %lu,%lu\n", loop, timestamp_buffer[loop].sec, timestamp_buffer[loop].usec );
    }

    close(sockd);

    return EXIT_SUCCESS;
}
