/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/27 12:05:03 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 11:14:47 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

int	parse_args(t_shared *shared, int argc, char **argv)
{
	int err;

	err = 0;
	err = ft_atoi2(argv[1], &shared->nr_of_philos);
	err |= ft_atoi2(argv[2], &shared->time_to_die);
	err |= ft_atoi2(argv[3], &shared->time_to_eat);
	err |= ft_atoi2(argv[4], &shared->time_to_sleep);
	if (argc == 6)
	{
		err |= ft_atoi2(argv[5], &shared->nr_of_times_each_philo_must_eat);
		if (shared->nr_of_times_each_philo_must_eat < 0)
			return (print_error("nr of times a philo must eat "
					"must be positive if it is given\n"));
	}
	else
		shared->nr_of_times_each_philo_must_eat = -1;
	if (err)
		return (print_error("integer out of range\n"));
	if (shared->nr_of_philos < 1)
		return (print_error("nr of philos must be at least 1\n"));
	if (shared->time_to_die < 0)
		return (print_error("time to die must be positive\n"));
	if (shared->time_to_eat < 0)
		return (print_error("time to eat must be positive\n"));
	if (shared->time_to_sleep < 0)
		return (print_error("time to sleep must be positive\n"));
	return (SUCCESS);
}
