/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_aux.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:27 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 17:59:46 by caperale         ###   ########.fr       */
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
