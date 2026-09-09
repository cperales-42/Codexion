/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_codexion.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:13:29 by caperale          #+#    #+#             */
/*   Updated: 2026/09/09 17:24:08 by caperale         ###   ########.fr       */
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
	char	*scheduler;
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
}	t_simulation_data;

typedef struct s_coder
{
	struct s_simulation_data	*simul_data;
	struct s_dongle				*left_dongle;
	struct s_dongle				*right_dongle;
	long long					*last_compilation_time;
	pthread_t					thread;
	int							index;
	int							is_compiling;
	int							is_refactoring;
	int							is_debugging;
	int							has_burnout;
}	t_coder;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				index;
	int				is_being_used;
}	t_dongle;

t_simulation_data	*init_args(void);
int					put_args_in_simul_data(t_simulation_data *args,
						char **argv);
int					validate_args(char **argv);
t_coder				init_coder(t_simulation_data *data);
t_coder				*init_coders(t_simulation_data *data);
int					ft_list_len(t_coder *coder_list);
void				free_coder_list(t_coder *coder_list, int number_of_coders);
void				*routine(void *arg);
#endif
