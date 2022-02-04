/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   time.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 16:39:20 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 11:58:13 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
#include <unistd.h>

static long	timeval_to_ms(struct timeval timeval)
{
	return ((long)timeval.tv_sec * 1000 + timeval.tv_usec / 1000);
}

long	get_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval_to_ms(timeval));
}

void	ft_sleep(t_shared *shared, long time)
{
	long	start;
	long	now;
	long	left;

	start = get_time();
	now = get_time();
	left = time - (now - start);
	while (!check(&shared->stop) && left > 0)
	{
		if (left < 5)
			usleep(left * 1000);
		else
			usleep(left * 700);
		now = get_time();
		left = time - (now - start);
	}
}
