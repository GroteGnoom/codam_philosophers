/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   activities.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:45:40 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/27 11:37:59 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

int	check_thinking(t_shared *shared, t_philo *philo)
{
	if (philo->activity == THINKING)
	{
		if (philo->forks_in_hand != 2)
		{
			if (take_forks(shared, philo))
				return (ERROR);
		}
		else
		{
			if (check_death(shared, philo))
				return (ERROR);
			if (start_activity(EATING, philo))
				return (ERROR);
			shared->eaten[philo->philo_i] += 1;
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
			if (drop_forks(shared, philo))
				return (ERROR);
			if (start_activity(SLEEPING, philo))
				return (ERROR);
		}
	}
	return (SUCCESS);
}
