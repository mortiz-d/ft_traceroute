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
		srcs/icmp.c \
 		srcs/packet.c \
		srcs/dns.c  
# 		srcs/signal.c \
# 		srcs/free.c \

# FLAGS #
OBJS_DIR = objetos
OBJS = $(SRCS:srcs/%.c=$(OBJS_DIR)/%.o)

NAME = ft_traceroute

CC = gcc

CFLAGS = -std=c99   -Wall -Werror -Wextra
LIBFT_A = libft/libft.a

# Instructions #
all: $(NAME)

$(NAME): $(OBJS)
	@make -C ./libft
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT_A)
	$(BLUE) NM set Up $(RESET)

$(OBJS_DIR)/%.o: srcs/%.c
#Creamos las carpetas necesarias para compilar
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -g -c $<  -o $@
	$(CYAN) NM Object Compiled $< $(RESET)

clean:
	@make fclean -s -C./libft
	@rm -rf $(OBJS_DIR)
	$(PURPLE) Cleaned NM Objects $(RESET)

fclean: clean
	rm -rf $(NAME)
	$(PURPLE) Cleaned NM Executable $(RESET)	

re: fclean all

test: re
	clear
	./$(NAME) -a $(FILE_TEST)

.PHONY: all clean fclean re test




