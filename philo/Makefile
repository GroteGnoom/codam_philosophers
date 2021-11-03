NAME = philo
SRC = philo.c ft_atoi.c
ifndef DEBUG
	FLAGS = -Wall -Wextra -Werror
else
	FLAGS = -Wall -Wextra
endif

all: $(NAME)

$(NAME): $(SRC)
	gcc $(FLAGS) $(SRC) -o $(NAME)

.PHONY: all 
