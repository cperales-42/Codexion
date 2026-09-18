# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: caperale <caperale@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/07 12:00:44 by caperale          #+#    #+#              #
#    Updated: 2026/09/18 17:32:41 by caperale         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc 
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm 
RMFLAGS = -f
NAME = codexion
SOURCES = main.c args_parse.c coders_parse.c routine.c pthreads.c actions.c priority.c scheduler.c monitor.c scheduler_aux.c mutex_aux.c

OBJECTS = $(SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)

re: fclean all

sanitize: fclean
	$(CC) $(CFLAGS) -fsanitize=thread -g $(SOURCES) -o $(NAME)

.PHONY: all clean fclean re sanitize
