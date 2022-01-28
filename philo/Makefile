NAME = philo
SRC = philo.c ft_atoi.c death.c init.c time.c forks.c activities.c mutex.c \
	  print.c parse.c
OBJ = $(SRC:%.c=obj/%.o)
ifdef DEBUG
	CC=clang
	FLAGS = -Wall -Wextra -fsanitize=address -DDEBUG=1 -g
else
	CC=gcc
	FLAGS = -Wall -Wextra -Werror
endif

all: make_dir $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

obj/%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@

make_dir:
	mkdir -p obj

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
.PHONY: all 
