/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 13:57:06 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:50:23 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	all_compiled(t_coder **coders)
{
	int	i;
	int	all_compiled;

	i = 0;
	all_compiled = 1;
	while (coders[i])
	{
		pthread_mutex_lock(&coders[i]->mutex);
		if (coders[i]->compile_count
			< coders[i]->simul_data->number_of_compiles_required)
			all_compiled = 0;
		pthread_mutex_unlock(&coders[i]->mutex);
		i++;
	}
	return (all_compiled);
}

int	coder_burned_out(t_coder *coder)
{
	int			count;
	long long	last;

	pthread_mutex_lock(&coder->mutex);
	count = coder->compile_count;
	last = coder->last_compilation_time;
	pthread_mutex_unlock(&coder->mutex);
	return (burned_out_aux(coder,
			coder->simul_data->number_of_compiles_required,
			count, last));
}

void	make_coders_ready(t_coder **coders)
{
	int	i;

	i = 0;
	while (coders[i])
	{
		coders[i]->ready = 1;
		i++;
	}
}

void	routine_aux(t_coder **coder_list)
{
	pthread_mutex_lock(&coder_list[0]->simul_data->start_mutex);
	monitor_cond_wait(coder_list[0]);
	coder_list[0]->simul_data->start_time = get_time_in_ms();
	make_coders_ready(coder_list);
	pthread_cond_broadcast(&coder_list[0]->simul_data->start_cond);
	pthread_mutex_unlock(&coder_list[0]->simul_data->start_mutex);
}

void	*monitor_routine(void *args)
{
	t_coder	**coders;
	int		i;

	coders = (t_coder **)args;
	if (coders[0]->simul_data->number_of_coders == 1)
		one_coder_case(coders[0]);
	else
	{
		routine_aux(coders);
		while (!all_compiled(coders) && !sim_is_over(coders[0]->simul_data))
		{
			i = 0;
			while (coders[i])
			{
				if (coder_burned_out(coders[i]))
				{
					burn_out(coders[i]);
					break ;
				}
				i++;
			}
			usleep(1000);
		}
	}
	return (NULL);
}
