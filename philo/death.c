/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   death.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:44:06 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/26 16:49:48 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

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
		/*
		printf("id: %d, now: %ld last_ate: %ld, ttd: %d, dead_time: %ld\n", philo->philo_i + 1, now,
				last_ate, shared->time_to_die,
				now - (last_ate + shared->time_to_die));
				*/
		die(shared, philo, now);
		return (ERROR);
	}
	return (SUCCESS);
}

int	check_everybody_eaten(t_shared *shared)
{
	int	i;

	i = 0;
	while (i < shared->number_of_philosophers)
	{
		if (shared->eaten[i]
				< shared->number_of_times_each_philosopher_must_eat)
			return (SUCCESS);
		i++;
	}
	shared->allowed_to_print = 0;
	shared->one_dead = 1;
	return (ERROR);
}
