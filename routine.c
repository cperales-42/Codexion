/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:23:07 by caperale          #+#    #+#             */
/*   Updated: 2026/09/11 13:40:02 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	sleep_ms(int ms)
{
	int	i;

	i = 0;
	while (i < ms)
	{
		usleep(1000);
		i++;
	}
}

long long	get_time_in_ms(void)
{
	struct	timeval	tv;
	long long	ms;

	gettimeofday(&tv, NULL);
	ms = tv.tv_sec * 1000;
	ms += tv.tv_usec / 1000;
	return (ms);
}

void       *coder_routine(void *args)
{
    t_coder *coder;

    coder = (t_coder *)args;
    while (1)
    {
        compile(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
