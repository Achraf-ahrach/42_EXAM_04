/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aahrach <aahrach@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/26 10:20:47 by aahrach           #+#    #+#             */
/*   Updated: 2023/06/26 11:06:58 by aahrach          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int ft_printf(char *str, char *arg)
{
    while (*str)
        write(2, str++, 1);
    if (arg)
        while(*arg)
            write (2, arg++, 1);
    write (2, "\n", 1);
    return (1);
}

int ft_execve(char **av, int i, int fd_tmp, char **env)
{
    av[i] = NULL;
    dup2(fd_tmp, 0);
    close(fd_tmp);
    execve(av[0], av, env);
    ft_printf("error: cannot execute ", av[0]);
    return (1);
}

int main(int ac, char **av, char **env)
{
    int i;
    int pip[2];
    int tmp_fd;

    i = 0;
    (void)ac;
    tmp_fd = dup(0);
    while(av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
            i++;
        if (!strcmp(av[0], "cd"))
        {
            if (i < 2)
                ft_printf("error: cd: bad arguments", NULL);
            else if (chdir(av[1]) == -1)
                ft_printf("error: cd: cannot change directory to ", av[1]);
        }
        else if (i != 0 && (av[i] == NULL || !strcmp(av[i], ";")))
        {
            if (!fork())
            {
                if (ft_execve(av, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(tmp_fd);
                waitpid(-1, NULL, 0);
                tmp_fd = dup(0);
            }
        }
        else if (i != 0 && !strcmp(av[i], "|"))
        {
            pipe(pip);
            if (!fork())
            {
                dup2(pip[1], 1);
                close(pip[1]);
                close(pip[0]);
                if (ft_execve(av, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(pip[1]);
                close(tmp_fd);
                tmp_fd = pip[0];
            }
        }
    }
    close(tmp_fd);
    return (0);
}