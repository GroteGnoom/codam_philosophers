/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   death.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:44:06 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 14:02:09 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

static int	die(t_shared *shared, t_philo *philo)
{
	shared->one_dead = 1;
	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	if (shared->allowed_to_print)
	{
		shared->allowed_to_print = 0;
		printf("%ld %d died\n", get_time(), philo->philo_i + 1);
	}
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	return (SUCCESS);
}

int	check_death(t_shared *shared, t_philo *philo)
{
	long	now;

	now = get_time();
	if (now > philo->last_ate + shared->time_to_die)
	{
		die(shared, philo);
		return (ERROR);
	}
	return (SUCCESS);
}

int	check_everybody_eaten(t_shared *shared)
{
	int	i;

	if (shared->nr_of_times_each_philo_must_eat == -1)
		return (SUCCESS);
	i = 0;
	while (i < shared->nr_of_philos)
	{
		if (shared->eaten[i]
			< shared->nr_of_times_each_philo_must_eat)
			return (SUCCESS);
		i++;
	}
	shared->allowed_to_print = 0;
	shared->one_dead = 1;
	return (ERROR);
}
