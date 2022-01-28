/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_atoi.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:50:40 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 11:08:52 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "philo.h"
#include <limits.h>
static int	ft_isspace(char c)
{
	if (c == ' '
		|| c == '\t'
		|| c == '\n'
		|| c == '\v'
		|| c == '\f'
		|| c == '\r')
		return (1);
	return (0);
}

int	ft_atoi(const char *str)
{
	int		sign;
	long	res;

	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	res = 0;
	while (*str >= '0' && *str <= '9')
	{
		res *= 10;
		res += (*str - '0');
		str++;
	}
	res *= sign;
	return (res);
}

int	ft_atoi2(const char *str, int *res)
{
	long	sign;
	long	l;

	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '-')
		sign = -1;
	if (*str == '+' || *str == '-')
		str++;
	l = 0;
	while (*str >= '0' && *str <= '9')
	{
		l = l * 10 + (*str - '0');
		str++;
		if ((sign == -1 && l > (long)INT_MAX + 1) || (sign == 1 && l > INT_MAX))
			return (ERROR);
	}
	if (*str)
		return (ERROR);
	l *= sign;
	*res = l;
	return (SUCCESS);
}

