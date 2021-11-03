#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>

void	*start_routine(void *niks)
{
	printf("hoi! %s\n", (char *)niks);
	return (NULL);
}

int	main(int argc, char **argv)
{
	int	number_of_philosophers;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	number_of_times_each_philosopher_must_eat;
	pthread_t	*threads;
	int	i;

	setbuf(stdout, NULL); //TODO remove
	printf("voor arg check\n");
	if (argc < 5 || argc > 6)
		return (1);
	printf("na arg check\n");
	number_of_philosophers = ft_atoi(argv[1]);
	time_to_die = ft_atoi(argv[2]);
	time_to_eat = ft_atoi(argv[3]);
	time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
		if (number_of_times_each_philosopher_must_eat < 0)
			return (1);
	}
	else
		number_of_times_each_philosopher_must_eat = -1;
	threads = malloc(sizeof(*threads) * number_of_philosophers);
	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_create(threads + i, NULL, start_routine, NULL);
		i++;
	}
	i = 0;
	while (i < number_of_philosophers)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	printf("finished!\n");
}

