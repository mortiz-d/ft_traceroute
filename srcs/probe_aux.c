#include "../lib/traceroute.h"

void print_trace_header ( t_params *params , t_tracer *trace,char new_router_ip[INET_ADDRSTRLEN])
{
    char *aux;

    if (ft_strlen(trace->router_ip) == 0 || ft_strncmp(trace->router_ip,new_router_ip,INET_ADDRSTRLEN) != 0)
    {
        printf("%s ",new_router_ip);

        if (params->flags->r)
        {
            aux = ip_a_dns(new_router_ip);
            if (aux != NULL)
            {
                printf("(%s) ",aux);
                free(aux);
            }
            else
                printf(" %s ",new_router_ip);
        }
        printf("%.3fms ",time_diff_ms(&trace->start, &trace->end));
        ft_strlcpy(trace->router_ip,new_router_ip,INET_ADDRSTRLEN);
    }
    else
    {
        printf(" %.3fms ",time_diff_ms(&trace->start, &trace->end));
    }
    
}

double time_diff_ms(struct timeval *start, struct timeval *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 +
           (double)(end->tv_usec - start->tv_usec) / 1000.0;
}

unsigned short checksum(char *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;

    //(Segimos el estandard (RFC 1071) y sumamos de 2 bytes en 2 bytes
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len == 1)
        sum += *(unsigned char *)buf;  // último byte si es impar

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}
