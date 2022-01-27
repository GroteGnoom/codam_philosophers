/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:24:29 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/27 12:23:02 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "philo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	checked_print(t_shared *shared, t_philo *philo, char *s)
{
	long	now;

	if (ft_mutex_lock(&shared->print_butler))
		return (ERROR);
	now = get_time();
	if (philo->shared->allowed_to_print)
		printf("%ld %d %s\n", now, philo->philo_i + 1, s);
	if (ft_mutex_unlock(&shared->print_butler))
		return (ERROR);
	return (SUCCESS);
}

void	*start_routine(void *philo_void)
{
	t_shared		*shared;
	t_philo			*philo;

	init_philo(philo_void, &shared, &philo);
	while (!shared->one_dead)
	{
		if (check_everybody_eaten(shared))
			break ;
		if (check_death(shared, philo))
			break ;
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
		if (drop_forks(shared, philo))
			return (NULL);
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
	threads = malloc(sizeof(pthread_t) * shared.number_of_philosophers);
	philo = malloc(sizeof(*philo) * shared.number_of_philosophers);
	if (initialize_mutexes(&shared))
		return (ERROR);
	shared.allowed_to_print = 1;
	if (initialize_threads(&shared, philo, threads))
		return (ERROR);
	while (!shared.one_dead)
		;
	i = 0;
	usleep(shared.time_to_die * shared.number_of_philosophers * 2);
	while (i < shared.number_of_philosophers)
		pthread_join(threads[i++], NULL);
	free_everything(shared, philo, threads);
}
