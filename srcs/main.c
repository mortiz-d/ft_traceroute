
#include "../lib/ping.h"


bool g_loop_trace;

int main(int argc, char **argv) 
{  
    t_params *params = NULL;
	t_tracer *pin;
    active_flags flags = {false,false,false,false,false,false,false};
	int sockfd = 0;
	struct sockaddr_in addr;
	// int i;
	// struct sockaddr_in addr; //direccion de destino
    
    if (argc < 2)
    {
        printf (PING_USSAGE_ERROR);
        return 0;
    }

	pin = calloc(1,sizeof(t_tracer));
	pin->id_process = getpid() & 0xFFFF; //Acortamos el tamaño del getpid para que entre bien en el packete icmp(16 bits)
	pin->sequence = 1;
    

    g_loop_trace = true;
    params = calloc(1,sizeof(t_params));
    params->ttl = 1;
    params->timeout_ms = DEFAULT_TIMEOUT;
    params->payload_size = PAYLOAD_SIZE_DEFAULT;
	params->destination = argv[1];
    params->hops = DEFAULT_HOPS;

    if (ping_check_flags(argc, argv, &flags, params) == 0)
    {
        free(params);
        return (0);
    }
    
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (establish_connection(params,sockfd) == 0)
	{
		free(params);
		return 0;
	}
	dns_lookup(params->destination, params);
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, params->ip_address, &addr.sin_addr) <= 0) {
        printf("ping : %s : Nombre o servicio desconocido\n",params->destination);
        if (DEBUG)
        fprintf(stderr,"NO PUDE CONVERTIR A BINARIO LA IP");
        close(sockfd);
        return 0;
    }

	while (g_loop_trace)
	{

        // printf("Salta? %d ",hops);
		if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
		{
            if (DEBUG)
                fprintf(stderr,"NO PUDE ESTABLECER TTL MANUALMENTE\n");
        }
        ft_memset(pin->router_ip , 0, INET_ADDRSTRLEN);
        prepare_trace( sockfd,   addr,  pin, params);
        params->ttl++;
    }
    close(sockfd);
    return 0;
}