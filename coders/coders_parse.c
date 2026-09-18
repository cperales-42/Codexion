/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_parse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:29:25 by caperale          #+#    #+#             */
/*   Updated: 2026/09/18 17:38:36 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	free_coder_list(t_coder	**coder_list)
{
	int	i;

	i = 0;
	while (coder_list[i])
	{
		if (coder_list[i]->right_dongle)
		{
			pthread_mutex_destroy(&coder_list[i]->right_dongle->mutex);
			free(coder_list[i]->right_dongle);
		}
		free(coder_list[i]);
		i++;
	}
	free(coder_list);
}

void	add_dongles_to_coders(t_coder	**coder_list)
{
	int	i;

	i = 0;
	while (coder_list[i])
	{
		coder_list[i]->right_dongle = (t_dongle *)malloc(sizeof(t_dongle));
		if (!coder_list[i]->right_dongle)
		{
			free_coder_list(coder_list);
			break ;
		}
		coder_list[i]->right_dongle->index = i + 1;
		coder_list[i]->right_dongle->is_being_used = 0;
		pthread_mutex_init(&coder_list[i]->right_dongle->mutex, NULL);
		heap_init(coder_list[i]->right_dongle);
		coder_list[i]->right_dongle->last_release_ms = 0;
		coder_list[i]->right_dongle->arrival_counter = 0;
		if (i > 0)
			coder_list[i]->left_dongle = coder_list[i - 1]->right_dongle;
		i++;
	}
	if (i != 0 && coder_list)
		coder_list[0]->left_dongle = coder_list[i - 1]->right_dongle;
}

t_coder	*init_coder(t_simulation_data *data)
{
	t_coder	*coder;

	coder = (t_coder *)malloc(sizeof(t_coder));
	if (!coder)
		return (NULL);
	coder->simul_data = data;
	coder->last_compilation_time = 0;
	coder->ready = 0;
	coder->index = 0;
	coder->is_compiling = 0;
	coder->has_dongles = 0;
	coder->is_refactoring = 0;
	coder->is_debugging = 0;
	coder->has_burnout = 0;
	coder->has_started = 0;
	coder->compile_count = 0;
	pthread_mutex_init(&coder->mutex, NULL);
	return (coder);
}

t_coder	**init_coders(t_simulation_data *data)
{
	t_coder	**coder_list;
	int		i;
	int		n_coders;

	i = 0;
	n_coders = data->number_of_coders;
	coder_list = (t_coder **)malloc((n_coders + 1) * sizeof(t_coder *));
	if (!coder_list)
		return (free(coder_list), NULL);
	while (i < n_coders)
	{
		coder_list[i] = init_coder(data);
		if (!coder_list[i])
			return (free(coder_list), NULL);
		coder_list[i]->index = i + 1;
		i++;
	}
	coder_list[i] = NULL;
	add_dongles_to_coders(coder_list);
	return (coder_list);
}
