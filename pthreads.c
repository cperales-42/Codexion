/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pthreads.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:24:22 by caperale          #+#    #+#             */
/*   Updated: 2026/09/11 13:27:46 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	log_state(t_coder *coder, const char *message)
{
	long long	ms;

	pthread_mutex_lock(&coder->simul_data->log_mutex);
	ms = get_time_in_ms() - coder->simul_data->start_time;
	printf("%lld %d %s\n", ms, coder->index, message);
	pthread_mutex_unlock(&coder->simul_data->log_mutex);
}

void	join_pthreads(t_coder **coders)
{
	int	i;

	i = 0;
	while (coders[i])
	{
		pthread_join(coders[i]->thread, NULL);
		i++;
	}
}

void	initialize_pthreads(t_coder **coders)
{
	int	i;

	i = 0;
	while (coders[i])
	{
		pthread_create(&coders[i]->thread, NULL, coder_routine, coders[i]);
		i++;
	}
}
