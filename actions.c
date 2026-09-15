/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: caperale <caperale@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 13:34:59 by caperale          #+#    #+#             */
/*   Updated: 2026/09/15 21:06:07 by caperale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_codexion.h"

void	compile(t_coder *coder)
{
	coder->is_compiling = 1;
	log_state(coder, "is compiling");
	sleep_ms(coder->simul_data->time_to_compile);
	coder->is_compiling = 0;
}

void	refactor(t_coder *coder)
{
	coder->is_refactoring = 1;
	log_state(coder, "is refactoring");
	sleep_ms(coder->simul_data->time_to_refactor);
	coder->is_refactoring = 0;
}

void	debug(t_coder *coder)
{
	coder->is_debugging = 1;
	log_state(coder, "is debugging");
	sleep_ms(coder->simul_data->time_to_debug);
	coder->is_debugging = 0;
}

void	burn_out(t_coder *coder)
{
	coder->has_burnout = 1;
	log_state(coder, "has burned out");
	coder->simul_data->simulation_over = 1;
}
