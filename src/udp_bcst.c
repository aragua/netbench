#include "udp_bcst.h"

#include "timestamp.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define TRY_NUMBER 10

typedef struct{
    timestamp_t ts;
    u_int32_t count;
} __attribute__((packed)) udp_bcst_t;

int start_udp_bcst_server( char * server_addr, short port )
{
    int sockfd = -1, is_broadcast = 1, loop;
    struct sockaddr_in saddr;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    size_t rs;

    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
    if ( sockfd  == -1)
    {
        printf ("unable to create socket\n");
        return EXIT_FAILURE;
    }
    //set broadcast flag on socket if isBroadcast is true on the context
    if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, &is_broadcast, sizeof (int)) == -1 )
        printf ("Unable to change socket options %s\n", strerror (errno));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons (port);
    saddr.sin_addr.s_addr = inet_addr (server_addr);

    for ( loop = 0; loop < TRY_NUMBER ; loop++)
    {
        usleep(10000);
        static udp_bcst_t buffer;
        buffer.ts = timestamp();
        buffer.count = loop;
        rs = sendto( sockfd, &buffer, sizeof(timestamp_t), 0, (struct sockaddr*)&saddr, clientlen);
    }

    close(sockfd);

    return 0;
}

int start_udp_bcst_client( short port )
{
    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    int sockfd = -1, reuse = 1, loop;
    size_t rs = 0;

    static udp_bcst_t ts_buffer[TRY_NUMBER*2];

    socklen_t clientlen = sizeof(struct sockaddr_in);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket creation failed");
        return -1;
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Error");
    }

    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr)) == -1)
    {
        printf("unable to bind socket");
        return -1;
    }

    for ( loop = 0; loop < TRY_NUMBER ; loop++)
    {
        rs = recvfrom( sockfd, &ts_buffer[loop*2], sizeof(timestamp_t), 0, (struct sockaddr*)&client_addr, &clientlen);
        ts_buffer[(loop+1)*2].ts = timestamp();
        //printf("top %u\n", ts_buffer[loop*2].count);
    }

    for ( loop = 0; loop < TRY_NUMBER ; loop++)
    {
        printf("%u : %u,%u --> %u,%u\n", ts_buffer[loop*2].count, ts_buffer[loop*2].ts.sec, ts_buffer[loop*2].ts.usec, ts_buffer[(loop+1)*2].ts.sec, ts_buffer[(loop+1)*2].ts.usec );
    }

    close(sockfd);
    return 0;
}
