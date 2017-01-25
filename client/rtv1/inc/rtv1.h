/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rtv1.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/15 00:05:50 by hmartzol          #+#    #+#             */
/*   Updated: 2017/01/24 11:21:49 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RTV1_H
# define RTV1_H

# include <libft.h>
# include <libftx.h>
# include <libftocl.h>
# include <ft_json.h>

# define OCL_SOURCE_PATH "./scl/test.cl"

# define SW 1000
# define SH 1000

typedef enum	e_prim_type
{
	SPHERE = 0, PLANE = 1, CONE = 2, CYLINDER = 3, PARABOLOID = 4
}				t_prim_type;

typedef enum	e_perturbation_type
{
	NONE = 0, SINE = 1, CHECKERBOARD = 2
}				t_perturbation_type;

/*
** note: the quaternion part of camera is discarded on GPU side
** (copied but not accessible via struct camera)
*/

typedef struct	s_perturbation
{
	t_perturbation_type	normal;
	t_perturbation_type	color;
}				t_perturbation;

typedef struct	s_texture
{
	cl_ulong	info_index;
	cl_float2	stretch;
}				t_texture;

typedef struct	s_material
{
	cl_float4		color;
	cl_float		diffuse;
	cl_float		specular;
	cl_float		alpha;
	cl_float		refraction;
	t_perturbation	perturbation;
	t_texture		texture;
	t_texture		bumpmap;
}				t_material;

typedef struct	s_img_info
{
	cl_ulong	index;
	cl_int2		size;
}				t_img_info;

typedef struct	s_pair
{
	char			*name;
	cl_ulong		index;
}				t_pair;

typedef struct	s_camera
{
	cl_float4		pos;
	cl_float4		dir;
	cl_float4		up;
	cl_float4		right;
	cl_float4		vpul;
	cl_float2		vp_size;
	float			dist;
	t_quaternion	orientation;
}				t_camera;

typedef struct	s_limit
{
	cl_int		relative;
	cl_float4	high;
	cl_float4	low;
}				t_limit;

typedef struct	s_primitive
{
	t_prim_type		type;
	cl_float4		position;
	cl_float4		direction;
	cl_float4		color;
	cl_float		radius;
	cl_uint			material;
	t_limit			limit;
}				t_primitive;

typedef struct	s_light
{
	cl_float4		position;
	cl_float4		color;
}				t_light;

typedef struct	s_argn
{
	cl_int2			screen_size;
	cl_int			nb_objects;
	cl_int			nb_lights;
	cl_float		ambient;
	cl_float		direct;
	cl_float		global;
	cl_int			global_illumination_samples;
	cl_int			antialias;
	cl_int			bounce_depth;
	cl_int			filter;
	cl_int			stereoscopy;
}				t_argn;

cl_float4		*ray_plane(const t_camera *cam, const cl_int2 screen_size);
t_camera		*cam(void);
void			update_kernel_args(void);
t_primitive		**prim(void);
t_argn			*argn(void);
t_light			**lights(void);
void			parser(const char *src);
cl_float4		cl_vector_from_json_array(t_json_value *node,
										cl_float4 default_return);

#endif
