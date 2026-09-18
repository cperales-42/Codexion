/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_aux.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:31:50 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 17:16:49 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	use_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	heap_pop(coder->left_dongle);
	coder->left_dongle->is_being_used = 1;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	heap_pop(coder->right_dongle);
	coder->right_dongle->is_being_used = 1;
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	coder->has_dongles = 1;
}

void	not_grantable(t_coder *coder)
{
	heap_pop(coder->left_dongle);
	heap_pop(coder->right_dongle);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
}

void	double_dongle_log(t_coder *coder)
{
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
}

void	monitor_cond_wait(t_coder *coder)
{
	while (coder->simul_data->arrived
		< coder->simul_data->number_of_coders)
		pthread_cond_wait(&coder->simul_data->start_cond,
			&coder->simul_data->start_mutex);
}
