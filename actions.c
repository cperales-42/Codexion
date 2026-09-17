/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 13:34:59 by caperale          #+#    #+#             */
/*   Updated: 2026/09/17 17:36:09 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	compile(t_coder *coder)
{
	if (coder->simul_data->simulation_over)
		return ;
	coder->is_compiling = 1;
	coder->last_compilation_time = get_time_in_ms();
	log_state(coder, "is compiling");
	sleep_ms(coder->simul_data->time_to_compile, coder->simul_data);
	coder->compile_count++;
	coder->is_compiling = 0;
}

void	refactor(t_coder *coder)
{
	if (coder->simul_data->simulation_over)
		return ;
	coder->is_refactoring = 1;
	log_state(coder, "is refactoring");
	sleep_ms(coder->simul_data->time_to_refactor, coder->simul_data);
	coder->is_refactoring = 0;
}

void	debug(t_coder *coder)
{
	if (coder->simul_data->simulation_over)
		return ;
	coder->is_debugging = 1;
	log_state(coder, "is debugging");
	sleep_ms(coder->simul_data->time_to_debug, coder->simul_data);
	coder->is_debugging = 0;
}

void	burn_out(t_coder *coder)
{
	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	if (coder->has_burnout || coder->simul_data->simulation_over)
	{
		pthread_mutex_unlock(&coder->simul_data->sched_mutex);
		return ;
	}
	coder->has_burnout = 1;
	coder->simul_data->simulation_over = 1;
	pthread_cond_broadcast(&coder->simul_data->sched_cond);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
	log_state(coder, "has burned out");
}
