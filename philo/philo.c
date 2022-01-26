#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct s_shared {
	int	number_of_philosophers;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	number_of_times_each_philosopher_must_eat;
	volatile int	one_dead;
	pthread_mutex_t	*forks;
	pthread_mutex_t butler;
} t_shared;

typedef struct s_info {
	t_shared *shared;
	int	philo_i;
} t_info;

enum error {
	SUCCESS,
	ERROR
};

long	timeval_to_ms(struct timeval timeval)
{
	return ((long)timeval.tv_sec * 1000 + timeval.tv_usec / 1000);
}

long	get_time()
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval_to_ms(timeval));
}

int ft_mutex_unlock(pthread_mutex_t *mutex)
{
	if (pthread_mutex_unlock(mutex))
	{
		printf("mutex unlock error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int ft_mutex_init(pthread_mutex_t *mutex)
{
	if (pthread_mutex_init(mutex, NULL))
	{
		printf("mutex init error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int ft_mutex_lock(pthread_mutex_t *mutex)
{
	if (pthread_mutex_lock(mutex))
	{
		printf("mutex lock error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int	take_forks(t_shared *shared, t_info *info, int *forks_in_hand)
{
	long now;

	if (pthread_mutex_lock(&shared->butler))
		return (ERROR);
	if (ft_mutex_lock(&shared->forks[info->philo_i]))
		return (ERROR);
	now = get_time();
	printf("%ld %d has taken a fork\n", now, info->philo_i + 1);
	if (ft_mutex_lock(&shared->forks[(info->philo_i + 1) % shared->number_of_philosophers]))
		return (ERROR);
	now = get_time();
	printf("%ld %d has taken a fork\n", now, info->philo_i + 1);
	if (pthread_mutex_unlock(&shared->butler))
		return (ERROR);
	*forks_in_hand = 2;
	return (SUCCESS);
}

enum activity {
	EATING,
	SLEEPING,
	THINKING
};

void	*start_routine(void *info_void)
{
	t_shared		*shared;
	int				forks_in_hand;
	long			last_ate;
	t_info			*info;
	long			now;
	long			activity_started;
	enum activity	activity;

	info = info_void;
	shared = info->shared;
	forks_in_hand = 0;
	now = get_time();
	last_ate = now;
	activity = THINKING;
	activity_started = now;

	printf("%ld %d is thinking\n", now, info->philo_i + 1);
	printf("hi! I'm %d, %d, %d\n", info->philo_i + 1, shared->number_of_philosophers, shared->time_to_die);
	while (!shared->one_dead)
	{
		now = get_time();
		if (activity != EATING && now > last_ate + shared->time_to_die)
		{
			printf("now: %ld last_ate: %ld, ttd: %d\n", now, last_ate, shared->time_to_die);
			shared->one_dead = 1;
			printf("%ld %d died\n", now, info->philo_i + 1);
			break;
		}
		if (activity == THINKING)
		{
			if (forks_in_hand == 2)
			{
				printf("%ld %d is eating\n", now, info->philo_i + 1);
				activity = EATING;
				activity_started = now;
			}
			else if (take_forks(shared, info, &forks_in_hand))
				return (NULL);
		}
		else if (activity == SLEEPING)
		{
			if (now - activity_started > shared->time_to_sleep)
			{
				activity = THINKING; 
				activity_started = now;
				printf("%ld %d is thinking\n", now, info->philo_i + 1);
			}
		} 
		else if (activity == EATING)
		{
			last_ate = now;
			if (now - activity_started > shared->time_to_eat)
			{
				ft_mutex_unlock(&shared->forks[info->philo_i]);
				ft_mutex_unlock(&shared->forks[(info->philo_i + 1) % shared->number_of_philosophers]);
				forks_in_hand = 0;
				activity = SLEEPING;
				activity_started = now;
				printf("%ld %d is sleeping\n", now, info->philo_i + 1);
			}
		}
	}
	ft_mutex_unlock(&shared->butler);
	if (forks_in_hand == 2)
	{
		ft_mutex_unlock(&shared->forks[info->philo_i]);
		ft_mutex_unlock(&shared->forks[(info->philo_i + 1) % shared->number_of_philosophers]);
	}
	return (NULL);
}

enum error initialize_shared(t_shared *shared, int argc, char **argv)
{
	shared->number_of_philosophers = ft_atoi(argv[1]);
	shared->time_to_die = ft_atoi(argv[2]);
	shared->time_to_eat = ft_atoi(argv[3]);
	shared->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		shared->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
		if (shared->number_of_times_each_philosopher_must_eat < 0)
			return (ERROR);
	}
	else
		shared->number_of_times_each_philosopher_must_eat = -1;
	shared->one_dead = 0;
	shared->forks = malloc(sizeof(*shared->forks) * shared->number_of_philosophers);
	return (SUCCESS);
}

int initialize_mutexes(t_shared *shared)
{
	int	i;
	int	err;
	
	i = 0;
	while (i < shared->number_of_philosophers)
	{
		err = ft_mutex_init(shared->forks + i);
		if (err)
			return (err);
		i++;
	}
	err = ft_mutex_init(&shared->butler);
	if (err)
		return (err);
	return (SUCCESS);
}

int	initialize_threads(t_shared *shared, t_info *info, pthread_t *threads)
{
	int	i;
	int	err;

	i = 0;
	while (i < shared->number_of_philosophers)
	{
		info[i].shared = shared;
		info[i].philo_i = i;
		err = pthread_create(threads + i, NULL, start_routine, &info[i]);
		if (err)
			return (err);
		i++;
	}
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_shared	shared;
	pthread_t	*threads;
	int	i;
	t_info		*info;
	enum error	err;

	printf("voor arg check\n");
	if (argc < 5 || argc > 6)
		return (1);
	printf("na arg check\n");
	err = initialize_shared(&shared, argc, argv);
	if (err)
		return (err);
	threads = malloc(sizeof(pthread_t) * shared.number_of_philosophers);
	info = malloc(sizeof(*info) * shared.number_of_philosophers);
	err = initialize_mutexes(&shared);
	if (err)
		return (err);
	err = initialize_threads(&shared, info, threads);
	if (err)
		return (err);
	while (!shared.one_dead)
		;
	i = 0;
	printf("finished!\n");
	usleep(shared.time_to_die * shared.number_of_philosophers * 2);
	while (i < shared.number_of_philosophers)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	free(shared.forks);
	free(info);
	free(threads);
}

