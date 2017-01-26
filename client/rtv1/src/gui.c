/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gui.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbondoer <pbondoer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/26 00:18:32 by pbondoer          #+#    #+#             */
/*   Updated: 2017/01/26 06:59:03 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rtv1.h"

t_gui		*gui(void)
{
	static t_gui	gui;

	return (&gui);
}

void		init_gui(t_window *ftx)
{
	if (!ftx)
		return;
	gui()->ftx = ftx;
	gui()->mlx = ftx_data()->mlx;
	gui()->win = ftx->win;
	gui()->buffer = ftx->vbuffer;
}

/*
** Draws text at the specified position
*/

void		gui_text(t_point pos, char *text, int color)
{
	if (pos.y + GUI_FONT_SIZE >= SH)
		return ;
	ftx_set_cursor(gui()->buffer, pos.x, pos.y);
	ftx_write(gui()->buffer, text, ft_strlen(text), color);
}

/*
** Draws a filled rectangle
*/

void		gui_rectangle(t_point pos, t_point size, int color)
{
	int		i;

	i = 0;
	while (i < size.y)
	{
		if (pos.y + i >= SH)
			return ;
		ftx_horizontal_line(gui()->buffer, ft_point(pos.x, pos.y + i),
				ft_point(pos.x + size.x - 1, pos.y + i), ft_point(color, color));
		i++;
	}
}

int			item_offset(int reset)
{
	static int i = 0;

	if (reset >= 0)
		i = reset - 1;
	return (++i);
}

int			item_select(int reset)
{
	static int i = 0;

	if (reset >= 0)
		i = reset;
	return (i);
}

void		gui_item(char *text, int type)
{
	int		i;
	int		c;

	i = item_offset(-1);
	if (type == ITEM_SELECT)
	{
		gui_rectangle(ft_point(0, GUI_ITEM * i), ft_point(GUI_WIDTH, GUI_ITEM),
			GUI_C_SELECT_BG);
		c = GUI_C_SELECT;
	}
	else if (type == ITEM_CATEGORY)
	{
		c = GUI_C_CATEGORY;
		i = item_offset(-1);
	}
	else
		c= GUI_C_NORMAL;
	gui_text(ft_point(GUI_ITEM_OFFSET_X, GUI_ITEM_OFFSET_Y + i * GUI_ITEM),
		text, c);
}

void		item_list(char **str, int size)
{
	int i;

	i = 0;
	while (i < size)
	{
		gui_item(str[i], (i == item_select(-1) ? ITEM_SELECT : ITEM_NORMAL));
		i++;
	}
}

void		gui_input()
{

}

void		object_menu()
{
	static char *str[7] = {"Type", "Position", "Radius", "Material",
		"Limit type", "Limit (high)", "Limit (low)"};

	item_list(str, 7);
}

void		vector_menu()
{
	static char	*str[3] = {"X", "Y", "Z"};

	item_list(str, 3);
}

void		texture_menu()
{
	//TODO: Texture selector menu
}

void		material_menu()
{
	static char	*str[8] = {"Color", "Diffuse", "Specular", "Reflection",
		"Perturbation (color)", "Perturbation (normal)", "Texture", "Bump-map"};

	item_list(str, 8);
}

void		camera_menu()
{
	static char	*str[2] = {"Position", "Direction"};

	item_list(str, 2);
}

void		options_menu()
{
	static char	*str[8] = {"Render size", "Ambient light",
		"Direct light", "Global light", "Antialias", "Ray bounce depth",
		"Color filter", "Red-cyan 3D stereoscopy"};

	item_list(str, 8);
}

void		menu()
{
	static char	*str[5] = {"Render options", "Camera", "Materials",
							"Objects", "Lights"};

	item_list(str, 5);
}

void		draw_gui()
{
	gui_text(ft_point(GUI_ITEM_OFFSET_X, GUI_ITEM_OFFSET_Y), "RT (test build)", GUI_C_TITLE);
	gui_text(ft_point(GUI_ITEM_OFFSET_X, GUI_ITEM_OFFSET_Y / 2 + GUI_ITEM), "hmarot / hmartzol / pbondoer / sbouyahi", GUI_C_AUTHOR);
	item_offset(1);
	gui_item("Render", ITEM_CATEGORY);
	options_menu();
	gui_item("Camera", ITEM_CATEGORY);
	camera_menu();
	gui_item("Material", ITEM_CATEGORY);
	material_menu();
	gui_item("Primitive", ITEM_CATEGORY);
	object_menu();
	// refresh the buffer
	ftx_refresh_window(gui()->ftx);
}
