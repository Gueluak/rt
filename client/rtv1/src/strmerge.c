/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strmerge.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbondoer <pbondoer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/02/21 14:12:07 by pbondoer          #+#    #+#             */
/*   Updated: 2017/01/19 21:33:28 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*strmerge(char *a, char *b)
{
	char	*ret;

	ret = ft_strjoin(a, b);
	ft_strdel(&a);
	ft_strdel(&b);
	return (ret);
}
