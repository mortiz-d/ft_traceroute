
#include "../lib/ping.h"

t_packet	*ft_packetnew(int rtt, bool delivered,int bytes, int sequence, int ttl)
{
	t_packet	*lista;

	lista = malloc(sizeof(t_packet));
	if (lista == 0)
		return (0);
	lista->rtt = rtt;
    lista->bytes = bytes;
    lista->sequence = sequence;
    lista->ttl = ttl;
    lista->delivered = delivered;
	lista->next = 0;
	return (lista);
}

t_packet	*ft_packetlast(t_packet *lst)
{
	if (lst != 0)
	{
		while (lst->next != 0)
			lst = lst->next;
		return (lst);
	}
	return (0);
}

void	ft_packetadd_back(t_packet **lst, t_packet *new)
{
	if (*lst != 0)
		ft_packetlast(*lst)->next = new;
	else
		*lst = new;
}

void free_packet(t_packet *stats)
{
    t_packet *aux;

    aux = stats;
    while (stats)
    {
        aux = stats->next;
        free(stats);
        stats = aux;
    }
}