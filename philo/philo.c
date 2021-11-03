#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct s_shared {
	int	number_of_philosophers;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	number_of_times_each_philosopher_must_eat;
	volatile int	one_dead;
	pthread_mutex_t	*forks;
} t_shared;

typedef struct s_info {
	t_shared *shared;
	int	philo_i;
} t_info;

long	timeval_to_ms(struct timeval timeval)
{
	return ((long)timeval.tv_sec * 1000 + timeval.tv_usec / 1000);
}

void	*start_routine(void *info_void)
{
	t_shared	*shared;
	int			forks_in_hand;
	long		last_ate;
	struct timeval	timeval;
	t_info		*info;
	long		now;

	info = info_void;
	shared = info->shared;
	forks_in_hand = 0;
	gettimeofday(&timeval, NULL);

	last_ate = timeval_to_ms(timeval);

	printf("hoi! I'm %d, %d, %d\n", info->philo_i, shared->number_of_philosophers, shared->time_to_die);
	while (!shared->one_dead)
	{
		gettimeofday(&timeval, NULL);
		now = timeval_to_ms(timeval);
		if (now > last_ate + shared->time_to_die)
			shared->one_dead = 1;
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_shared	shared;
	pthread_t	*threads;
	int	i;
	t_info		*info;

	setbuf(stdout, NULL); //TODO remove
	printf("voor arg check\n");
	if (argc < 5 || argc > 6)
		return (1);
	printf("na arg check\n");
	shared.number_of_philosophers = ft_atoi(argv[1]);
	shared.time_to_die = ft_atoi(argv[2]);
	shared.time_to_eat = ft_atoi(argv[3]);
	shared.time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		shared.number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
		if (shared.number_of_times_each_philosopher_must_eat < 0)
			return (1);
	}
	else
		shared.number_of_times_each_philosopher_must_eat = -1;
	shared.one_dead = 0;
	threads = malloc(sizeof(*threads) * shared.number_of_philosophers);
	shared.forks = malloc(sizeof(*shared.forks) * shared.number_of_philosophers);
	info = malloc(sizeof(*info) * shared.number_of_philosophers);
	i = 0;
	while (i < shared.number_of_philosophers)
	{
		pthread_mutex_init(shared.forks + i, NULL);
		i++;
	}
	i = 0;
	while (i < shared.number_of_philosophers)
	{
		info[i].shared = &shared;
		info[i].philo_i = i;
		pthread_create(threads + i, NULL, start_routine, &info[i]);
		i++;
	}
	while (!shared.one_dead)
		;
	i = 0;
	printf("finished!\n");
	while (i < shared.number_of_philosophers)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

