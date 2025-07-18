#ifndef PING_H
# define PING_H

#define _POSIX_C_SOURCE 200112L //Esto es para mi ordenador que puede actuar funny el hijopu
#define NI_MAXHOST 1025
#define PING_USSAGE_ERROR "ping :usage error:a direction must be specified\n"


#define PAYLOAD_SIZE_DEFAULT 32 //Tamaño del mensaje en ICMP que queremos mandar
#define ICMPHDR (sizeof(struct icmphdr))
#define TOTAL_SIZE (ICMPHDR+sizeof(struct iphdr))

#define DEFAULT_TTL 64
#define DEFAULT_TIMEOUT 1000
#define DEFAULT_HOPS 30
#define DEFAULT_NQUERIES 3
#define UDP_DEFAULT_BASE_PORT 33434

#define ICMP_ECHO_CODE 0
#define DEBUG 0

#include "../libft/lib/libft.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netdb.h> 
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h> 

extern bool  g_loop_trace;

typedef struct s_flags{                   
    bool v;
    bool h;
    bool I;
    bool q;
    bool m;
    bool r;
    bool undef;
} t_flags;

typedef struct s_params
{
    t_flags         *flags;
    char			*destination;
    char            ip_address[INET_ADDRSTRLEN];
	int				ttl;
    int 			timeout_ms;
    int				payload_size;
    int             hops;
    int             nquerys;

}	t_params;

typedef struct s_ping
{
    int             icmp_sock;
    int             udp_sock;
	int				id_process;
	int				sequence;
    int             ttl;
    char            router_ip[INET_ADDRSTRLEN];
    struct timeval	start;
	struct timeval	end;
}	t_tracer;


void handle_sigint(int sig);

//Socket connection
bool establish_connection(t_params *params,int new_sockfd);
bool update_ttl_sockets(t_tracer *pin, t_params *params);
bool close_sockets(t_tracer *pin);
//DNS resolution
int dns_lookup(char *host, t_params *params);
char *ip_a_dns(const char *ip_str);

int ping_check_flags(int argc, char **argv,t_params *params);

int prepare_trace( struct sockaddr_in addr, t_tracer *pin,t_params *params);

//UDP
bool process_probe_udp(t_tracer *pin, t_params *params, char *recv_buf);
int send_probe_udp(struct sockaddr_in addr, t_tracer *pin,t_params *params);

//ICMP
bool process_probe_icmp(t_tracer *pin, t_params *params, char *recv_buf);
int send_probe_icmp(struct sockaddr_in addr, t_tracer *pin,t_params *params);

//AUX
unsigned short checksum(char *b, int len);
double time_diff_ms(struct timeval *start, struct timeval *end);
void print_trace_header ( t_params *params , t_tracer *trace,char new_router_ip[INET_ADDRSTRLEN]);

#endif