/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:18:43 by caperale          #+#    #+#             */
/*   Updated: 2026/09/17 17:17:00 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	print_error_mesage(void)
{
	printf("Invalid parameters. Usage:");
	printf(" ./<program_name> <number_of_coders>");
	printf(" <time_to_burnout> <time_to_compile>");
	printf(" <time_to_debug> <time_to_refactor>");
	printf(" <number_of_compiles_required> <dongle_cooldown>");
	printf(" <scheduler>\n");
}

int	main(int argc, char **argv)
{
	t_simulation_data	*args;
	t_coder				**coders;
	pthread_t			monitor_thread;

	args = init_args();
	if (!args)
		return (free(args), 2);
	if (argc == 9 && validate_args(argv))
	{
		if (!put_args_in_simul_data(args, argv))
			return (1);
		coders = init_coders(args);
		if (!coders)
			return (free(args), 2);
		args->start_time = get_time_in_ms();
		pthread_create(&monitor_thread, NULL, monitor_routine, (void *)coders);
		pthread_join(monitor_thread, NULL);
	}
	else
	{
		print_error_mesage();
		free(args);
	}
	return (0);
}
