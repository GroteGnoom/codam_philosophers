/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 11:55:29 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 12:13:27 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

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

void	try_to_eat(t_shared *shared, t_philo *philo)
{
	t_mut_int	*forks[2];

	forks[0] = &shared->forks[fork1_nr(philo)];
	forks[1] = &shared->forks[fork2_nr(philo)];
	start_activity(THINKING, philo);
	while (!philo->fork1 || !philo->fork2)
	{
		if (check(&shared->stop))
			return ;
		usleep(100);
		if (!philo->fork1)
			try_to_take_fork(philo, forks[0], &philo->fork1);
		if (!philo->fork2)
			try_to_take_fork(philo, forks[1], &philo->fork2);
	}
	start_activity(EATING, philo);
	set(&philo->last_ate, get_time());
	inc(&philo->eaten);
	ft_sleep(shared, shared->time_to_eat);
	if (check(&shared->stop))
		return ;
	start_activity(SLEEPING, philo);
	drop_fork(forks[0], &philo->fork1);
	drop_fork(forks[1], &philo->fork2);
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

int	main(int argc, char **argv)
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
	free(shared.forks);
	free(philo);
}
