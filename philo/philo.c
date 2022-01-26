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
	int	allowed_to_print;
	volatile int	one_dead;
	pthread_mutex_t	*forks;
	pthread_mutex_t butler;
	pthread_mutex_t print_butler;
} t_shared;

enum activity {
	EATING,
	SLEEPING,
	THINKING
};

typedef struct s_philo {
	t_shared		*shared;
	int				philo_i;
	enum activity	activity;
	long			activity_started;
	long			last_ate;
	int				forks_in_hand;
} t_philo;

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

int	take_forks(t_shared *shared, t_philo *philo, int *forks_in_hand)
{
	long now;

	if (pthread_mutex_lock(&shared->butler))
		return (ERROR);
	if (ft_mutex_lock(&shared->forks[philo->philo_i]))
		return (ERROR);
	now = get_time();
	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	if (philo->shared->allowed_to_print)
		printf("%ld %d has taken a fork\n", now, philo->philo_i + 1);
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	if (ft_mutex_lock(&shared->forks[(philo->philo_i + 1) % shared->number_of_philosophers]))
		return (ERROR);
	now = get_time();
	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	if (philo->shared->allowed_to_print)
		printf("%ld %d has taken a fork\n", now, philo->philo_i + 1);
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	if (pthread_mutex_unlock(&shared->butler))
		return (ERROR);
	*forks_in_hand = 2;
	return (SUCCESS);
}

int	start_activity(enum activity new_activity, enum activity *activity,
		long *activity_started, t_philo *philo)
{
	static char	*words[] = {"eating", "sleeping", "thinking"};
	long		now;
	t_shared	*shared = philo->shared;

	now=get_time();
	*activity = new_activity;
	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	if (philo->shared->allowed_to_print)
		printf("%ld %d is %s\n", now, philo->philo_i + 1, words[*activity]);
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	*activity_started = now;
	return (SUCCESS);
}

int drop_forks(t_shared *shared, t_philo *philo, int *forks_in_hand)
{
	if (ft_mutex_unlock(&shared->forks[philo->philo_i]))
		return (ERROR);
	if (ft_mutex_unlock(&shared->forks[(philo->philo_i + 1) % shared->number_of_philosophers]))
		return (ERROR);
	*forks_in_hand = 0;
	return (SUCCESS);
}

int die(t_shared *shared, t_philo *philo, long now)
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

int check_death(t_shared *shared, t_philo *philo, long last_ate, enum activity activity)
{
	long	now;

	now = get_time();
	if (activity != EATING && now > last_ate + shared->time_to_die)
	{
		printf("now: %ld last_ate: %ld, ttd: %d, dead_time: %ld\n", now, last_ate, shared->time_to_die, now - (last_ate + shared->time_to_die));
		die(shared, philo, now);
		return (ERROR);
	}
	return (SUCCESS);
}

int check_thinking(t_shared *shared, t_philo *philo, long *activity_started, enum activity *activity, long *last_ate)
{
	if (*activity == THINKING)
	{
		if (philo->forks_in_hand != 2)
			if (take_forks(shared, philo, &philo->forks_in_hand))
				return (ERROR);
		if (start_activity(EATING, activity, activity_started, philo))
			return (ERROR);
		*last_ate = *activity_started;
	}
	return (SUCCESS);
}

void	*start_routine(void *philo_void)
{
	t_shared		*shared;
	t_philo			*philo;
	long			now;

	philo = philo_void;
	shared = philo->shared;
	philo->forks_in_hand = 0;
	now = get_time();
	philo->last_ate = now;
	start_activity(THINKING, &philo->activity, &philo->activity_started, philo);

	printf("%ld %d is thinking\n", now, philo->philo_i + 1);
	/*printf("hi! I'm %d, %d, %d\n", philo->philo_i + 1, shared->number_of_philosophers, shared->time_to_die);*/
	while (!shared->one_dead)
	{
		now = get_time();
		if (check_death(shared, philo, philo->last_ate, philo->activity))
			break;
		usleep(1000);
		if (check_death(shared, philo, philo->last_ate, philo->activity))
			break;
		if (check_thinking(shared, philo, &philo->activity_started, &philo->activity, &philo->last_ate))
			break;
		else if (philo->activity == SLEEPING)
		{
			if (now - philo->activity_started > shared->time_to_sleep)
				start_activity(THINKING, &philo->activity, &philo->activity_started, philo);
		} 
		else if (philo->activity == EATING)
		{
			if (now - philo->activity_started > shared->time_to_eat)
			{
				if (drop_forks(shared, philo, &philo->forks_in_hand))
					return (NULL);
				start_activity(SLEEPING, &philo->activity, &philo->activity_started, philo);
			}
		}
	}
	ft_mutex_unlock(&shared->butler);
	if (philo->forks_in_hand == 2)
		if (drop_forks(shared, philo, &philo->forks_in_hand))
			return (NULL);
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
	int	i;
	t_philo		*philo;
	enum error	err;

	printf("voor arg check\n");
	if (argc < 5 || argc > 6)
		return (1);
	printf("na arg check\n");
	err = initialize_shared(&shared, argc, argv);
	if (err)
		return (err);
	threads = malloc(sizeof(pthread_t) * shared.number_of_philosophers);
	philo = malloc(sizeof(*philo) * shared.number_of_philosophers);
	err = initialize_mutexes(&shared);
	if (err)
		return (err);
	shared.allowed_to_print = 1;
	err = initialize_threads(&shared, philo, threads);
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
	free(philo);
	free(threads);
}

