/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readfile.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbondoer <pbondoer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/18 10:12:11 by pbondoer          #+#    #+#             */
/*   Updated: 2017/01/19 21:34:17 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <fcntl.h>
#include <unistd.h>
#include "rtv1.h"

/*
** Reads a whole file into a string in memory
*/

char	*readfile(int fd)
{
	int		ret;
	char	*str;
	char	*str2;
	char	buf[4096 + 1];

	if (fd == -1)
		return (NULL);
	str = ft_strnew(0);
	while ((ret = read(fd, buf, 4096)))
	{
		buf[ret] = '\0';
		str2 = ft_strnew(ret);
		ft_strcpy(str2, buf);
		str = strmerge(str, str2);
	}
	if (ret == -1)
	{
		ft_strdel(&str);
		return (NULL);
	}
	return (str);
}
