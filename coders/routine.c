/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:23:07 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:06:53 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	sleep_ms(int ms, t_simulation_data *data)
{
	int	i;

	i = 0;
	while (i < ms && !sim_is_over(data))
	{
		usleep(1000);
		i++;
	}
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;
	long long		ms;

	gettimeofday(&tv, NULL);
	ms = tv.tv_sec * 1000;
	ms += tv.tv_usec / 1000;
	return (ms);
}

void	routine_loop(t_coder *coder)
{
	if (coder->compile_count < coder->simul_data->number_of_compiles_required
		&& acquire_dongles(coder))
	{
		compile(coder);
		release_dongles(coder);
		debug(coder);
		refactor(coder);
	}
}

void	*coder_routine(void *args)
{
	t_coder	*coder;

	coder = (t_coder *)args;
	pthread_mutex_lock(&coder->simul_data->start_mutex);
	coder->simul_data->arrived++;
	if (coder->simul_data->arrived == coder->simul_data->number_of_coders)
		pthread_cond_broadcast(&coder->simul_data->start_cond);
	while (!coder->ready)
		pthread_cond_wait(&coder->simul_data->start_cond,
			&coder->simul_data->start_mutex);
	set_compilation_time(coder);
	pthread_mutex_unlock(&coder->simul_data->start_mutex);
	while (routine_loop_aux(coder))
	{
		routine_loop(coder);
		if (coder->compile_count
			>= coder->simul_data->number_of_compiles_required)
			return (NULL);
		if (coder->simul_data->time_to_burnout
			<= get_time_in_ms() - coder->last_compilation_time)
			burn_out(coder);
	}
	return (NULL);
}
