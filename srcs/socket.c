#include "../lib/traceroute.h"

bool update_ttl_sockets(t_tracer *trace, t_params *params)
{
    if (setsockopt(trace->icmp_sock, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
	{
        fprintf(stderr,"traceroute: error : cannot stablish ttl on icmp socket\n");
        return false;
    }
    if (setsockopt(trace->udp_sock, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
	{
        fprintf(stderr,"traceroute: error : cannot stablish ttl on udp socket\n");
        return false;
    } 
    return true;
}

bool establish_connection(t_params *params,int sockfd)
{
    struct timeval tv_timeout;

    if (sockfd < 0) {
        fprintf(stderr,"traceroute: socket error : cannot stablish socket , try using root user \n");
        return false;
    }

	tv_timeout.tv_sec = params->timeout_ms / 1000;
    tv_timeout.tv_usec = (params->timeout_ms % 1000) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_timeout, sizeof(tv_timeout)) < 0) {
        fprintf(stderr,"traceroute: error : cannot stablish timeout for socket\n");
        close(sockfd);
        return false;
    }
    return true;
}

bool close_sockets(t_tracer *trace)
{
    if (trace->icmp_sock >= 0)
        close(trace->icmp_sock);
    if (trace->udp_sock >= 0)
        close(trace->udp_sock);
    return true;
}