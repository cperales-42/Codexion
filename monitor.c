/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 13:57:06 by caperale          #+#    #+#             */
/*   Updated: 2026/09/17 20:11:30 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	all_have_compiled(t_coder **coders)
{
	int	i;
	int	all_compiled;

	i = 0;
	all_compiled = 1;
	while (coders[i])
	{
		if (coders[i]->compile_count
			< coders[i]->simul_data->number_of_compiles_required)
			all_compiled = 0;
		i++;
	}
	return (all_compiled);
}

int	coder_burned_out(t_coder *coder)
{
	long long	last_start;
	long long	now;

	if (coder->compile_count >= coder->simul_data->number_of_compiles_required)
		return (0);
	now = get_time_in_ms();
	if (coder->last_compilation_time == 0)
		last_start = coder->simul_data->start_time;
	else
		last_start = coder->last_compilation_time;
	if (!coder->has_dongles
		&& now - last_start >= coder->simul_data->time_to_burnout)
		return (1);
	return (coder->has_burnout);
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

void	*monitor_routine(void *args)
{
	t_coder	**coder_list;
	int		i;

	coder_list = (t_coder **)args;
	pthread_mutex_lock(&coder_list[0]->simul_data->start_mutex);
	while (coder_list[0]->simul_data->arrived < coder_list[0]->simul_data->number_of_coders)
		pthread_cond_wait(&coder_list[0]->simul_data->start_cond, &coder_list[0]->simul_data->start_mutex);
	coder_list[0]->simul_data->start_time = get_time_in_ms();
	make_coders_ready(coder_list);
	pthread_cond_broadcast(&coder_list[0]->simul_data->start_cond);
	pthread_mutex_unlock(&coder_list[0]->simul_data->start_mutex);
	while (!all_have_compiled(coder_list) && !coder_list[0]->simul_data->simulation_over && coder_list[0]->simul_data->arrived != coder_list[0]->simul_data->number_of_coders)
	{
		i = 0;
		while (coder_list[i])
		{
			if (coder_burned_out(coder_list[i]))
			{
				burn_out(coder_list[i]);
				break ;
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
