#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void	ft_putstr_fd(char *str, int fd)
{
	int i = 0;
	while(str[i])
		i++;
	write(fd, str, i);
}

void	ft_error(void)
{
	ft_putstr_fd("error!", 2);
	exit(1);
}

void	exec_argv(char **argv, char **env, int *fd, int *pipe_fd)
{
	if (argv && argv[0] && env && fd && pipe_fd)
	{

		if (strcmp(argv[0], "cd") == 0)
		{
			if (argv[1] == NULL || argv[2] != NULL)
				ft_putstr_fd("error: cd: bad arg", 2);
			else if (chdir(argv[1]) != 0)
			{
				ft_putstr_fd("error: cd", 2);
				ft_putstr_fd(argv[1], 2);
			}

		}
		else
		{
			pid_t pid = fork();
			if (pid == -1)
				ft_error();
			if (pid == 0)
			{
				if (*pipe_fd != -1)
					dup2(*pipe_fd, STDIN_FILENO);
				if (fd[1] != -1)
					dup2(fd[1], STDOUT_FILENO);
				if (execve(argv[0], argv, env) == -1)
					ft_error();
				exit(0);
			}
			else
			{
				waitpid(pid, NULL, 0);
				if (*pipe_fd != -1)
				{
					close(*pipe_fd);
					*pipe_fd = -1;
				}
				if (fd[1] != -1)
				{
					close(fd[1]);
					fd[1] = -1;
				}
				if (fd[0] == -1)
				{
					*pipe_fd = fd[0];
					fd[0] = -1;
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
		int pipe_fd = -1;
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
				exec_argv(&(argv[start]), env, fd, &pipe_fd);
				start = i + 1;
			}
			i++;
		}
		exec_argv(&(argv[start]), env, fd, &pipe_fd);
	}
}
