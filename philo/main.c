#include "philo.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int	start_activity(enum e_activity new_activity, t_philo *philo)
{
	static char	*words[] = {"is eating", "is sleeping", "is thinking"};
	long		now;

	now = get_time();
	philo->activity = new_activity;
	print(philo, words[new_activity]);
	philo->activity_started = now;
	return (SUCCESS);
}

long	check(t_mut_int *mi)
{
	long	i;

	pthread_mutex_lock(&mi->mut);
	i = mi->i;
	pthread_mutex_unlock(&mi->mut);
	return (i);
}

void	set(t_mut_int *mi, long i)
{
	pthread_mutex_lock(&(mi->mut));
	mi->i = i;
	pthread_mutex_unlock(&(mi->mut));
}

void	inc(t_mut_int *mi)
{
	pthread_mutex_lock(&mi->mut);
	mi->i++;
	pthread_mutex_unlock(&mi->mut);
}


void ft_sleep(t_shared *shared, long time)
{
	long start;
	long now;
	long left;

	start = get_time();
	now = get_time();
	left = time - (now - start);
	while (!check(&shared->stop) && left > 0)
	{
		if (left < 5)
			usleep(left * 1000);
		else
			usleep(left * 700);
		now = get_time();
		left = time - (now - start);
	}
}

void try_to_eat(t_shared *shared, t_philo *philo)
{
	t_mut_int *fork1;
	t_mut_int *fork2;

	fork1 = &shared->forks[fork1_nr(philo)];
	fork2 = &shared->forks[fork2_nr(philo)];
	start_activity(THINKING, philo);
	while (!philo->fork1 || !philo->fork2)
	{
		if (check(&shared->stop))
		{
			print(philo, "stopping");
			return ;
		}
		usleep(100);
		if (!philo->fork1)
			try_to_take_fork(philo, fork1, &philo->fork1);
		if (!philo->fork2)
			try_to_take_fork(philo, fork2, &philo->fork2);
	}
	start_activity(EATING, philo);
	set(&philo->last_ate, get_time());
	inc(&philo->eaten);
	ft_sleep(shared, shared->time_to_eat);
	if (check(&shared->stop))
		return ;
	start_activity(SLEEPING, philo);
	drop_fork(fork1);
	drop_fork(fork2);
	philo->fork1 = 0;
	philo->fork2 = 0;
	ft_sleep(shared, shared->time_to_sleep);
}

void	*start_routine(void *philo_void)
{
	t_shared		*shared;
	t_philo			*philo;

	philo = philo_void;
	shared = philo->shared;
	if (philo->philo_i % 2)
		usleep(1000);
	while (!check(&shared->stop))
	{
		try_to_eat(shared, philo);
	}
	return (NULL);
}

void	free_all(t_shared *shared, t_philo *philo)
{
	free(shared->forks);
	free(philo);
}


void check_death_or_eaten(t_shared *shared, t_philo *philo)
{
	int	i;
	int	dead;
	int	all_eaten;

	while (1)
	{
		dead = 0;
		all_eaten = 1;
		i = 0;
		while (i < shared->nr_of_philos)
		{
			if (check(&philo[i].last_ate) < get_time() - shared->time_to_die)
				dead = i + 1;
			if (all_eaten && (shared->nr_of_times_each_philo_must_eat < 0 || (check(&philo[i].eaten) < shared->nr_of_times_each_philo_must_eat)))
				all_eaten = 0;
			i++;
		}
		if (dead || all_eaten)
			break;
		usleep(1000);
	}
	set(&shared->stop, 1);
	i = 0;
	while (i < shared->nr_of_philos)
	{
		pthread_join(philo[i++].thread, NULL);
	}
	if (dead)
		print(&philo[dead - 1], "died");
}

int main(int argc, char **argv)
{
	t_shared	shared;
	t_philo		*philo;

	setbuf(stdout, NULL);
	if (parse_args(&shared, argc, argv))
		return (ERROR);
	if (allocate(&shared, &philo))
		return (ERROR);
	if (initialize_threads(&shared, philo))
		return (ERROR);
	check_death_or_eaten(&shared, philo);
	free_all(&shared, philo);
}



