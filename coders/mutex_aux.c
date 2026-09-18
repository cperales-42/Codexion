/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_aux.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:27 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:03:01 by caperale         ###   ########.fr       */
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