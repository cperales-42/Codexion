/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_codexion.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:13:29 by caperale          #+#    #+#             */
/*   Updated: 2026/09/17 17:34:59 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CODEXION_H
# define FT_CODEXION_H
# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>

typedef struct s_simulation_data
{
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	sched_mutex;
	pthread_cond_t	sched_cond;
	long long		start_time;
	char			*scheduler;
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				simulation_over;
}	t_simulation_data;

typedef struct s_coder
{
	struct s_simulation_data	*simul_data;
	struct s_dongle				*left_dongle;
	struct s_dongle				*right_dongle;
	pthread_mutex_t				init_mutex;
	pthread_cond_t				init_cond;
	long long					last_compilation_time;
	pthread_t					thread;
	int							index;
	int							is_compiling;
	int							is_refactoring;
	int							is_debugging;
	int							has_dongles;
	int							has_burnout;
	int							has_started;
	int							compile_count;
}	t_coder;

typedef struct s_wait_entry
{
	t_coder		*coder;
	long long	priority;
}	t_wait_entry;

typedef struct s_dongle
{
	struct s_wait_entry	wait_heap[2];
	long long			last_release_ms;
	long long			arrival_counter;
	pthread_mutex_t		mutex;
	int					heap_size;
	int					index;
	int					is_being_used;
}	t_dongle;

t_simulation_data	*init_args(void);
int					put_args_in_simul_data(t_simulation_data *args,
						char **argv);
int					validate_args(char **argv);
t_coder				*init_coder(t_simulation_data *data);
t_coder				**init_coders(t_simulation_data *data);
int					ft_list_len(t_coder *coder_list);
void				free_coder_list(t_coder	**coder_list);
void				*coder_routine(void *arg);
void				initialize_pthreads(t_coder **coders);
void				join_pthreads(t_coder **coders);
long long			get_time_in_ms(void);
void				log_state(t_coder *coder, const char *message);
void				debug(t_coder *coder);
void				refactor(t_coder *coder);
void				compile(t_coder *coder);
void				burn_out(t_coder *coder);
void				sleep_ms(int ms, t_simulation_data *data);
int					heap_init(t_dongle *dongle);
void				heap_destroy(t_dongle *dongle);
int					heap_push(t_dongle *dongle, t_coder *coder,
						long long priority);
t_wait_entry		heap_pop(t_dongle *dongle);
t_wait_entry		heap_peek(t_dongle *dongle);
int					acquire_dongles(t_coder *coder);
void				release_dongles(t_coder *coder);
int					is_grantable(t_coder *coder);
void				*monitor_routine(void *args);
#endif
