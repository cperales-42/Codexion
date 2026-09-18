/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 13:34:59 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:09:26 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	compile(t_coder *coder)
{
	if (sim_is_over(coder->simul_data))
		return ;
	pthread_mutex_lock(&coder->mutex);
	coder->is_compiling = 1;
	coder->last_compilation_time = get_time_in_ms();
	pthread_mutex_unlock(&coder->mutex);
	log_state(coder, "is compiling");
	sleep_ms(coder->simul_data->time_to_compile, coder->simul_data);
	pthread_mutex_lock(&coder->mutex);
	coder->is_compiling = 0;
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
}

void	refactor(t_coder *coder)
{
	if (sim_is_over(coder->simul_data))
		return ;
	pthread_mutex_lock(&coder->mutex);
	coder->is_refactoring = 1;
	pthread_mutex_unlock(&coder->mutex);
	log_state(coder, "is refactoring");
	sleep_ms(coder->simul_data->time_to_refactor, coder->simul_data);
	pthread_mutex_lock(&coder->mutex);
	coder->is_refactoring = 0;
	pthread_mutex_unlock(&coder->mutex);
}

void	debug(t_coder *coder)
{
	if (sim_is_over(coder->simul_data))
		return ;
	pthread_mutex_lock(&coder->mutex);
	coder->is_debugging = 1;
	pthread_mutex_unlock(&coder->mutex);
	log_state(coder, "is debugging");
	sleep_ms(coder->simul_data->time_to_debug, coder->simul_data);
	pthread_mutex_lock(&coder->mutex);
	coder->is_debugging = 0;
	pthread_mutex_unlock(&coder->mutex);
}

void	burn_out(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	if (coder->has_burnout || sim_is_over(coder->simul_data))
	{
		pthread_mutex_unlock(&coder->mutex);
		return ;
	}
	coder->has_burnout = 1;
	pthread_mutex_unlock(&coder->mutex);
	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	if (coder->simul_data->sim_end)
	{
		pthread_mutex_unlock(&coder->simul_data->sched_mutex);
		return ;
	}
	coder->simul_data->sim_end = 1;
	pthread_cond_broadcast(&coder->simul_data->sched_cond);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
	log_state(coder, "has burned out");
}
