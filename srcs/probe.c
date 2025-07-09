#include "../lib/ping.h"

static unsigned short checksum(void *b, int len) {
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


static double time_diff_ms(struct timeval *start, struct timeval *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 +
           (double)(end->tv_usec - start->tv_usec) / 1000.0;
}

static void print_trace_header ( t_tracer *trace,char new_router_ip[INET_ADDRSTRLEN])
{
    char *aux;

    if (ft_strlen(trace->router_ip) == 0 || ft_strncmp(trace->router_ip,new_router_ip,INET_ADDRSTRLEN) != 0)
    {
        aux = ip_a_dns(new_router_ip);
        if (aux != NULL)
        {
            printf(" %s ",aux);
            free(aux);
        }
        else
            printf(" %s ",new_router_ip);

        printf("(%s) %.3fms",new_router_ip,time_diff_ms(&trace->start, &trace->end));
        ft_strlcpy(trace->router_ip,new_router_ip,INET_ADDRSTRLEN);
    }
    else
    {
        printf(" %.3fms",time_diff_ms(&trace->start, &trace->end));
    }
    
}

static bool check_inner_ICMP_package(struct icmphdr *inner_icmp, int desired_type, t_tracer *pin)
{
    if (inner_icmp->type == desired_type &&
        inner_icmp->un.echo.id == pin->id_process &&
        inner_icmp->un.echo.sequence == pin->sequence)
        return true;
    return false; 
}


static bool process_probe(t_tracer *pin, t_params *params, char *recv_buf)
{
    //Extracting and figuring out how the icmp package
    struct iphdr   *ip_hdr    = (struct iphdr *)recv_buf;
    struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
    char router_ip[INET_ADDRSTRLEN];
    //To gut TTL Exceeded timeout packets
    char *ptr = (char *)(recv_icmp + 1);
    struct iphdr  *inner_ip   = (struct iphdr *)ptr;
    int            inner_len  = inner_ip->ihl * 4;

    //In case we have to gut the ICMP probe 
    struct icmphdr *inner_icmp;
    //In case we have to gut the UDP probe
    struct udphdr *inner_udp ;
    uint16_t dest_port;
    uint16_t expect_port;

    //ICMP Probe
    if (params->flags->I)
    {
        //Probe reached destiny
        if (recv_icmp->type == ICMP_ECHOREPLY &&
        recv_icmp->un.echo.id == pin->id_process &&
        recv_icmp->un.echo.sequence == pin->sequence)
        {

            inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
            print_trace_header(pin, router_ip);
            return true;
        }

        //TTL too short
        if (recv_icmp->type == ICMP_TIME_EXCEEDED ||
            recv_icmp->type == ICMP_DEST_UNREACH)
        {
            inner_icmp = (struct icmphdr *)(ptr + inner_len);
            if (check_inner_ICMP_package(inner_icmp,ICMP_ECHO,pin) || check_inner_ICMP_package(inner_icmp,ICMP_ECHOREPLY,pin))
            {
                inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
                print_trace_header( pin, router_ip);
                return true;
            }
        }
            
        return false;  
    }
    else //UDP Probe
    {
            inner_udp  = (struct udphdr *)(ptr + inner_len);
            dest_port   = ntohs(inner_udp->dest);
            expect_port = UDP_DEFAULT_BASE_PORT + pin->sequence;
            
            if (dest_port != expect_port)
                return false;
            
            
            inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
            print_trace_header(pin, router_ip);
            return true;
    }
    
    return false;
}

static void recv_probe(t_tracer *pin, t_params *params, ssize_t mensaje_size)
{
    char mensaje[mensaje_size];
    struct sockaddr_in recv_addr;
    socklen_t addr_len;
    ssize_t rec_bytes;
    int max_attempts;

    max_attempts = 3;
    ft_memset(mensaje,0,mensaje_size);
    while(max_attempts > 0)
    {
        rec_bytes = 0;
        addr_len = sizeof(recv_addr);
        rec_bytes = recvfrom(pin->icmp_sock, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (rec_bytes < 0) {
            printf(" * ");
            if (DEBUG)
                fprintf(stderr,"Error receiving the damm message :V\n");
            break;
        }
        gettimeofday(&pin->end, NULL); //Packet reached reception
        if (process_probe(pin, params, mensaje))
            break;
        max_attempts--;
    }
    return;
}

static int send_probe(struct sockaddr_in addr, t_tracer *pin,t_params *params)
{
    char *mensaje;
    int     socket;
    int     send;
    ssize_t mensaje_size;
    //ICMP
    struct icmphdr *icmp; //Encabezado de mensaje ICMP (8 bytes)
    //UDP
    uint16_t dest_port = UDP_DEFAULT_BASE_PORT + pin->sequence;

    socket = pin->udp_sock;
    mensaje_size = params->payload_size;
    if (params->flags->I)
        mensaje_size += ICMPHDR;

    mensaje = ft_calloc(mensaje_size, sizeof(char));

    if (params->flags->I)
    {
        icmp = (struct icmphdr *)mensaje;
        icmp->type = ICMP_ECHO;                 //tipo de mensaje 0 = Echo
        icmp->code = ICMP_ECHO_CODE;            //Codigo del echo 0 = normal
        icmp->un.echo.id = pin->id_process;     //Identificador para saber que es nuestro
        icmp->un.echo.sequence = pin->sequence; //Nº de secuencia
        icmp->checksum = checksum(mensaje, mensaje_size);
        socket = pin->icmp_sock;
    }
    else
        addr.sin_port = htons(dest_port);

    send = sendto(socket, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send < 0) {
        if (DEBUG)
            fprintf(stderr,"Error sending the damm message :V\n");
        return 0;
    }

    free(mensaje);
    return 1;
}


int prepare_trace(struct sockaddr_in addr, t_tracer *pin,t_params *params)
{
    printf("%d ",params->ttl);
    for  (int i = 0; i < params->nquerys; i++ )
    {
        if (send_probe( addr,pin, params) == 1)
        {
            gettimeofday(&pin->start, NULL); //Message sended
            recv_probe(pin,params,(TOTAL_SIZE + params->payload_size));
        }
        pin->sequence++;
    }

    if (ft_strncmp(pin->router_ip,params->ip_address,INET_ADDRSTRLEN) == 0)
        g_loop_trace = false;
    
    printf("\n");
    return 1;
}