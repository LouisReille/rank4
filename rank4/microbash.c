/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microbash.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lreille <lreille@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 01:21:11 by lreille           #+#    #+#             */
/*   Updated: 2023/04/11 19:41:03 by lreille          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void	ft_error()
{
	write (1, "error\n", 6);
	exit (1);
}

void	exec_cmd(char **cmd, char **env, int *fd, int *prev)
{
	if (cmd[0] && cmd && env && fd && prev)
	{
		if (strcmp(cmd[0], "cd") == 0)
		{
			if (cmd[1] == NULL || cmd[2] != NULL)
				ft_error();
			if (chdir(cmd[1]) != 0)
				ft_error();
		}
		else
		{
			pid_t pid = fork();
			if (pid == -1)
				ft_error();
			if (pid == 0)
			{
				if (*prev != -1)
					dup2(*prev, STDIN_FILENO);
				if (fd[1] != -1)
					dup2(fd[1], STDOUT_FILENO);
				if (execve(cmd[0], cmd, env) == -1)
					ft_error();
				exit(0);
			}
			else
			{
				waitpid(pid, NULL, 0);
				if (*prev != -1)
				{
					close(*prev);
					*prev = -1;
				}
				if (fd[1] != -1)
				{
					close(fd[1]);
					fd[1] = -1;
				}
				if (fd[0] != -1)
				{
					fd[0] = *prev;
					*prev = -1;
				}
			}
		}
	}
}

int main(int argc, char **argv, char **env)
{
	if (argc > 1)
	{
		int start = 1;
		int fd[2] = {-1, -1};
		int prev = -1;
		int i = 1;
		while (argv[i])
		{
			if (strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0)
			{
				if (strcmp(argv[i], "|") == 0)
				{
					if (pipe(fd) == -1)
						ft_error();
				}
				argv[i] = NULL;
				exec_cmd(&(argv[start]), env, fd, &prev);
				start = i + 1;
			}
			exec_cmd(&(argv[start]), env, fd, &prev);
			i++;
		}
	}
}
