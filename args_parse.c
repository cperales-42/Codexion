/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_parse.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 12:42:53 by caperale          #+#    #+#             */
/*   Updated: 2026/09/08 22:22:05 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

int	validate_args(char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (i == 1 && !(strcmp(argv[i], "fifo") || strcmp(argv[i], "edf")))
			return (0);
		else if (argv[i] < 0 && i != 1)
			return (0);
		i++;
	}
	return (1);
}

t_simulation_data	*init_args(void)
{
	t_simulation_data	*data;

	data = (t_simulation_data *)malloc(sizeof(t_simulation_data));
	if (!data)
		return (free(data), NULL);
	data->number_of_coders = 0;
	data->time_to_burnout = 0;
	data->time_to_compile = 0;
	data->time_to_debug = 0;
	data->time_to_refactor = 0;
	data->number_of_compiles_required = 0;
	data->dongle_cooldown = 0;
	data->scheduler = "fifo";
	return (data);
}

int	put_args_in_simul_data(t_simulation_data *args, char **argv)
{
	int	i;

	i = 1;
	while (argv[i + 1])
	{
		if (atoi(argv[i]) <= 0)
		{
			printf("Argv number %d is negative or zero\n", i);
			free(args);
			return (0);
		}
		i++;
	}
	args->number_of_coders = atoi(argv[1]);
	args->time_to_burnout = atoi(argv[2]);
	args->time_to_compile = atoi(argv[3]);
	args->time_to_debug = atoi(argv[4]);
	args->time_to_refactor = atoi(argv[5]);
	args->number_of_compiles_required = atoi(argv[6]);
	args->dongle_cooldown = atoi(argv[7]);
	args->scheduler = argv[8];
	return (1);
}
