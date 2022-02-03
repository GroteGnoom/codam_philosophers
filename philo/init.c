/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   init.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:47:01 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/03 15:30:07 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>

int	allocate(t_shared *shared, t_philo **philo)
{
	int i;

	shared->forks = malloc(sizeof(*shared->forks) * shared->nr_of_philos);
	if (!shared->forks)
		return (print_error(ALLOC_FAILED));
	i = 0;
	while (i < shared->nr_of_philos)
		if (pthread_mutex_init(&shared->forks[i++].mut, NULL))
			return (print_error(ALLOC_FAILED));
	i = 0;
	while (i < shared->nr_of_philos)
		shared->forks[i++].i = 1;
	*philo = malloc(sizeof(**philo) * shared->nr_of_philos);
	if (!*philo)
		return (print_error(ALLOC_FAILED));
	if (pthread_mutex_init(&shared->stop.mut, NULL))
		return (print_error(ALLOC_FAILED));
	shared->stop.i = 0;
	return (SUCCESS);
}

int initialize_threads(t_shared *shared, t_philo *philo)
{
	int	i;
	int	err;

	i = 0;
	while (i < shared->nr_of_philos)
	{
		philo[i].shared = shared;
		philo[i].philo_i = i;
		philo[i].forks_in_hand = 0;
		set(&philo[i].last_ate, get_time());
		set(&philo[i].eaten, 0);
		err = pthread_create(&philo[i].thread, NULL, start_routine, &philo[i]);
		if (err)
			return (err);
		i++;
	}
	return (SUCCESS);
}
