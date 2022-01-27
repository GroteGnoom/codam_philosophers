/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   print.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/27 11:26:06 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/27 12:16:07 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

int	print_error(char *str)
{
	printf("%s", str);
	return (1);
}

void	print_death_info(t_philo *philo)
{
	long	now;

	now = get_time();
	printf("id: %d, now: %ld last_ate: %ld, ttd: %d, dead_time: %ld\n",
		philo->philo_i + 1, now,
		philo->last_ate, philo->shared->time_to_die,
		now - (philo->last_ate + philo->shared->time_to_die));
}
