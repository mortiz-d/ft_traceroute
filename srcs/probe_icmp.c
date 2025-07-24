#include "../lib/traceroute.h"

int send_probe_icmp(struct sockaddr_in addr, t_tracer *trace,t_params *params)
{
    char *mensaje;
    int     socket;
    int     send;
    ssize_t mensaje_size;
    struct icmphdr *icmp; //Encabezado de mensaje ICMP (8 bytes)

    mensaje_size = params->payload_size + ICMPHDR;
    mensaje = ft_calloc(mensaje_size, sizeof(char));

    icmp = (struct icmphdr *)mensaje;
    icmp->type = ICMP_ECHO;                 //tipo de mensaje 0 = Echo
    icmp->code = ICMP_ECHO_CODE;            //Codigo del echo 0 = normal
    icmp->un.echo.id = trace->id_process;     //Identificador para saber que es nuestro
    icmp->un.echo.sequence = trace->sequence; //Nº de secuencia
    icmp->checksum = checksum(mensaje, mensaje_size);
    socket = trace->icmp_sock;

    send = sendto(socket, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send < 0) {
        if (DEBUG)
            fprintf(stderr,"Error sending the damm message :V\n");
        return 0;
    }

    free(mensaje);
    return 1;
}

bool process_probe_icmp(t_tracer *trace, t_params *params, char *recv_buf, int byte_size)
{
    //Extracting and figuring out how the icmp package
    struct iphdr   *ip_hdr    = (struct iphdr *)recv_buf;
    struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
    char router_ip[INET_ADDRSTRLEN];
    //Check integrity
    if (checksum((void *)recv_icmp, byte_size) != 0)
    {
       if (DEBUG)
            printf("traceroute: ICMP packet integrity compromised\n");
        return false;
    }

    if (recv_icmp->type == ICMP_ECHOREPLY ||
        recv_icmp->type == ICMP_TIME_EXCEEDED ||
        recv_icmp->type == ICMP_DEST_UNREACH
    )
    {
        inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
        print_trace_header(params ,trace, router_ip);
        return true;
    }
    return false;  
   
}

// In case we want to butcher the message
// bool process_probe_icmp(t_tracer *trace, t_params *params, char *recv_buf, int byte_size)
// {
//     //Extracting and figuring out how the icmp package
//     struct iphdr   *ip_hdr    = (struct iphdr *)recv_buf;
//     struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
//     char router_ip[INET_ADDRSTRLEN];
//     //To gut TTL Exceeded timeout packets
//     char *ptr = (char *)(recv_icmp + 1);
//     struct iphdr  *inner_ip   = (struct iphdr *)ptr;
//     int            inner_len  = inner_ip->ihl * 4;
//     //In case we have to gut the ICMP probe 
//     struct icmphdr *inner_icmp;
    
//     //Check integrity
//     if (checksum((void *)recv_icmp, byte_size) != 0)
//     {
//        if (DEBUG)
//             printf("traceroute: ICMP packet integrity compromised\n");
//         return false;
//     }

//     //Probe reached destiny
//     if (recv_icmp->type == ICMP_ECHOREPLY){
//         if (recv_icmp->un.echo.id == trace->id_process &&
//         recv_icmp->un.echo.sequence == trace->sequence)
//         {
//             inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
//             print_trace_header(params ,trace, router_ip);
//             return true;
//         }
//     }

//     //TTL too short || Unreachable
//     if (recv_icmp->type == ICMP_TIME_EXCEEDED ||
//         recv_icmp->type == ICMP_DEST_UNREACH)
//     {
//         inner_icmp = (struct icmphdr *)(ptr + inner_len);
//         (void)inner_icmp;
//         if (inner_icmp->un.echo.id == trace->id_process &&
//         inner_icmp->un.echo.sequence == trace->sequence)
//         {
//             inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
//             print_trace_header( params, trace, router_ip);
//             return true;
//         }
//     }
//     return false;  
   
// }