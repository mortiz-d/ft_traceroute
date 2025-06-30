
#include "../lib/ping.h"


bool g_loop_trace;

int main(int argc, char **argv) 
{  
    t_params *params = NULL;
	t_tracer *pin;
    active_flags flags = {false,false,false,false,false,false,false};
	int sockfd = 0;
	struct sockaddr_in addr;
    // t_packet *packet_aux;
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
    
	// packet_aux = ft_packetnew(0,false,0,pin->sequence,0);

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

	// printf("Se ha creado el socket - %d\n",sockfd);
    // printf("WOLOLO\n");
    // reverse_dns_lookup();
    // return 0;
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
            // if (send_ping( sockfd, addr,pin, params) == 1)
            // {
            //     // printf("Mensaje enviado\n");
            //     recv_ping(sockfd,pin,packet_aux,params);
                
            //     // if (packet_aux->delivered)
            //     // {
            //     // printf("ඞ %d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
            //     //     packet_aux->bytes,             //bytes
            //     //     params->ip_address,     //ip
            //     //     packet_aux->sequence,          //icmp_secuencia
            //     //     packet_aux->ttl,               //ttl (cuantos routers tiene que pasar hasta destino)
            //     //     packet_aux->rtt);
                    
            //     // }
            //     pin->sequence++;
            //     params->ttl++;
            // }
        params->ttl++;
        pin->sequence++;
        

    }
    close(sockfd);


    // i = 1;
    // while (i < argc)
    // {
	// 	if (argv[i][0] == '\0')
	// 	{
	// 		i++;
	// 		continue;
	// 	}
        
	// 	params->destination = argv[i];
    
    //     if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &(params->ttl), sizeof(params->ttl)) < 0) 
    //     {
    //         if (DEBUG)
    //             fprintf(stderr,"NO PUDE ESTABLECER TTL MANUALMENTE\n");
    //         close(sockfd);
    //         free(params);
    //         return 1;
    //     }


	// 	// if (send_ping(socketfd, addr, pin) == 1)
    //     // {
    //     //     recv_ping(socketfd,pin,aux);
		
	// 	// }
            
		
        
        
    // //     if (flags.v)
    // //         printf("ping: sock4.fd: %d (socktype: SOCK_RAW, protocol: IPPROTO_ICMP), ttl=%d, timeout=%dms\n\nai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", sockfd, params->ttl, params->timeout_ms, params->destination);
            
    // //     signal(SIGINT, handle_sigint);
    // //     ping_loop(sockfd,params,&flags);
    //     close(sockfd);
    //     if (!g_loop_ping)
    //         break;
    //     i++;
        
    // }
    // free(params);
    return 0;
}