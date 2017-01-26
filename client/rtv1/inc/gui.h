/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gui.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbondoer <pbondoer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/24 10:36:46 by pbondoer          #+#    #+#             */
/*   Updated: 2017/01/26 03:47:40 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GUI_H
# define GUI_H
# include <libft.h>
# include <libftx.h>

# define GUI_WIDTH			300
# define GUI_FONT_SIZE		9
# define GUI_ITEM_OFFSET_Y	5
# define GUI_ITEM_OFFSET_X	6
# define GUI_ITEM			(GUI_FONT_SIZE + GUI_ITEM_OFFSET_Y * 2)
# define GUI_C_TITLE		0x00FF00
# define GUI_C_AUTHOR		0x00FFFF
# define GUI_C_SELECT		0xFFFFFF
# define GUI_C_NORMAL		0x999999
# define GUI_C_CATEGORY		0xFFFF00
# define GUI_C_SELECT_BG	0x222222
# define ITEM_NORMAL		0
# define ITEM_SELECT		1
# define ITEM_CATEGORY		2

typedef struct	s_gui
{
	t_window	*ftx;
	void		*mlx;
	void		*win;
	t_image		*buffer;
}				t_gui;

t_gui			*gui(void);
void			init_gui(t_window *ftx);
void			draw_gui(void);
#endif
