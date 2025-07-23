# COLORS #
GREEN = @echo "\033[0;32m"
BLUE = @echo "\033[0;34m" 
PURPLE = @echo "\033[0;35m"
CYAN = @echo "\033[0;36m"
RESET = "\033[1;0m"

# RESOURCES #
SRCS =	srcs/main.c \
		srcs/socket.c \
		srcs/flag.c \
		srcs/probe.c \
 		srcs/probe_aux.c \
		srcs/probe_icmp.c \
		srcs/probe_udp.c \
		srcs/dns.c  

# FLAGS #
OBJS_DIR = objetos
OBJS = $(SRCS:srcs/%.c=$(OBJS_DIR)/%.o)

PROYECT_NAME = Traceroute
NAME = ft_traceroute

CC = gcc

CFLAGS = -std=c99   -Wall -Werror -Wextra
LIBFT_A = libft/libft.a

# Instructions #
all: $(NAME)

$(NAME): $(OBJS)
	@make -C ./libft
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT_A)
#	mv ./$(NAME) ./testing/files/$(NAME)
	$(BLUE)$(PROYECT_NAME) set Up $(RESET)

$(OBJS_DIR)/%.o: srcs/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -g -c $<  -o $@
	$(CYAN)$(PROYECT_NAME) Object Compiled $< $(RESET)

clean:
	@make fclean -s -C./libft
	@rm -rf $(OBJS_DIR)
	$(PURPLE)Cleaned $(PROYECT_NAME) Objects $(RESET)

fclean: clean
	rm -rf $(NAME)
	$(PURPLE)Cleaned $(PROYECT_NAME) Executable $(RESET)	

re: fclean all

test: re
	clear
	./$(NAME) -a $(FILE_TEST)

.PHONY: all clean fclean re test




