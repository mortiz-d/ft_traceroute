#include "../lib/ping.h"

int establish_connection(t_params *params,int sockfd)
{
    struct timeval tv_timeout;

    if (sockfd < 0) {
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER RAW SOCKET\n");
        return 1;
    }

	tv_timeout.tv_sec = params->timeout_ms / 1000;
    tv_timeout.tv_usec = (params->timeout_ms % 1000) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_timeout, sizeof(tv_timeout)) < 0) {
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER TIMEOUT MANUALMENTE\n");
        close(sockfd);
        
        return 0;
    }
    return 1;
}
