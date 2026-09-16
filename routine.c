/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:23:07 by caperale          #+#    #+#             */
/*   Updated: 2026/09/16 18:14:10 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	sleep_ms(int ms, t_simulation_data *data)
{
	int	i;

	i = 0;
	while (i < ms && !data->simulation_over)
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

void	*coder_routine(void *args)
{
	t_coder	*coder;

	coder = (t_coder *)args;
	while (!coder->simul_data->simulation_over && !coder->has_burnout)
	{
		if (coder->compile_count < coder->simul_data->number_of_compiles_required
			&& acquire_dongles(coder))
		{
			compile(coder);
			release_dongles(coder);
			debug(coder);
			refactor(coder);
		}
		else if (coder->compile_count
		>= coder->simul_data->number_of_compiles_required)
			return (NULL);
		if (coder->simul_data->time_to_burnout
			<= get_time_in_ms() - coder->last_compilation_time)
			{
				burn_out(coder);
				return (NULL);
			}
	}
	return (NULL);
}
