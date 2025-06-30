
#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/ip.h>    
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define ICMP_ECHO     8
#define ICMP_ECHOREPLY 0
#define PACKET_SZ     64
#define TIMEOUT_MS    1000

/* Calcula el checksum del encabezado ICMP */
static uint16_t checksum(void *buf, int len)
{
    uint32_t sum = 0;
    uint16_t *ptr = buf;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    if (len == 1) {
        uint16_t odd = 0;
        *(uint8_t *)&odd = *(uint8_t *)ptr;
        sum += odd;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (uint16_t)~sum;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <host> [max_ttl]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *target = argv[1];
    int max_ttl = (argc >= 3) ? atoi(argv[2]) : 30;

    /* Resolver el host destino */
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if (getaddrinfo(target, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in dest = *(struct sockaddr_in *)res->ai_addr;
    freeaddrinfo(res);

    /* Crear socket RAW */
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Construir paquete ICMP base */
    char sendbuf[PACKET_SZ];
    memset(sendbuf, 0, sizeof(sendbuf));
    struct icmphdr *icmp = (struct icmphdr *)sendbuf;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(getpid() & 0xFFFF);

    puts("traceroute_simple – saltos intermedios:");
    for (int ttl = 1; ttl <= max_ttl; ++ttl) {
        /* Establecer TTL */
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("setsockopt(IP_TTL)");
            break;
        }

        /* Actualizar datos variables del paquete */
        icmp->un.echo.sequence = htons(ttl);
        icmp->checksum = 0;
        icmp->checksum = checksum(sendbuf, sizeof(sendbuf));

        struct timeval start, end;
        gettimeofday(&start, NULL);

        if (sendto(sockfd, sendbuf, sizeof(sendbuf), 0,
                   (struct sockaddr *)&dest, sizeof(dest)) < 0) {
            perror("sendto");
            break;
        }

        /* Esperar respuesta (select con timeout) */
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        struct timeval tv = {
            .tv_sec  = TIMEOUT_MS / 1000,
            .tv_usec = (TIMEOUT_MS % 1000) * 1000
        };

        int ready = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (ready > 0 && FD_ISSET(sockfd, &readfds)) {
            char recvbuf[512];
            struct sockaddr_in reply_addr;
            socklen_t addrlen = sizeof(reply_addr);
            ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                                 (struct sockaddr *)&reply_addr, &addrlen);
            if (n > 0) {
                gettimeofday(&end, NULL);
                double rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
                             (end.tv_usec - start.tv_usec) / 1000.0;

                /* Obtener IP de quien responde */
                char addrstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &reply_addr.sin_addr, addrstr, sizeof(addrstr));

                struct iphdr *ip = (struct iphdr *)recvbuf;
                struct icmphdr *r_icmp = (struct icmphdr *)(recvbuf + ip->ihl * 4);

                printf("%2d  %-15s  %.3f ms\n", ttl, addrstr, rtt);

                if (r_icmp->type == ICMP_ECHOREPLY) {
                    puts("¡Destino alcanzado!");
                    break;
                }
            }
        } else {
            printf("%2d  * * *  (timeout)\n", ttl);
        }
    }

    close(sockfd);
    return 0;
}
