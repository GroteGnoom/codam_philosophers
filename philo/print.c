/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   print.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/27 11:26:06 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 13:47:10 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>
#include <unistd.h>

static int	ft_strlen(char *str)
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

void	print(t_philo *philo, char *s)
{
	t_shared	*shared;

	shared = philo->shared;
	pthread_mutex_lock(&shared->stop.mut);
	if (!shared->stop.i)
		printf("%ld %d %s\n", get_time(), philo->philo_i + 1, s);
	pthread_mutex_unlock(&shared->stop.mut);
}
