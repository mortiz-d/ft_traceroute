#ifndef PING_H
# define PING_H

#define _POSIX_C_SOURCE 200112L //Esto es para mi ordenador que puede actuar funny el hijopu
#define NI_MAXHOST 1025
#define PING_USSAGE_ERROR "ping :usage error:a direction must be specified\n"


#define PAYLOAD_SIZE_DEFAULT 56 //Tamaño del mensaje en ICMP que queremos mandar
#define ICMPHDR (sizeof(struct icmphdr))
#define TOTAL_SIZE (ICMPHDR+sizeof(struct iphdr))

#define DEFAULT_TTL 64
#define DEFAULT_TIMEOUT 1000
#define DEFAULT_HOPS 3 

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
#include <sys/time.h>
#include <errno.h>
#include <netdb.h> 

extern bool  g_loop_trace;

typedef struct s_packet
{
    int             bytes;
    int             sequence;
    int             ttl;
    double			rtt;
    bool            delivered;
	struct s_packet	*next;
}	t_packet;

//List functions
t_packet	*ft_packetnew(int rtt, bool delivered,int bytes, int sequence, int ttl);
t_packet	*ft_packetlast(t_packet *lst);
void	ft_packetadd_back(t_packet **lst, t_packet *new);
void free_packet(t_packet *stats);

typedef struct s_params
{
    struct timeval	start_tracer;
	struct timeval	end_ping;
    char			*destination;
    char            ip_address[INET_ADDRSTRLEN];
	int				ttl;
    int 			timeout_ms;
    int 			count;
    int				payload_size;
    //Trace params
    int             hops;

}	t_params;

typedef struct s_ping
{
	int				id_process;
	int				sequence;
    int             ttl;
    char            router_ip[INET_ADDRSTRLEN];
    struct timeval	start;
	struct timeval	end;
}	t_tracer;

typedef struct s_stat
{
	double      min;
    double      max;
    double      sum;
    int         count;
    int         count_rec;
    double      avg;
    double      mdev;
    double      mdev_aux;
}	t_stat;

typedef struct active_flags{                   
    bool v;
    bool h;
    bool c;
    bool D;
    bool t;
    bool s;
    bool undef;
} active_flags;


void handle_sigint(int sig);
// int send_ping(int sockfd, struct sockaddr_in addr, t_tracer *pin,t_params *params);
// void recv_ping(int sockfd,t_tracer *pin, t_params *params);
int dns_lookup(char *host, t_params *params);
int ping_check_flags(int argc, char **argv,active_flags *flags,t_params *params);
int establish_connection(t_params *params,int new_sockfd);
int prepare_trace(int sockfd, struct sockaddr_in addr, t_tracer *pin,t_params *params);
char *ip_a_dns(const char *ip_str);
#endif