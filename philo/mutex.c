/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   mutex.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 12:10:28 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 12:11:28 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	check(t_mut_int *mi)
{
	long	i;

	pthread_mutex_lock(&mi->mut);
	i = mi->i;
	pthread_mutex_unlock(&mi->mut);
	return (i);
}

void	set(t_mut_int *mi, long i)
{
	pthread_mutex_lock(&(mi->mut));
	mi->i = i;
	pthread_mutex_unlock(&(mi->mut));
}

void	inc(t_mut_int *mi)
{
	pthread_mutex_lock(&mi->mut);
	mi->i++;
	pthread_mutex_unlock(&mi->mut);
}
