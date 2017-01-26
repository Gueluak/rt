/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/09 09:15:54 by hmartzol          #+#    #+#             */
/*   Updated: 2017/01/26 02:19:50 by pbondoer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rtv1.h>
#include <stdio.h>
#include "gui.h"

int			cb_exit(int k, int s, void *p)
{
	(void)k;
	(void)p;
	if (s == FTX_KEY_STATUS_PRESSED)
		ft_end(0);
	return (0);
}

t_light	**lights(void)
{
	static t_light	*data = NULL;

	return (&data);
}

t_primitive	cone(cl_float4 pos, cl_float4 direction, cl_float alpha, cl_float4 color)
{
	return ((t_primitive){.type = CONE, .position = pos,
		.direction = direction, .radius = alpha * M_PI / 180.0f, .color = color,
		.ambient = AMBIENT, .diffuse = DIFFUSE, .specular = SPECULAR,
		.normal_perturbation = 0.0f, .perturbation = NONE, .reflection = 1.0f,
		.refraction = 0.0f, .r_index = 0.0f});
}

t_primitive	cylinder(cl_float4 pos, cl_float4 direction, cl_float radius, cl_float4 color)
{
	return ((t_primitive){.type = CYLINDER, .position = pos,
		.direction = direction, .radius = radius, .color = color,
		.ambient = AMBIENT, .diffuse = DIFFUSE, .specular = SPECULAR,
		.normal_perturbation = 0.0f, .perturbation = NONE, .reflection = 1.0f,
		.refraction = 0.0f, .r_index = 0.0f});
}

t_primitive	sphere(cl_float4 pos, cl_float radius, cl_float4 color)
{
	return ((t_primitive){.type = SPHERE, .position = pos,
		.direction = {.x = 0, .y = 0, .z = 1, .w = 0}, .radius = radius,
		.color = color, .ambient = AMBIENT, .diffuse = DIFFUSE,
		.specular = SPECULAR, .normal_perturbation = 0.0f,
		.perturbation = NONE, .reflection = 1.0f, .refraction = 0.0f,
		.r_index = 1.00f, .transparency = 0.0f});
}

t_primitive	plane(cl_float4 pos, cl_float4 norm, cl_float4 color)
{
	return ((t_primitive){.type = PLANE, .position = pos, .direction = norm,
		.radius = 0, .color = color, .ambient = AMBIENT, .diffuse = DIFFUSE,
		.specular = SPECULAR, .normal_perturbation = 0.0f,
		.perturbation = NONE, .reflection = 1.0f, .refraction = 0.0f,
		.r_index = 0.0f, .transparency = 0.0f});
}

t_primitive	paraboloid(cl_float4 pos, cl_float4 norm, cl_float value, cl_float4 color)
{
	return ((t_primitive){.type = PARABOLOID, .position = pos, .direction = norm,
		.radius = value, .color = color, .ambient = AMBIENT, .diffuse = DIFFUSE,
		.specular = SPECULAR, .normal_perturbation = 0.0f,
		.perturbation = NONE, .reflection = 1.0f, .refraction = 0.0f,
		.r_index = 0.0f});
}

t_light		light(cl_float4 pos, cl_float4 color)
{
	return ((t_light){.position = pos, .color = color, .direct = 1.0f});
}

void		calc_vpul(void)
{
	float	y;
	float	x;

	y = ((float)cam()->vp_size.y / 2.0f);
	x = -((float)cam()->vp_size.x / 2.0f);
	cam()->vpul.x = ((cam()->dir.x * cam()->dist) +
					(cam()->up.x * y)) + (cam()->right.x * x);
	cam()->vpul.y = ((cam()->dir.y * cam()->dist) +
					(cam()->up.y * y)) + (cam()->right.y * x);
	cam()->vpul.z = ((cam()->dir.z * cam()->dist) +
					(cam()->up.z * y)) + (cam()->right.z * x);
	cam()->vpul.w = 0.0f;
}

t_vector	cl_float4_to_vector(cl_float4 v)
{
	return ((t_vector){.x = v.x, .y = v.y, .z = v.z});
}

cl_float4	vector_to_cl_float4(t_vector v)
{
	return ((cl_float4){.x = v.x, .y = v.y, .z = v.z, .w = 0.0f});
}

void		rotate_cam(double angle, t_vector axe)
{
	t_quaternion 	q;
	t_matrix		*mat;
	t_vector		*tva;

	q = ft_quat_rotation_build(angle, axe);
	q = ft_quat_multiply(cam()->orientation, q);
	cam()->orientation = q;
	mat = ft_quat_rotation_to_matrix(NULL, q);
	tva = ft_matrix_multply_vector_array((t_vector[3]){
//		cl_float4_to_vector(cam()->dir),
//		cl_float4_to_vector(cam()->up),
//		cl_float4_to_vector(cam()->right)
	ft_vector(0,0,1),
	ft_vector(0,1,0),
	ft_vector(1,0,0)
}, 3, mat);
	cam()->dir = vector_to_cl_float4(tva[0]);
	cam()->up = vector_to_cl_float4(tva[1]);
	cam()->right = vector_to_cl_float4(tva[2]);
	ft_matrix_free(mat);
	calc_vpul();
}

int			keys(t_ftx_data *data)
{
	static t_ubmp		out = {.data = NULL, .size = {SW, SH}};

	(void)data;
	if (data->keymap[KEY_W].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.y += 10;
	if (data->keymap[KEY_S].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.y -= 10;
	if (data->keymap[KEY_D].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.x += 10;
	if (data->keymap[KEY_A].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.x -= 10;
	if (data->keymap[KEY_Q].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.z += 10;
	if (data->keymap[KEY_E].status == FTX_KEY_STATUS_PRESSED)
		cam()->pos.z -= 10;
	if (data->keymap[KEY_LEFT].status == FTX_KEY_STATUS_PRESSED)
		rotate_cam(-0.05, ft_vector(0, 1, 0));
	if (data->keymap[KEY_RIGHT].status == FTX_KEY_STATUS_PRESSED)
		rotate_cam(0.05, ft_vector(0, 1, 0));
	if (data->keymap[KEY_UP].status == FTX_KEY_STATUS_PRESSED)
		rotate_cam(-0.05, ft_vector(1, 0, 0));
	if (data->keymap[KEY_DOWN].status == FTX_KEY_STATUS_PRESSED)
		rotate_cam(0.05, ft_vector(1, 0, 0));
	//out.size = ft_point(SW, SH);
	if (out.data == NULL)
		out.data = (int*)ft_memalloc(sizeof(int) * SW * SH);
	ftocl_clear_current_kernel_arg(4);
//	ftocl_set_current_kernel_arg(CL_MEM_READ_ONLY, 2, sizeof(t_primitive) *
//		argn()->nb_objects, (void*)*prim());
	ftocl_set_current_kernel_arg(CL_MEM_READ_ONLY, 4, sizeof(t_camera), (void*)cam());

	ftocl_start_current_kernel(1, (const size_t[1]){SW * SH}, NULL);
	ftocl_read_current_kernel_arg(0, out.data);
	//ftx_fill_image(rt_win()->vbuffer, 0x00FFFF, 1);
	ftx_put_ubmp_img(rt_win()->vbuffer, ft_point(GUI_WIDTH, 0), &out,
					NOMASK);
	draw_gui();
	//ftx_refresh_window(rt_win());
	//ft_free(out.data);

	return (0);
}

int		rien(int x, int y, void *lol)
{
	(void)x; (void)y; (void)lol;
	return (0);
}

t_window	*rt_win(void)
{
	static t_window		*window = NULL;

	if (window == NULL)
		window = ftx_new_window(ft_point(SW + GUI_WIDTH, SH), "RT", NULL);
	return (window);
}

void		rtv1(void)
{
	t_ubmp		out;
	cl_event	event;

	argn()->screen_size = (cl_int2){.x = SW, .y = SH};
	argn()->nb_objects = 3;
	argn()->nb_lights = 3;
	argn()->gamma = 1.0f;
	argn()->filter = NONE;
	argn()->antialias = 1;
	*prim() = (t_primitive*)ft_malloc(sizeof(t_primitive) * argn()->nb_objects);
	*lights() = (t_light*)ft_malloc(sizeof(t_light) * argn()->nb_lights);
	//prim()[0][0] = cone((cl_float4){.x = 0, .y = 0, .z = 800, .w = 0}, (cl_float4){.x = 0, .y = 1, .z = 0, .w = 0}, 10, (cl_float4){.x = 1, .y = 0, .z = 0, .w = 0});
	prim()[0][0] = sphere((cl_float4){.x = -150, .y = 0, .z = 500, .w = 0}, 200, (cl_float4){.x = 1, .y = 0, .z = 0, .w = 0});
	prim()[0][1] = plane((cl_float4){.x = 0, .y = 0, .z = 1000, .w = 0}, (cl_float4){.x = 0, .y = 0, .z = 1, .w = 0}, (cl_float4){.x = 1, .y = 1, .z = 1, .w = 0});
	prim()[0][2] = sphere((cl_float4){.x = 300, .y = 0, .z = 500, .w = 0}, 120, (cl_float4){.x = 0, .y = 1, .z = 0, .w = 0});
	//prim()[0][3] = paraboloid((cl_float4){.x = 0, .y = -200, .z = 300, .w = 0}, (cl_float4){.x = 0, .y = -1, .z = 0, .w = 0}, 100, (cl_float4){.x = 1, .y = 1, .z = 1, .w = 0});
	lights()[0][0] = light((cl_float4){.x = 100, .y = 0, .z = 500, .w = 0},  (cl_float4){.x = 1, .y = 1, .z = 1, .w = 0});
	lights()[0][1] = light((cl_float4){.x = 0, .y = 300, .z = 600, .w = 0}, (cl_float4){.x = 1, .y = 1, .z = 1, .w = 0});
	lights()[0][2] = light((cl_float4){.x = 0, .y = 300, .z = 100, .w = 0}, (cl_float4){.x = 1, .y = 1, .z = 1, .w = 0});
	cam()->pos = (cl_float4){.x = 0, .y = 0, .z = 0, .w = 0};
	cam()->vp_size = (cl_int2){.x = SW, .y = SH};
	cam()->dist = 800;
	calc_vpul();
	out.size = ft_point(SW, SH);
	out.data = (int*)ft_malloc(sizeof(int) * SW * SH);
	update_kernel_args();
	event = ftocl_start_current_kernel(1, (const size_t[1]){SW * SH}, NULL);
	clWaitForEvents(1, &event);
	clReleaseEvent(event);
	ftocl_read_current_kernel_arg(0, out.data);
	//ftx_put_ubmp_img(rt_win()->vbuffer, ft_point(GUI_WIDTH, 0), &out,
	//				NOMASK);
	printf("Kernel: %.8s was succesfully loaded\n",
				(char*)&ftocl_data()->current_kernel->id);
	ftx_key_hook(KEY_EXIT, cb_exit, NULL);
	//keys();
	ftx_loop_hook(&keys);
	ftx_hook_mice_button(rt_win(), MICE_LEFT, rien, NULL);
	init_gui(rt_win());
	draw_gui();
	ftx_start();
}

int			main(const int argc, char **argv, char **env)
{
	int		fd;

	ft_init(env);
	if (argc != 2)
		ft_end(0 * printf("\nUsage: \t%s/%s <scene.json>\n\n", ft_pwd(),
										ft_path_name(argv[0])));
	if ((fd = open(OCL_SOURCE_PATH, O_RDONLY)) == -1)
		ft_end(-1);
	ftocl_make_program(ft_str_to_id64("rtv1"),
		ft_str_clear_commentaries(readfile(fd)), NULL);
	close(fd);
	if (!(fd = ftocl_set_current_kernel(ft_str_to_id64("example"))))
		rtv1();
	if (fd == 1)
		ft_end(0 * printf("kernel was not found\n"));
	ft_end(0);
}
