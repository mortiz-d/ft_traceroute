#include "../lib/ping.h"

double time_diff_ms(struct timeval *start, struct timeval *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 +
           (double)(end->tv_usec - start->tv_usec) / 1000.0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void print_trace_header ( t_params *params ,t_tracer *trace,char new_router_ip[INET_ADDRSTRLEN])
{
    char *aux;

    (void) params;
    if (ft_strlen(trace->router_ip) == 0)
    {
        aux = ip_a_dns(new_router_ip);
        if (aux != NULL)
        {
            printf(" %s ",aux);
            free(aux);
        }
        else
            printf(" %s ",new_router_ip);

        printf("(%s) %.3f ms", new_router_ip,time_diff_ms(&trace->start, &trace->end));
        ft_strlcpy(trace->router_ip,new_router_ip,INET_ADDRSTRLEN);
    }
    else if (ft_strncmp(trace->router_ip,new_router_ip,INET_ADDRSTRLEN) != 0)
    {
        printf(" %s %.3f ms",new_router_ip,time_diff_ms(&trace->start, &trace->end));
    }
    else
    {
        printf(" %.3f ms",time_diff_ms(&trace->start, &trace->end));
    }
    
}

bool process_ping(t_tracer *pin,t_params *params,char recv_buf[ICMPHDR + params->payload_size])
{
    struct iphdr   *ip_hdr   = (struct iphdr *)recv_buf;
    struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
    char router_ip[INET_ADDRSTRLEN];

    if (recv_icmp->type == ICMP_TIME_EXCEEDED)
    {
        /* A) Saltar cabecera ICMP externa (8 bytes) */
        char *ptr = (char *)(recv_icmp + 1);

        /* B) Cabecera IP interna */
        struct iphdr *inner_ip = (struct iphdr *)ptr;
        int inner_ip_len = inner_ip->ihl * 4;

        /* C) Cabecera ICMP Echo original */
        struct icmphdr *inner_icmp = (struct icmphdr *)(ptr + inner_ip_len);

        /* D) ¿Pertenece a nuestra sonda? */
        if (inner_icmp->type == ICMP_ECHO &&
            inner_icmp->un.echo.id == pin->id_process &&
            inner_icmp->un.echo.sequence == pin->sequence)
        {
            inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
            print_trace_header(params, pin, router_ip);
            return true;          /* salto válido */
        }
        return false;             /* no es nuestra sonda */
    }
    if (recv_icmp->type == ICMP_ECHOREPLY &&
        recv_icmp->un.echo.id == pin->id_process &&
        recv_icmp->un.echo.sequence == pin->sequence)
    {
        inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
        print_trace_header(params, pin, router_ip);
        return true;              /* destino alcanzado */
    }

    return false;                 /* paquete ajeno */
}


void recv_ping(int sockfd,t_tracer *pin, t_params *params)
{
    char recv_buf[TOTAL_SIZE + params->payload_size];
    struct sockaddr_in recv_addr;
    socklen_t addr_len;
    ssize_t rec_bytes;
    int max_attempts;
    

    ft_memset(recv_buf,0,((int)TOTAL_SIZE + params->payload_size));
    max_attempts = 3;
    while (max_attempts > 0)
    {
        rec_bytes = 0;
        addr_len = sizeof(recv_addr);
        rec_bytes = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (rec_bytes < 0) {
            // aux->ttl = -1;
            printf(" * ");
            if (DEBUG)
                fprintf(stderr,"recvfrom : No obtuvo respuesta\n");
            break;
        }
        gettimeofday(&pin->end, NULL);
        if(process_ping(pin,params,recv_buf) == true)
            break;
        max_attempts--;
    }
    
    return;
}

int send_ping(int sockfd, struct sockaddr_in addr, t_tracer *pin,t_params *params)
{
    struct icmphdr *icmp; //Encabezado de mensaje ICMP (8 bytes)
    char packet[ICMPHDR + params->payload_size ]; //Tamaño del mensaje
    int send;

    //Establecemos el mensaje ICMP que mandamos
    memset(packet, 0, ICMPHDR + params->payload_size);
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO;                 //tipo de mensaje 0 = Echo
    icmp->code = ICMP_ECHO_CODE;            //Codigo del echo 0 = normal
    icmp->un.echo.id = pin->id_process;     //Identificador para saber que es nuestro
    icmp->un.echo.sequence = pin->sequence; //Nº de secuencia
    icmp->checksum = checksum(packet, ICMPHDR + params->payload_size);

    //Mandamos el mensaje
    send =  sendto(sockfd, packet, ICMPHDR + params->payload_size, 0,(struct sockaddr *)&addr, sizeof(addr));
    if (send <= 0) {
        if (DEBUG)
            fprintf(stderr,"sendto");
        return (0);
    }
    return (1);
}

int prepare_trace(int sockfd, struct sockaddr_in addr, t_tracer *pin,t_params *params)
{
    printf("%d ",params->ttl);
    for  (int i = 0; i < 3; i++ )
    {
        if (send_ping( sockfd, addr,pin, params) == 1)
        {
                // printf("Mensaje enviado\n");
                gettimeofday(&pin->start, NULL);
                recv_ping(sockfd,pin,params);
        }
        pin->sequence++;
    }
    if (ft_strncmp(pin->router_ip,params->ip_address,INET_ADDRSTRLEN) == 0)
        g_loop_trace = false;
    printf("\n");
    return 1;
}