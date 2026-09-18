/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_aux.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:27 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:50:46 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	routine_loop_aux(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	if (!coder->simul_data->sim_end && !coder->has_burnout)
	{
		pthread_mutex_unlock(&coder->mutex);
		pthread_mutex_unlock(&coder->simul_data->sched_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->mutex);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
	return (0);
}

void	set_compilation_time(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compilation_time = coder->simul_data->start_time;
	pthread_mutex_unlock(&coder->mutex);
}

int	sim_is_over(t_simulation_data *data)
{
	int	r;

	pthread_mutex_lock(&data->sched_mutex);
	r = data->sim_end;
	pthread_mutex_unlock(&data->sched_mutex);
	return (r);
}

int	burned_out_aux(t_coder *coder, int required, int count,
	long long last)
{
	long long	now;
	long long	last_start;
	int			has_dongles;
	int			burnout;

	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	has_dongles = coder->has_dongles;
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
	pthread_mutex_lock(&coder->mutex);
	burnout = coder->has_burnout;
	pthread_mutex_unlock(&coder->mutex);
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
