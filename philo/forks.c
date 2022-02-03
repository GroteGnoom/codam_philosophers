/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   forks.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 11:32:05 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/03 17:19:00 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	fork1_nr(t_philo *philo)
{
	if (philo->philo_i != philo->shared->nr_of_philos - 1)
		return (philo->philo_i);
	return ((philo->philo_i + 1) % philo->shared->nr_of_philos);
}

int	fork2_nr(t_philo *philo)
{
	if (philo->philo_i != philo->shared->nr_of_philos - 1)
		return ((philo->philo_i + 1) % philo->shared->nr_of_philos);
	return (philo->philo_i);
}

void	drop_fork(t_philo *philo, t_mut_int *fork)
{
	pthread_mutex_lock(&fork->mut);
	fork->i = 1;
	pthread_mutex_unlock(&fork->mut);
	philo->forks_in_hand--;
}

void	try_to_take_fork(t_philo *philo, t_mut_int *fork)
{
	pthread_mutex_lock(&fork->mut);
	if (fork->i)
	{
		fork->i = 0;
		philo->forks_in_hand++;
		print(philo, "has taken a fork");
	}
	pthread_mutex_unlock(&fork->mut);
}
