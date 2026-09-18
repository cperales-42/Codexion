/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 13:57:06 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:07:45 by caperale         ###   ########.fr       */
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
		pthread_mutex_lock(&coders[i]->mutex);
		if (coders[i]->compile_count
			< coders[i]->simul_data->number_of_compiles_required)
			all_compiled = 0;
		pthread_mutex_unlock(&coders[i]->mutex);
		i++;
	}
	return (all_compiled);
}

int	burned_out_aux(t_coder *coder, int required, int count,
    long long last, int burnout)
{
    long long	now;
    long long	last_start;
    int			has_dongles;

    pthread_mutex_lock(&coder->simul_data->sched_mutex);
    has_dongles = coder->has_dongles;
    pthread_mutex_unlock(&coder->simul_data->sched_mutex);

    if (count >= required)
        return (0);
    now = get_time_in_ms();
    if (last == 0)
        last_start = coder->simul_data->start_time;
    else
        last_start = last;
    if (!has_dongles && now - last_start >= coder->simul_data->time_to_burnout)
        return (1);
    return (burnout);
}

int	coder_burned_out(t_coder *coder)
{
    int			count;
    long long	last;
    int			burnout;

    pthread_mutex_lock(&coder->mutex);
    count = coder->compile_count;
    last = coder->last_compilation_time;
    burnout = coder->has_burnout;
    pthread_mutex_unlock(&coder->mutex);

    return (burned_out_aux(coder,
        coder->simul_data->number_of_compiles_required,
        count, last, burnout));
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
		while (!all_have_compiled(coders) && !sim_is_over(coders[0]->simul_data))
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
