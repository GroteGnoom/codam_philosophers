/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   init.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:47:01 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/27 13:15:07 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>

void	init_philo(void *philo_void, t_shared **shared_p, t_philo **philo_p)
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

static void	alloc_shared(t_shared *shared)
{
	shared->forks = malloc(sizeof(*shared->forks)
			* shared->number_of_forks);
	shared->forks_2 = malloc(sizeof(*shared->forks_2)
			* shared->number_of_forks);
	shared->eaten = malloc(sizeof(*shared->eaten)
			* shared->number_of_philosophers);
	if (!shared->forks || !shared->forks_2 || !shared->eaten)
		exit(1);
}

int	initialize_shared(t_shared *shared, int argc, char **argv)
{
	int	i;

	if (parse_args(shared, argc, argv))
		return (ERROR);
	shared->number_of_forks = shared->number_of_philosophers;
	if (shared->number_of_philosophers == 1)
		shared->number_of_forks = FORKS_FOR_ONE;
	shared->one_dead = 0;
	alloc_shared(shared);
	i = 0;
	while (i < shared->number_of_philosophers)
		shared->eaten[i++] = 0;
	i = 0;
	while (i < shared->number_of_forks)
		shared->forks_2[i++] = 1;
	return (SUCCESS);
}

int	initialize_mutexes(t_shared *shared)
{
	int	i;
	int	err;

	i = 0;
	while (i < shared->number_of_forks)
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
