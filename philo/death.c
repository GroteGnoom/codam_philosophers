/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   death.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 12:09:15 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 13:44:35 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>
#include <stdio.h>

void	stop(t_shared *shared, t_philo *philo, int dead, long tod)
{
	int	i;

	i = 0;
	while (i < shared->nr_of_philos)
	{
		pthread_join(philo[i++].thread, NULL);
	}
	if (dead)
		printf("%ld %d %s\n", tod, dead, "died");
}

void	check_death_or_eaten(t_shared *shared, t_philo *philo)
{
	int			i;
	static int	dead = 0;
	static int	all_eaten = 0;
	long		tod;

	while (!dead && !all_eaten)
	{
		all_eaten = 1;
		i = 0;
		while (!dead && i < shared->nr_of_philos)
		{
			if (check(&philo[i].last_ate) < get_time() - shared->time_to_die)
			{
				dead = i + 1;
				set(&shared->stop, 1);
				tod = get_time();
			}
			if (all_eaten && (shared->min_eats < 0
					|| (check(&philo[i].eaten) < shared->min_eats)))
				all_eaten = 0;
			i++;
		}
		usleep(1000);
	}
	stop(shared, philo, dead, tod);
}
