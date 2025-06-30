# COLORS #
GREEN = @echo "\033[0;32m"
BLUE = @echo "\033[0;34m" 
PURPLE = @echo "\033[0;35m"
CYAN = @echo "\033[0;36m"
RESET = "\033[1;0m"

# RESOURCES #
SRCS = 	srcs/ft_memset.c 			\
		srcs/ft_bzero.c 			\
		srcs/ft_memcpy.c 			\
		srcs/ft_memmove.c 			\
		srcs/ft_memchr.c 			\
		srcs/ft_memcmp.c 			\
		srcs/ft_strlen.c 			\
		srcs/ft_isalpha.c 			\
		srcs/ft_isdigit.c 			\
		srcs/ft_isalnum.c 			\
		srcs/ft_isascii.c  			\
		srcs/ft_isprint.c 			\
		srcs/ft_toupper.c 			\
		srcs/ft_tolower.c 			\
		srcs/ft_strchr.c 			\
		srcs/ft_strrchr.c 			\
		srcs/ft_strncmp.c  			\
		srcs/ft_strlcpy.c 			\
		srcs/ft_strlcat.c 			\
		srcs/ft_strnstr.c 			\
		srcs/ft_atoi.c 				\
		srcs/ft_calloc.c 			\
		srcs/ft_strdup.c 			\
		srcs/ft_substr.c 			\
		srcs/ft_strjoin.c 			\
		srcs/ft_strtrim.c 			\
		srcs/ft_split.c 			\
		srcs/ft_itoa.c 				\
		srcs/ft_strmapi.c 			\
		srcs/ft_putchar_fd.c  		\
		srcs/ft_putstr_fd.c 		\
		srcs/ft_putendl_fd.c 		\
		srcs/ft_putnbr_fd.c 		\
		srcs/ft_striteri.c 			\
		srcs/ft_count_list.c 		\
		srcs/ft_lstnew_bonus.c 		\
		srcs/ft_lstadd_front_bonus.c\
		srcs/ft_lstsize_bonus.c 	\
		srcs/ft_lstlast_bonus.c 	\
		srcs/ft_lstadd_back_bonus.c \
		srcs/ft_lstdelone_bonus.c 	\
		srcs/ft_lstclear_bonus.c 	\
		srcs/ft_lstiter_bonus.c 	\
		srcs/ft_lstmap_bonus.c 		\

# FLAGS #
OBJS = ${SRCS:srcs/%.c=obj/%.o}

NAME = libft.a

CC = gcc

CFLAGS = -std=c99 -Wall -Werror -Wextra

AR = ar rcs

# INSTRUCTIONS #
all: ${NAME}

${NAME}: ${OBJS}
	${AR} ${NAME} ${OBJS}
	$(GREEN)LIBFT Library Ensambled $(RESET)	

# Rules for object files
obj/%.o: srcs/%.c
	@mkdir -p obj
	${CC} ${CFLAGS} -c $< -o $@
	$(CYAN)Compiled LIBFT Object $< $(RESET)

clean:
	@rm -rf obj
	$(PURPLE)Cleaned LIBFT Objects $(RESET)

fclean: clean
	@rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re