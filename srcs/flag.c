#include "../lib/traceroute.h"

int invoque_flag_help(void)
{
	printf("Usage:\n"
		"traceroute [options...] <destination>\n"
		"Options:\n"
		"	-f <NUM>			set initial hop distance, i.e., time-to-live\n"
		"	-h					print help and exit\n"
		"	-I					use ICMP ECHO as probe\n"
		"	-q	<NUM>			send NUM probes packet per hop (default : 3)\n"
		"	-m	<NUM>			sets max amount of hops (default : 64)\n"
		"	--resolve-hostnames	resolve hostnames\n"
		"For more details consult mortiz-d or traceroute...\n"
		"Arguments:\n"
		"+     host          The host to traceroute to\n"
      	"packetlen     The full packet length (default is the length of an IP header plus 40). Can be ignored or increased to a minimal allowed value\n"
	);
    return (0);
}

bool is_exact_word(char *str, char *cmp)
{
	if (ft_strncmp(str,cmp,ft_strlen(cmp)) == 0 && strlen(str) == strlen(cmp))
		return true;
	return false;
}

int needs_argument(char c,t_flags *flags)
{
	flags->h = true;
	printf ("traceroute : %c requires an argument\n",c);
	return (invoque_flag_help());
}


bool valid_argument(char *str, int max_range, int min_range)
{
	char *aux_c;
    int aux;
	bool result;

	result = false;
	aux = ft_atoi(str);
    aux_c = ft_itoa(aux);


    if (ft_strncmp(str, aux_c, ft_strlen(str)) == 0)
		result = true;
	else
		printf("traceroute: invalid argument: %s\n",str);
	free(aux_c);
	if (aux > max_range || aux < min_range )
	{
		result = false;
		printf("traceroute: invalid argument: '%s': out of range: %d <= value <= %d\n",str,min_range,max_range);
	}
	return (result);
}


bool is_key_word(char * str)
{
	char *aux[] = {"-h","-I","-q","-m","--resolve-hostnames","-f"};
	int i;
	
	i = 0;
	while (i <  6)
		if (is_exact_word(str,aux[i++]))
	 		return true;
	return false;
}

int trace_check_flags(int argc, char **argv, t_params *params)
{
	(void) params;
	int i = 1;

	while (i < argc)
	{
	
		if (is_exact_word("-h",argv[i]))	//Flag help
			return (invoque_flag_help());
		else if (is_exact_word("--resolve-hostnames",argv[i]))	//Flag resolve hostnames
			params->flags->r = true;
		else if (is_exact_word("-I",argv[i]))	//Flag ICMP ECHO probes activated
		{
			params->flags->I = true;
		}
		else if (is_exact_word("-q",argv[i]))	//nqueries
		{
			if ((i + 1) >= argc)
				return needs_argument('q',params->flags);

            if (valid_argument(argv[i+1], 10, 1))
			{
				params->nquerys = ft_atoi(argv[i+1]);
				params->flags->q = true;
			}
            else
				return 0;
			argv[i+1][0] = '\0';
		}
		else if (is_exact_word("-m",argv[i]))	//maxttl - sets number of hops
		{
			if ((i + 1) >= argc)
				return needs_argument('m',params->flags);

            if (valid_argument(argv[i+1], 255, 1))
			{
				params->hops = ft_atoi(argv[i+1]);
			}
            else
				return 0;
			argv[i+1][0] = '\0';
		}
		else if (is_exact_word("-f",argv[i]))	//first-hop
		{
			if ((i + 1) >= argc)
				return needs_argument('m',params->flags);

            if (valid_argument(argv[i+1], 254, 1))
			{
				params->ttl = ft_atoi(argv[i+1]);
			}
            else
				return 0;
			argv[i+1][0] = '\0';
		}


		if (is_key_word(argv[i]))
			argv[i][0] = '\0';

		i++;
	}
	// params->flags = *flags;
	return 1;
}


