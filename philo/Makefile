NAME = philo
SRC = main.c ft_atoi.c init.c time.c forks.c\
	  print.c parse.c death.c mutex.c
OBJ = $(SRC:%.c=obj/%.o)
ifdef DEBUG
	CC=clang
	FLAGS = -Wall -Wextra -fsanitize=thread -DDEBUG=1 -g
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
	@mkdir -p obj

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
.PHONY: all 
