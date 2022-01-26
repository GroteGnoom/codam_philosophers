NAME = philo
SRC = philo.c ft_atoi.c death.c init.c time.c forks.c activities.c mutex.c
ifdef DEBUG
	CC=clang
	FLAGS = -Wall -Wextra -fsanitize=address -DDEBUG=1 -g
else
	CC=gcc
	FLAGS = -Wall -Wextra -Werror
endif

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(NAME)

fclean:
	rm -f $(NAME)

re: fclean all
.PHONY: all 
