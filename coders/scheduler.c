/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 14:41:16 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 20:03:04 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

long long	get_priority(t_coder *coder, t_dongle *d)
{
	long long	priority;

	priority = 0;
	if (!strcmp(coder->simul_data->scheduler, "fifo"))
	{
		priority = d->arrival_counter;
		d->arrival_counter++;
	}
	else if (!strcmp(coder->simul_data->scheduler, "edf"))
	{
		priority = (coder->last_compilation_time
				+ coder->simul_data->time_to_burnout);
	}
	else if (!strcmp(coder->simul_data->scheduler, "albrodri"))
	{
		priority = (coder->index * -1);
	}
	return (priority);
}

int	is_grantable(t_coder *coder)
{
	t_dongle	*l_dongle;
	t_dongle	*r_dongle;

	l_dongle = coder->left_dongle;
	r_dongle = coder->right_dongle;
	if (heap_peek(l_dongle).coder == coder
		&& heap_peek(r_dongle).coder == coder
		&& (!l_dongle->is_being_used
			&& !r_dongle->is_being_used)
		&& ((get_time_in_ms() - l_dongle->last_release_ms
				>= coder->simul_data->dongle_cooldown
				&& get_time_in_ms() - r_dongle->last_release_ms
				>= coder->simul_data->dongle_cooldown)
			|| (l_dongle->last_release_ms == 0
				&& r_dongle->last_release_ms == 0)))
		return (1);
	return (0);
}

struct	timespec	create_deadline(void)
{
	struct timespec	deadline;
	long long		now;

	now = get_time_in_ms() + 1;
	deadline.tv_sec = now / 1000;
	deadline.tv_nsec = (now % 1000) * 1000000;
	return (deadline);
}

int	acquire_dongles(t_coder *coder)
{
	struct timespec	deadline;

	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	heap_push(coder->left_dongle,
		coder, get_priority(coder, coder->left_dongle));
	heap_push(coder->right_dongle,
		coder, get_priority(coder, coder->right_dongle));
	while (!is_grantable(coder) && !coder->simul_data->sim_end)
	{
		deadline = create_deadline();
		pthread_cond_timedwait(&coder->simul_data->sched_cond,
			&coder->simul_data->sched_mutex, &deadline);
	}
	if (!is_grantable(coder))
	{
		not_grantable(coder);
		return (0);
	}
	use_dongles(coder);
	if (!coder->simul_data->sim_end)
		double_dongle_log(coder);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
	return (1);
}

void	release_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->simul_data->sched_mutex);
	coder->left_dongle->is_being_used = 0;
	coder->right_dongle->is_being_used = 0;
	coder->left_dongle->last_release_ms = get_time_in_ms();
	coder->right_dongle->last_release_ms = get_time_in_ms();
	coder->has_dongles = 0;
	pthread_cond_broadcast(&coder->simul_data->sched_cond);
	pthread_mutex_unlock(&coder->simul_data->sched_mutex);
}
