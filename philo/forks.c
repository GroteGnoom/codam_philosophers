/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   forks.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:42:05 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/27 13:29:20 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	fork1(t_philo *philo)
{
	return (philo->philo_i);
}

static int	fork2(t_philo *philo)
{
	return ((philo->philo_i + 1) % philo->shared->nr_of_forks);
}

int	really_take_forks(t_shared *shared, t_philo *philo)
{
	if (philo->forks_in_hand == 0)
	{
		if (ft_mutex_lock(&shared->forks[fork1(philo)]))
			return (ERROR);
		shared->forks_2[fork1(philo)] = 0;
		if (checked_print(shared, philo, "has taken a fork"))
			return (ERROR);
		philo->forks_in_hand = 1;
	}
	if (shared->nr_of_forks > 1)
	{
		if (ft_mutex_lock(&shared->forks[fork2(philo)]))
			return (ERROR);
		if (checked_print(shared, philo, "has taken a fork"))
			return (ERROR);
		shared->forks_2[fork2(philo)] = 0;
		philo->forks_in_hand = 2;
	}
	return (SUCCESS);
}

int	take_forks(t_shared *shared, t_philo *philo)
{
	if (!shared->forks_2[fork1(philo)] || !shared->forks_2[fork2(philo)])
		return (ERROR);
	if (ft_mutex_lock(&shared->butler))
		return (ERROR);
	if (!shared->forks_2[fork1(philo)] || !shared->forks_2[fork2(philo)])
	{
		if (ft_mutex_unlock(&shared->butler))
			return (ERROR);
		return (SUCCESS);
	}
	if (really_take_forks(shared, philo))
		return (ERROR);
	if (ft_mutex_unlock(&shared->butler))
		return (ERROR);
	return (SUCCESS);
}

int	drop_forks(t_shared *shared, t_philo *philo)
{
	if (ft_mutex_unlock(&shared->forks[fork1(philo)]))
		return (ERROR);
	if (ft_mutex_unlock(&shared->forks[fork2(philo)]))
		return (ERROR);
	shared->forks_2[fork1(philo)] = 1;
	shared->forks_2[fork2(philo)] = 1;
	philo->forks_in_hand = 0;
	return (SUCCESS);
}
