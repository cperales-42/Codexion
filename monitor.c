/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 13:57:06 by caperale          #+#    #+#             */
/*   Updated: 2026/09/16 14:40:08 by caperale         ###   ########.fr       */
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

void	*monitor_routine(void *args)
{
	t_coder	**coder_list;
	int	i;

	coder_list = (t_coder **)args;
	while (!all_have_compiled(coder_list) && !coder_list[0]->simul_data->simulation_over)
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