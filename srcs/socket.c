#include "../lib/traceroute.h"

bool update_ttl_sockets(t_tracer *pin, t_params *params)
{
    if (setsockopt(pin->icmp_sock, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
	{
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER TTL MANUALMENTE\n");
        return false;
    }
    if (setsockopt(pin->udp_sock, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
	{
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER TTL MANUALMENTE\n");
        return false;
    } 
    return true;
}

bool establish_connection(t_params *params,int sockfd)
{
    struct timeval tv_timeout;

    if (sockfd < 0) {
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER RAW SOCKET\n");
        return false;
    }

	tv_timeout.tv_sec = params->timeout_ms / 1000;
    tv_timeout.tv_usec = (params->timeout_ms % 1000) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_timeout, sizeof(tv_timeout)) < 0) {
        if (DEBUG)
            fprintf(stderr,"NO PUDE ESTABLECER TIMEOUT MANUALMENTE\n");
        close(sockfd);
        
        return false;
    }
    return true;
}

bool close_sockets(t_tracer *pin)
{
    if (pin->icmp_sock >= 0)
        close(pin->icmp_sock);
    if (pin->udp_sock >= 0)
        close(pin->udp_sock);
    return true;
}