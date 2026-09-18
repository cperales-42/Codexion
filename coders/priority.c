/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 14:04:46 by caperale          #+#    #+#             */
/*   Updated: 2026/09/15 18:13:37 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	heap_init(t_dongle *dongle)
{
	dongle->heap_size = 0;
	return (1);
}

void	heap_destroy(t_dongle *dongle)
{
	dongle->heap_size = 0;
}

int	heap_push(t_dongle *dongle, t_coder *coder, long long priority)
{
	t_wait_entry	tmp;

	if (dongle->heap_size >= 2)
		return (0);
	dongle->wait_heap[dongle->heap_size].coder = coder;
	dongle->wait_heap[dongle->heap_size].priority = priority;
	dongle->heap_size++;
	if (dongle->heap_size == 2)
	{
		if (dongle->wait_heap[0].priority
			> dongle->wait_heap[1].priority)
		{
			tmp = dongle->wait_heap[0];
			dongle->wait_heap[0] = dongle->wait_heap[1];
			dongle->wait_heap[1] = tmp;
		}
	}
	return (1);
}

t_wait_entry	heap_pop(t_dongle *dongle)
{
	t_wait_entry	tmp;

	tmp.coder = NULL;
	tmp.priority = 0;
	if (dongle->heap_size == 0)
		return (tmp);
	else if (dongle->heap_size == 1)
	{
		tmp = dongle->wait_heap[0];
		heap_destroy(dongle);
	}
	else if (dongle->heap_size == 2)
	{
		tmp = dongle->wait_heap[0];
		dongle->wait_heap[0] = dongle->wait_heap[1];
		dongle->heap_size--;
	}
	return (tmp);
}

t_wait_entry	heap_peek(t_dongle *dongle)
{
	t_wait_entry	tmp;

	tmp.coder = NULL;
	tmp.priority = 0;
	if (dongle->heap_size == 0)
		return (tmp);
	return (dongle->wait_heap[0]);
}
