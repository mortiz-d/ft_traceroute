
#include "../lib/traceroute.h"


bool g_loop_trace;

int close_all(t_params *params,t_tracer *pin,int error_code)
{
    close_sockets(pin);
    if (pin)
        free(pin);
    if(params)
    {
        free(params->flags);
        free(params);
    }

    return (error_code);
}

void setup_default_params(t_params *params,t_tracer *pin)
{
    t_flags *flags;

    flags = ft_calloc(1,sizeof(t_flags));
	pin->id_process = getpid() & 0xFFFF; //Acortamos el tamaño del getpid para que entre bien en el packete icmp(16 bits)
	pin->sequence = 1;
    
    
    g_loop_trace = true;
    params->ttl = 1; //En este caso queremos que entren por defecto desde el mas bajo
    params->timeout_ms = DEFAULT_TIMEOUT;
    params->payload_size = PAYLOAD_SIZE_DEFAULT;
    params->hops = DEFAULT_HOPS;
    params->nquerys = DEFAULT_NQUERIES;
    params->flags = flags;

}

int assign_destination(char **argv,int argc, t_params *params)
{
    int i;

    i = 1;
    params->destination = NULL;
    while (i < argc)
    {
        if (argv[i][0] != '\0')
        {
            params->destination = argv[i];
            break;
        }
        // printf("%s\n",argv[i]);
        i++;
    }
    // printf("destiny is %s\n",params->destination);
    if (params->destination == NULL)
        return 0;
    return 1;

}


int main(int argc, char **argv) 
{  
    t_params *params = NULL;
	t_tracer *pin;
	struct sockaddr_in addr;//direccion de destino
    
    if (argc < 2)
    {
        printf (PING_USSAGE_ERROR);
        return 0;
    }

    pin = calloc(1,sizeof(t_tracer));
    params = calloc(1,sizeof(t_params));
    setup_default_params(params,pin);

    if (ping_check_flags(argc, argv, params) == 0)
        return (close_all(params,pin,1));

    if (!assign_destination(argv,argc,params))
        return (close_all(params,pin,1));

    pin->udp_sock  = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    pin->icmp_sock = socket(AF_INET, SOCK_RAW,  IPPROTO_ICMP);
    
    if (!establish_connection(params,pin->udp_sock))
		return (close_all(params,pin,1));

	if (!establish_connection(params,pin->icmp_sock))
        return (close_all(params,pin,1));

	dns_lookup(params->destination, params);
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, params->ip_address, &addr.sin_addr) <= 0) {
        printf("ping : %s : Nombre o servicio desconocido\n",params->destination);
        if (DEBUG)
        fprintf(stderr,"NO PUDE CONVERTIR A BINARIO LA IP");
        return (close_all(params,pin,1));
    }

    printf("traceroute to %s (%s) , %d hops max , %ld byte packets\n",params->destination, params->ip_address, params->hops, (TOTAL_SIZE + params->payload_size));
	while (g_loop_trace && params->hops-- > 0)
	{
		if (!update_ttl_sockets(pin, params)) 
            break;
        
        ft_memset(pin->router_ip , 0, INET_ADDRSTRLEN);
        prepare_trace(addr, pin, params);
        params->ttl++;
    }
    close_all(params,pin,0);
    return 0;
}