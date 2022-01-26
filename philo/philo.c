/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:24:29 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/26 15:49:21 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef DEBUG
# define DEBUG 0
#endif

typedef struct s_shared {
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	int				allowed_to_print;
	volatile int	one_dead;
	pthread_mutex_t	*forks;
	int				*forks_2;
	pthread_mutex_t	butler;
	pthread_mutex_t	print_butler;
}	t_shared;

enum e_activity {
	EATING,
	SLEEPING,
	THINKING
};

typedef struct s_philo {
	t_shared		*shared;
	int				philo_i;
	enum e_activity	activity;
	long			activity_started;
	long			last_ate;
	int				forks_in_hand;
}	t_philo;

enum e_error {
	SUCCESS,
	ERROR
};

long	timeval_to_ms(struct timeval timeval)
{
	return ((long)timeval.tv_sec * 1000 + timeval.tv_usec / 1000);
}

long	get_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval_to_ms(timeval));
}

int	ft_mutex_unlock(pthread_mutex_t *mutex)
{
	if (pthread_mutex_unlock(mutex))
	{
		printf("mutex unlock error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int	ft_mutex_init(pthread_mutex_t *mutex)
{
	if (pthread_mutex_init(mutex, NULL))
	{
		printf("mutex init error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int	ft_mutex_lock(pthread_mutex_t *mutex)
{
	if (pthread_mutex_lock(mutex))
	{
		printf("mutex lock error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

int	checked_print(t_shared *shared, t_philo *philo, char *s)
{
	long	now;

	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	now = get_time();
	if (philo->shared->allowed_to_print)
		printf("%ld %d %s\n", now, philo->philo_i + 1, s);
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	return (SUCCESS);
}

int	fork1(t_philo *philo)
{
	return (philo->philo_i);
}

int	fork2(t_philo *philo)
{
	return ((philo->philo_i + 1) % philo->shared->number_of_philosophers);
}

int	take_forks(t_shared *shared, t_philo *philo, int *forks_in_hand)
{
	if (ft_mutex_lock(&shared->butler))
		return (ERROR);
	if (!shared->forks_2[fork1(philo)] || !shared->forks_2[fork2(philo)])
	{
		if (ft_mutex_unlock(&shared->butler))
			return (ERROR);
		return (SUCCESS);
	}
	if (ft_mutex_lock(&shared->forks[fork1(philo)]))
		return (ERROR);
	shared->forks_2[fork1(philo)] = 0;
	if (checked_print(shared, philo, "has taken a fork"))
		return (ERROR);
	if (ft_mutex_lock(&shared->forks[fork2(philo)]))
		return (ERROR);
	if (checked_print(shared, philo, "has taken a fork"))
		return (ERROR);
	shared->forks_2[fork2(philo)] = 0;
	if (ft_mutex_unlock(&shared->butler))
		return (ERROR);
	*forks_in_hand = 2;
	return (SUCCESS);
}

int	start_activity(enum e_activity new_activity, t_philo *philo)
{
	static char	*words[] = {"is eating", "is sleeping", "is thinking"};
	long		now;
	t_shared	*shared;

	shared = philo->shared;
	now = get_time();
	philo->activity = new_activity;
	if (checked_print(shared, philo, words[philo->activity]))
		return (ERROR);
	philo->activity_started = now;
	return (SUCCESS);
}

int	drop_forks(t_shared *shared, t_philo *philo, int *forks_in_hand)
{
	if (ft_mutex_unlock(&shared->forks[philo->philo_i]))
		return (ERROR);
	if (ft_mutex_unlock(&shared->forks[(philo->philo_i + 1)
				% shared->number_of_philosophers]))
		return (ERROR);
	shared->forks_2[fork1(philo)] = 1;
	shared->forks_2[fork2(philo)] = 1;
	*forks_in_hand = 0;
	return (SUCCESS);
}

int	die(t_shared *shared, t_philo *philo, long now)
{
	shared->one_dead = 1;
	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	if (shared->allowed_to_print)
	{
		shared->allowed_to_print = 0;
		printf("%ld %d died\n", now, philo->philo_i + 1);
	}
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	return (SUCCESS);
}

int	check_death(t_shared *shared, t_philo *philo, long last_ate)
{
	long	now;

	now = get_time();
	if (now > last_ate + shared->time_to_die)
	{
		printf("id: %d, now: %ld last_ate: %ld, ttd: %d, dead_time: %ld\n", philo->philo_i + 1, now,
				last_ate, shared->time_to_die,
				now - (last_ate + shared->time_to_die));
		die(shared, philo, now);
		return (ERROR);
	}
	return (SUCCESS);
}

int	check_thinking(t_shared *shared, t_philo *philo)
{
	if (philo->activity == THINKING)
	{
		if (philo->forks_in_hand != 2)
		{
			if (take_forks(shared, philo, &philo->forks_in_hand))
				return (ERROR);
		}
		else
		{
			if (check_death(shared, philo, philo->last_ate))
				return (ERROR);
			if (start_activity(EATING, philo))
				return (ERROR);
			philo->last_ate = philo->activity_started;
		}
	}
	return (SUCCESS);
}

int	check_sleeping(t_shared *shared, t_philo *philo)
{
	long	now;

	if (philo->activity == SLEEPING)
	{
		now = get_time();
		if (now - philo->activity_started > shared->time_to_sleep)
			if (start_activity(THINKING, philo))
				return (ERROR);
	}
	return (SUCCESS);
}

int	check_eating(t_shared *shared, t_philo *philo)
{
	long	now;

	if (philo->activity == EATING)
	{
		now = get_time();
		if (now - philo->activity_started > shared->time_to_eat)
		{
			if (drop_forks(shared, philo, &philo->forks_in_hand))
				return (ERROR);
			if (start_activity(SLEEPING, philo))
				return (ERROR);
		}
	}
	return (SUCCESS);
}

void init_philo(void *philo_void, t_shared **shared_p, t_philo **philo_p)
{
	t_shared		*shared;
	t_philo			*philo;

	philo = philo_void;
	shared = philo->shared;
	philo->forks_in_hand = 0;
	philo->last_ate = get_time();
	start_activity(THINKING, philo);
	*shared_p = shared;
	*philo_p = philo;
}

void	*start_routine(void *philo_void)
{
	t_shared		*shared;
	t_philo			*philo;

	init_philo(philo_void, &shared, &philo);
	while (!shared->one_dead)
	{
		if (check_death(shared, philo, philo->last_ate))
			break ;
		usleep(1000);
		if (check_death(shared, philo, philo->last_ate))
			break ;
		if (check_thinking(shared, philo))
			break ;
		if (check_sleeping(shared, philo))
			break ;
		if (check_eating(shared, philo))
			break ;
	}
	ft_mutex_unlock(&shared->butler);
	if (philo->forks_in_hand == 2)
		if (drop_forks(shared, philo, &philo->forks_in_hand))
			return (NULL);
	return (NULL);
}

enum e_error	initialize_shared(t_shared *shared, int argc, char **argv)
{
	int i;

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
	shared->forks = malloc(sizeof(*shared->forks)
			* shared->number_of_philosophers);
	shared->forks_2 = malloc(sizeof(*shared->forks_2)
			* shared->number_of_philosophers);
	i = 0;
	while (i < shared->number_of_philosophers)
		shared->forks_2[i++] = 1;
	return (SUCCESS);
}

int	initialize_mutexes(t_shared *shared)
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
	err = ft_mutex_init(&shared->print_butler);
	if (err)
		return (err);
	return (SUCCESS);
}

int	initialize_threads(t_shared *shared, t_philo *philo, pthread_t *threads)
{
	int	i;
	int	err;

	i = 0;
	while (i < shared->number_of_philosophers)
	{
		philo[i].shared = shared;
		philo[i].philo_i = i;
		err = pthread_create(threads + i, NULL, start_routine, &philo[i]);
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
	int			i;
	t_philo		*philo;

	if (argc < 5 || argc > 6)
		return (1);
	if (initialize_shared(&shared, argc, argv))
		return (ERROR);
	threads = malloc(sizeof(pthread_t) * shared.number_of_philosophers);
	philo = malloc(sizeof(*philo) * shared.number_of_philosophers);
	if (initialize_mutexes(&shared))
		return (ERROR);
	shared.allowed_to_print = 1;
	if (initialize_threads(&shared, philo, threads))
		return (ERROR);
	while (!shared.one_dead)
		;
	i = 0;
	usleep(shared.time_to_die * shared.number_of_philosophers * 2);
	while (i < shared.number_of_philosophers)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	free(shared.forks);
	free(shared.forks_2);
	free(philo);
	free(threads);
}
