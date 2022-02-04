/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:27:42 by dnoom         #+#    #+#                 */
/*   Updated: 2022/02/04 13:47:58 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <sys/time.h>

# define ALLOC_FAILED "Memory allocation failed.\n"

typedef struct s_mut_int{
	pthread_mutex_t	mut;
	volatile long	i;
}	t_mut_int;

typedef struct s_shared {
	int			nr_of_philos;
	int			time_to_die;
	int			time_to_eat;
	int			time_to_sleep;
	int			min_eats;
	t_mut_int	*forks;
	t_mut_int	stop;
}	t_shared;

enum e_activity {
	EATING,
	SLEEPING,
	THINKING
};

typedef struct s_philo {
	t_shared		*shared;
	int				philo_i;
	enum e_activity	activity;
	long			activity_started;
	t_mut_int		last_ate;
	int				fork1;
	int				fork2;
	t_mut_int		eaten;
	pthread_t		thread;
}	t_philo;

int		ft_atoi2(const char *str, int *res);
long	get_time(void);
void	ft_sleep(t_shared *shared, long time);
int		parse_args(t_shared *shared, int argc, char **argv);
int		print_error(char *str);
void	print(t_philo *philo, char *s);

int		fork1_nr(t_philo *philo);
int		fork2_nr(t_philo *philo);
void	drop_fork(t_mut_int *fork, int *fork_in_hand);
void	try_to_take_fork(t_philo *philo, t_mut_int *fork, int *fork_in_hand);
int		initialize_threads(t_shared *shared, t_philo *philo);
void	*start_routine(void *philo_void);
int		allocate(t_shared *shared, t_philo **philo);
void	check_death_or_eaten(t_shared *shared, t_philo *philo);

long	check(t_mut_int *mi);
void	set(t_mut_int *mi, long i);
void	inc(t_mut_int *mi);

enum e_error {
	SUCCESS,
	ERROR
};

#endif
