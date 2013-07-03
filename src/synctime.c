#include "synctime.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/time.h>

static struct timeval timestamp_buffer[10];

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
    read( newsockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    gettimeofday( &timestamp_buffer[1], NULL);
    gettimeofday( &timestamp_buffer[2], NULL);
    write( newsockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    read( newsockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    gettimeofday( &timestamp_buffer[5], NULL);
    gettimeofday( &timestamp_buffer[6], NULL);
    write( newsockd, timestamp_buffer, sizeof(struct timeval) * 10 );

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

    memset( timestamp_buffer, 0, sizeof(struct timeval) * 10 );

    gettimeofday( &timestamp_buffer[0], NULL);
    write( sockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    read( sockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    gettimeofday( &timestamp_buffer[3], NULL);
    gettimeofday( &timestamp_buffer[4], NULL);
    write( sockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    read( sockd, timestamp_buffer, sizeof(struct timeval) * 10 );
    gettimeofday( &timestamp_buffer[7], NULL);

    /* treat timestamp_buffer */
    printf("Synchronisation result:\n");
    for ( loop = 0 ; loop < 8 ; loop++ )
    {
        printf("%d: %lu,%lu\n", loop, timestamp_buffer[loop].tv_sec, timestamp_buffer[loop].tv_usec );
    }

    close(sockd);

    return EXIT_SUCCESS;
}
