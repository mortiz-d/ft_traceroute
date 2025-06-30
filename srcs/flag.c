#include "../lib/ping.h"

// ping: usage error: Se debe especificar la dirección de destino (no poner argumento)
// ping: invalid argument: 'asdaa' (erroneus input)

int invoque_flag_help(void)
{
	printf("Usage:\n"
		"traceroute [] [] [options] <destination>\n"
		"Options:\n"
		"   -h                 print help and exit\n"
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

int ping_check_flags(int argc, char **argv, active_flags *flags,t_params *params)
{
	(void) flags;
	(void) params;
	int i = 1;

	while (i < argc)
	{
		if (is_exact_word("-h",argv[i]))	//Flag help
			return (invoque_flag_help());
		i++;
	}
	return 1;
}

// bool valid_argument(char *str, int max_range, int min_range)
// {
// 	char *aux_c;
//     int aux;
// 	bool result;

// 	result = false;
// 	aux = ft_atoi(str);
//     aux_c = ft_itoa(aux);


//     if (ft_strncmp(str, aux_c, ft_strlen(str)) == 0)
// 		result = true;
// 	else
// 		printf("ping: invalid argument: %s\n",str);
// 	free(aux_c);
// 	if (aux > max_range || aux < min_range )
// 	{
// 		result = false;
// 		printf("ping: invalid argument: '%s': out of range: %d <= value <= %d\n",str,min_range,max_range);
// 	}
// 	return (result);
// }

// int needs_argument(char c,active_flags *flags)
// {
// 	flags->h = true;
// 	printf ("ping : %c requires an argument\n",c);
// 	return (invoque_flag_help());
// }


// bool is_key_word(char * str)
// {
// 	char *aux[] = {"-h","-v","-c","-D","-t","-s"};
// 	int i;
	
// 	i = 0;
// 	while (i <=  5)
// 		if (is_exact_word(str,aux[i++]))
// 	 		return true;
// 	return false;
// }
