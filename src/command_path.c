/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvalerio <mvalerio@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 12:59:53 by maggie            #+#    #+#             */
/*   Updated: 2024/03/27 19:01:51 by mvalerio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/pipex.h"

// Looks for the PATH variable in the environment variables and returns all
//the paths in an array of strings.
char	**path_list(char *envp[])
{
	int		i;
	char	**paths;

	i = 0;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], "PATH", 5))
			break ;
		i++;
	}
	if (!envp[i])
		return (NULL);
	paths = ft_split(&(envp[i][5]), ':');
	return (paths);
}

// Finds the path to the command in the PATH variable. If the command is not
//found, it returns NULL.
char	*find_cmd_path(char *envp[], char *cmd, int i)
{
	char	**paths;
	char	*final_path;

	if (cmd == NULL)
		return (NULL);
	final_path = ft_absolute_path(cmd);
	if (final_path != NULL)
		return (final_path);
	cmd = ft_strjoin("/", cmd);
	paths = path_list(envp);
	while (paths[i])
	{
		final_path = ft_strjoin(paths[i], cmd);
		if (access(final_path, X_OK) == 0)
		{
			ft_free_strarray(paths);
			free (cmd);
			return (final_path);
		}
		i++;
		free(final_path);
	}
	ft_free_strarray(paths);
	free (cmd);
	return (NULL);
}

// Checks if the path to the command is valid. If not, it frees the linked
//list (args) and exits the program with an error message.
void	ft_invalid_path(t_args *args)
{
	t_args	*current;

	current = args->head;
	while (current)
	{
		if (!(current->path))
		{
			ft_printf("%s: command not found\n", \
				current->cmd_and_flags[0]);
			current->skip_command = 1;
		}
		else
			current->skip_command = 0;
		current = current->next;
	}
}

// Adds each new argument to the linked list of commands (args).
void	ft_add_argument_to_list(t_args *current, t_args *args)
{
	t_args	*temp;

	if (!(args->head))
		args->head = current;
	else
	{
		temp = args->head;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = current;
	}
}

// Sets a linked list (args) of all the commands that the program is going
//to execute. In this list, there is the path to the command ,a string), and
// the command and flags, an array of strings.
void	ft_set_arguments(t_args *args, int argc, char *argv[], char *envp[])
{
	t_args	*current;
	int		i;

	i = 2;
	args->head = NULL;
	while (i < argc - 1)
	{
		current = malloc(sizeof(t_args));
		if (!current)
			return ;
		current->skip_command = 0;
		if (i == 2 && args->infile_invalid == 1)
			current->skip_command = 1;
		current->next = NULL;
		current->cmd_and_flags = ft_split(argv[i], ' ');
		current->path = find_cmd_path(envp, current->cmd_and_flags[0], 0);
		current->pid = -5;
		ft_add_argument_to_list(current, args);
		i++;
	}
}
