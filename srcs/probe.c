#include "../lib/traceroute.h"


bool process_probe(t_tracer *trace, t_params *params, char *recv_buf, int byte_size)
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

static void recv_probe(t_tracer *tra, t_params *params)
{
    while  (1)
    {
        char message[MAX_PACKET_SIZE];
        struct sockaddr_in recv_addr;
        socklen_t addr_len;
        ssize_t rec_bytes;
        bool delivered;
        
        ft_memset(message,0,MAX_PACKET_SIZE);
        
        rec_bytes = 0;
        addr_len = sizeof(recv_addr);
        rec_bytes = recvfrom(tra->icmp_sock, message, sizeof(message), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (rec_bytes < 0) {
            printf(" * ");
            if (DEBUG)
                fprintf(stderr,"traceroute : error packet_timeout :V\n");
            break;
        }
        gettimeofday(&tra->end, NULL); //Packet reached reception
        delivered = process_probe_icmp(tra, params, message,rec_bytes);
      
        if (delivered)
            break;
        if (DEBUG)
            fprintf(stderr,"traceroute : message '%d' arrived but failed to be identified :V\n",tra->sequence);
    }
    return;
}

int prepare_trace(struct sockaddr_in addr, t_tracer *trace,t_params *params)
{
    int probe;
   
    for  (int i = 0; i < params->nquerys; i++ )
    {
        probe = 0;
        if (params->flags->I)
            probe = send_probe_icmp( addr,trace, params);
        else
            probe = send_probe_udp( addr,trace, params);

        if (probe == 1)
        {
            gettimeofday(&trace->start, NULL); //Message sended
            recv_probe(trace,params);
        }
        trace->sequence++;
    }

    if (ft_strncmp(trace->router_ip,params->ip_address,INET_ADDRSTRLEN) == 0)
        g_loop_trace = false;
    
    printf("\n");
    return 1;
}