/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:24:29 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 11:39:34 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	*start_routine(void *philo_void)
{
	t_shared		*shared;
	t_philo			*philo;

	init_philo(philo_void, &shared, &philo);
	while (!shared->one_dead)
	{
		if (check_everybody_eaten(shared))
			break ;
		usleep(10);
		if (philo->activity == EATING || philo->activity == SLEEPING)
			usleep(1000);
		if (check_death(shared, philo))
			break ;
		if (check_thinking(shared, philo))
			break ;
		if (check_sleeping(shared, philo))
			break ;
		if (check_eating(shared, philo))
			break ;
	}
	ft_mutex_unlock(&shared->butler);
	if (philo->forks_in_hand == 2)
		drop_forks(shared, philo);
	return (NULL);
}

void	free_everything(t_shared shared, t_philo *philo, pthread_t *threads)
{
	free(shared.forks);
	free(shared.forks_2);
	free(shared.eaten);
	free(philo);
	free(threads);
}

int	main(int argc, char **argv)
{
	t_shared	shared;
	pthread_t	*threads;
	int			i;
	t_philo		*philo;

	if (argc < 5 || argc > 6)
		return (print_error("There should be either 4 or 5 arguments\n"));
	if (initialize_shared(&shared, argc, argv))
		return (ERROR);
	threads = malloc(sizeof(pthread_t) * shared.nr_of_philos);
	philo = malloc(sizeof(*philo) * shared.nr_of_philos);
	if (initialize_mutexes(&shared))
		return (ERROR);
	shared.allowed_to_print = 1;
	if (initialize_threads(&shared, philo, threads))
		return (ERROR);
	while (!shared.one_dead)
		;
	i = 0;
	usleep(shared.time_to_die * shared.nr_of_philos * 2);
	while (i < shared.nr_of_philos)
		pthread_join(threads[i++], NULL);
	free_everything(shared, philo, threads);
}
