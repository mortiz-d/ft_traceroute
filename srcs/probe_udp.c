#include "../lib/traceroute.h"

int send_probe_udp(struct sockaddr_in addr, t_tracer *trace,t_params *params)
{
    char *mensaje;
    int     socket;
    int     send;
    ssize_t mensaje_size;
    uint16_t dest_port ;

    dest_port = UDP_DEFAULT_BASE_PORT + trace->sequence;
    socket = trace->udp_sock;
    mensaje_size = params->payload_size;

    mensaje = ft_calloc(mensaje_size, sizeof(char));
    addr.sin_port = htons(dest_port);

    send = sendto(socket, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send < 0) {
        if (DEBUG)
            fprintf(stderr,"traceroute : Error sending UDP probe\n");
        return 0;
    }

    free(mensaje);
    return 1;
}


bool process_probe_udp(t_tracer *trace, t_params *params, char *recv_buf, int byte_size)
{
    //Extracting and figuring out how the icmp package
    struct iphdr   *ip_hdr    = (struct iphdr *)recv_buf;
    struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
    char router_ip[INET_ADDRSTRLEN];

    //Check integrity
    if (checksum((void *)recv_icmp, byte_size) != 0)
    {
       if (DEBUG)
            printf("ICMP packet integrity compromised\n");
        return false;
    }

    if (recv_icmp->type == ICMP_TIME_EXCEEDED || recv_icmp->type == ICMP_DEST_UNREACH)
    {
        inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
        print_trace_header(params, trace, router_ip);
        return true;
    }
    return false;
}

// In case we want to butcher the message
// bool process_probe_udp(t_tracer *trace, t_params *params, char *recv_buf, int byte_size)
// {
//     //Extracting and figuring out how the icmp package
//     struct iphdr   *ip_hdr    = (struct iphdr *)recv_buf;
//     struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buf + ip_hdr->ihl * 4);
//     char router_ip[INET_ADDRSTRLEN];
//     //To gut TTL Exceeded timeout packets
//     char *ptr = (char *)(recv_icmp + 1);
//     struct iphdr  *inner_ip   = (struct iphdr *)ptr;
//     int            inner_len  = inner_ip->ihl * 4;
//     //To gut the UDP probe
//     struct udphdr *inner_udp ;
//     uint16_t dest_port;
//     uint16_t expect_port;

//     //Check integrity
//     if (checksum((void *)recv_icmp, byte_size) != 0)
//     {
//        if (DEBUG)
//             printf("ICMP packet integrity compromised\n");
//         return false;
//     }

//     //UDP messages only recieve a reply in case de packet didnt arrive correctly (TTL timeout/port unreachable)
//     if (recv_icmp->type == ICMP_TIME_EXCEEDED || recv_icmp->type == ICMP_DEST_UNREACH)
//     {
//         inner_udp  = (struct udphdr *)(ptr + inner_len);
//         dest_port   = ntohs(inner_udp->dest);
//         expect_port = UDP_DEFAULT_BASE_PORT + trace->sequence;
            
//         if (dest_port != expect_port)
//             return false;
                
            
//         inet_ntop(AF_INET, &ip_hdr->saddr, router_ip, sizeof router_ip);
//         print_trace_header(params, trace, router_ip);
//         return true;
    
//     }
//     return false;
// }