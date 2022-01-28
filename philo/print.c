/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   print.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/27 11:26:06 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 11:39:30 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>
#include <unistd.h>

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	print_error(char *str)
{
	write(2, str, ft_strlen(str));
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

