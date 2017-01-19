/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rtv1.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/15 00:05:50 by hmartzol          #+#    #+#             */
/*   Updated: 2017/01/19 23:49:20 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RTV1_H
# define RTV1_H

# include <libft.h>
# include <libftx.h>
# include <libftocl.h>

# define OCL_SOURCE_PATH "./scl/test.cl"

# define SW 1000
# define SH 1000

# define AMBIENT 0.1f
# define DIFFUSE 1.0f
# define SPECULAR 1.0f

typedef enum	e_prim_type
{
	SPHERE = 0, PLANE = 1, CONE = 2, CYLINDER = 3, PARABOLOID = 4
}				t_prim_type;

typedef enum	e_color_filter
{
	NONE = 0, SEPIA = 1, GRAYSCALE = 2, CARTOON = 3
}				t_color_filter;

/*
** note: the quaternion part of camera is discarded on GPU side
** (copied but not accessible via struct camera)
*/

typedef struct	s_camera
{
	cl_float4		pos;
	cl_float4		dir;
	cl_float4		up;
	cl_float4		right;
	cl_float4		vpul;
	cl_int2			vp_size;
	float			dist;
	t_quaternion	orientation;
}				t_camera;

typedef struct	s_primitive
{
	t_prim_type		type;
	cl_float4		position;
	cl_float4		direction;
	cl_float4		color;
	cl_float		radius;
	cl_float		ambient;
	cl_float		diffuse;
	cl_float		specular;
}				t_primitive;

typedef struct	s_light
{
	cl_float4		position;
	cl_float4		color;
	cl_float		direct;
}				t_light;

typedef struct	s_argn
{
	cl_int2			screen_size;
	cl_int			nb_objects;
	cl_int			nb_lights;
	cl_float		gamma;
	t_color_filter	filter;
}				t_argn;

cl_float4		*ray_plane(const t_camera *cam, const cl_int2 screen_size);
t_camera		*cam(void);
void			update_kernel_args(void);
t_primitive		**prim(void);
t_argn			*argn(void);
t_light			**lights(void);
char			*readfile(int fd);
char			*strmerge(char *a, char *b);

#endif
