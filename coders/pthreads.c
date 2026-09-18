/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pthreads.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:24:22 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 16:17:46 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	destroy_simulation_data(t_simulation_data *data)
{
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->start_mutex);
	pthread_mutex_destroy(&data->sched_mutex);
	pthread_cond_destroy(&data->start_cond);
	pthread_cond_destroy(&data->sched_cond);
}

void	log_state(t_coder *coder, const char *message)
{
	long long	ms;

	pthread_mutex_lock(&coder->simul_data->log_mutex);
	ms = get_time_in_ms() - coder->simul_data->start_time;
	if (coder->simul_data->number_of_coders == 1)
		ms = coder->simul_data->time_to_burnout;
	printf("%lld %d %s\n", ms, coder->index, message);
	pthread_mutex_unlock(&coder->simul_data->log_mutex);
}

void	join_pthreads(t_coder **coders)
{
	int	i;

	i = 0;
	if (coders[0]->simul_data->number_of_coders != 1)
	{
		while (coders[i])
		{
			pthread_join(coders[i]->thread, NULL);
			i++;
		}
	}
}

void	initialize_pthreads(t_coder **coders)
{
	int	i;

	i = 0;
	if (coders[0]->simul_data->number_of_coders != 1)
	{
		while (coders[i])
		{
			pthread_create(&coders[i]->thread, NULL, coder_routine, coders[i]);
			i++;
		}
	}
}
