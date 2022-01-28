/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philo.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: dnoom <marvin@codam.nl>                      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/26 13:27:42 by dnoom         #+#    #+#                 */
/*   Updated: 2022/01/28 11:16:40 by dnoom         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <sys/time.h>

# define FORKS_FOR_ONE 1

typedef struct s_shared {
	int				number_of_philosophers;
	int				number_of_forks;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
	int				allowed_to_print;
	volatile int	one_dead;
	pthread_mutex_t	*forks;
	int				*forks_2;
	pthread_mutex_t	butler;
	pthread_mutex_t	print_butler;
	int				*eaten;
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
	long			last_ate;
	int				forks_in_hand;
}	t_philo;

enum e_error {
	SUCCESS,
	ERROR
};

int		ft_atoi2(const char *str, int *res);

long	timeval_to_ms(struct timeval timeval);
long	get_time(void);

int		ft_mutex_unlock(pthread_mutex_t *mutex);
int		ft_mutex_init(pthread_mutex_t *mutex);
int		ft_mutex_lock(pthread_mutex_t *mutex);

int		take_forks(t_shared *shared, t_philo *philo);
int		drop_forks(t_shared *shared, t_philo *philo);

int		checked_print(t_shared *shared, t_philo *philo, char *s);

int		check_everybody_eaten(t_shared *shared);
int		check_death(t_shared *shared, t_philo *philo);
int		die(t_shared *shared, t_philo *philo, long now);

int		start_activity(enum e_activity new_activity, t_philo *philo);
int		check_thinking(t_shared *shared, t_philo *philo);
int		check_sleeping(t_shared *shared, t_philo *philo);
int		check_eating(t_shared *shared, t_philo *philo);

void	init_philo(void *philo_void, t_shared **shared_p, t_philo **philo_p);
int		initialize_shared(t_shared *shared, int argc, char **argv);
int		initialize_mutexes(t_shared *shared);
int		initialize_threads(t_shared *shared,
			t_philo *philo, pthread_t *threads);
int		parse_args(t_shared *shared, int argc, char **argv);

void	print_death_info(t_philo *philo);
int		print_error(char *str);

void	*start_routine(void *philo_void);
#endif
