/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/21 19:24:09 by hmartzol          #+#    #+#             */
/*   Updated: 2017/01/25 10:21:53 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rtv1.h>
#include <stdio.h>

void				print_argn(void)
{
	printf("screen_size: %d, %d\n", argn()->screen_size.x,
									argn()->screen_size.y);
	printf("nb_objects: %d\n", argn()->nb_objects);
	printf("nb_lights: %d\n", argn()->nb_lights);
	printf("ambient: %f\n", argn()->ambient);
	printf("direct: %f\n", argn()->direct);
	printf("global: %f\n", argn()->global);
	printf("global_illumination_samples: %d\n",
			argn()->global_illumination_samples);
	printf("antialias: %d\n", argn()->antialias);
	printf("bounce_depth: %d\n", argn()->bounce_depth);
	printf("filter: %d\n", argn()->filter);
	printf("stereoscopy: %d\n", argn()->stereoscopy);
}

void				print_cam(void)
{
	printf("pos: %f, %f, %f\n", cam()->pos.x, cam()->pos.y, cam()->pos.z);
	printf("dir: %f, %f, %f\n", cam()->dir.x, cam()->dir.y, cam()->dir.z);
	printf("up: %f, %f, %f\n", cam()->up.x, cam()->up.y, cam()->up.z);
	printf("right: %f, %f, %f\n", cam()->right.x, cam()->right.y, cam()->right.z);
	printf("vpul: %f, %f, %f\n", cam()->vpul.x, cam()->vpul.y, cam()->vpul.z);
	printf("vp_size: %f, %f\n", cam()->vp_size.x, cam()->vp_size.y);
	printf("dist: %f\n", cam()->dist);
	printf("orientation: %f, %f, %f, %f\n", cam()->orientation.r, cam()->orientation.i, cam()->orientation.j, cam()->orientation.k);
}

void				print_materials(void)
{

}

void				parse_render_options_0(t_json_value *ro)
{
	t_json_value		*v[2];
	t_json_value_type	*la_norm_est_chiante;

	v[0] = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 4, .ptr = "size"});
	la_norm_est_chiante = (t_json_value_type[2]){integer, integer};
	ft_json_check_array_types(v[0], 2, la_norm_est_chiante) ?
		(void)(argn()->screen_size = (cl_int2){.x =
		(cl_int) * (int*)((t_json_array*)v[0]->ptr)->value[0]->ptr, .y =
		(cl_int) * (int*)((t_json_array*)v[0]->ptr)->value[1]->ptr}) : 0;
	v[0] = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 8, .ptr = "lighting"});
	v[1] = ft_json_search_pair_in_object(v[0],
		(t_json_string){.length = 7, .ptr = "ambient"});
	(v[1] != NULL && v[1]->type == number) ? argn()->ambient =
		ft_clampd(*(double*)v[1]->ptr, 0.0, 1.0) : 0;
	v[1] = ft_json_search_pair_in_object(v[0],
		(t_json_string){.length = 6, .ptr = "direct"});
	(v[1] != NULL && v[1]->type == number) ? argn()->direct =
		ft_clampd(*(double*)v[1]->ptr, 0.0, 1.0) : 0;
	v[1] = ft_json_search_pair_in_object(v[0],
		(t_json_string){.length = 6, .ptr = "global"});
	(v[1] != NULL && v[1]->type == number) ? argn()->global =
		ft_clampd(*(double*)v[1]->ptr, 0.0, 1.0) : 0;
}

void				parse_render_options(t_json_value *ro)
{
	t_json_value	*v;

	parse_render_options_0(ro);
	v = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 27, .ptr = "global_illumination_samples"});
	(v != NULL && v->type == integer) ? argn()->global_illumination_samples =
		ft_clampi(*(int*)v->ptr, 0, 64) : 0;
	v = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 9, .ptr = "antialias"});
	(v != NULL && v->type == integer) ? argn()->antialias =
		ft_clampi(*(int*)v->ptr, 1, 16) : 0;
	v = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 12, .ptr = "bounce_depth"});
	(v != NULL && v->type == integer) ? argn()->bounce_depth =
		ft_clampi(*(int*)v->ptr, 2, 32) : 0;
	v = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 6, .ptr = "filter"});
	argn()->filter = ft_json_check_string(v, 4,
		(char**)(size_t[4]){(size_t)NULL, (size_t)"sepia", (size_t)"grayscale",
			(size_t)"cartoon"}, argn()->filter);
	v = ft_json_search_pair_in_object(ro,
		(t_json_string){.length = 11, .ptr = "stereoscopy"});
	(v != NULL && v->type == boolean) ? argn()->stereoscopy =
		*(int*)v->ptr : 0;
}

void				parse_camera_0(t_json_value *vps)
{
	t_json_value_type	*j_ai_plus_d_idee_pour_railler_la_norm;

	j_ai_plus_d_idee_pour_railler_la_norm =
		(t_json_value_type[2]){integer | number, integer | number};
	if (ft_json_check_array_types(vps, 2, j_ai_plus_d_idee_pour_railler_la_norm))
	{
		cam()->vp_size.x = ((t_json_array*)vps->ptr)->value[0]->type == integer
			? (cl_float) * (int*)((t_json_array*)vps->ptr)->value[0]->ptr :
			(cl_float) * (double*)((t_json_array*)vps->ptr)->value[0]->ptr;
		cam()->vp_size.y = ((t_json_array*)vps->ptr)->value[1]->type == integer
			? (cl_float) * (int*)((t_json_array*)vps->ptr)->value[1]->ptr :
			(cl_float) * (double*)((t_json_array*)vps->ptr)->value[1]->ptr;
	}
}

void				parse_camera(t_json_value *c)
{
	t_json_value		*v;

	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 8, .ptr = "position"});
	cam()->pos = cl_vector_from_json_array(v, cam()->pos);
	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 9, .ptr = "direction"});
	cam()->dir = cl_vector_from_json_array(v, cam()->dir);
	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 2, .ptr = "up"});
	cam()->up = cl_vector_from_json_array(v, cam()->up);
	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 5, .ptr = "right"});
	cam()->right = cl_vector_from_json_array(v, cam()->right);
	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 7, .ptr = "vp_size"});
	parse_camera_0(v);
	v = ft_json_search_pair_in_object(c,
		(t_json_string){.length = 4, .ptr = "dist"});
	if (v != NULL && (v->type == number || v->type == integer) &&
			v->ptr != NULL)
		cam()->dist = (v->type == number) ?
			(cl_float) * (double*)v->ptr : (cl_float) * (int*)v->ptr;
}

t_material			default_material(void)
{
	t_material	out;

	out.color = (cl_float4){.x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 0.0f};
	out.diffuse = 1.0f;
	out.specular = 1.0f;
	out.refraction = 0.0f;
	out.alpha = 1.0f;
	out.refraction = 1.0f;
	out.perturbation = (t_perturbation){.normal = NONE, .color = NONE};
	out.texture.info_index = 0;
	out.texture.stretch = (cl_float2){.x = 100.0f, .y = 100.0f};
	out.texture.info_index = 0;
	out.texture.stretch = (cl_float2){.x = 100.0f, .y = 100.0f};
	return (out);
}

//cl_ulong

t_texture			parse_texture(t_json_value *t, t_texture default_return)
{
	t_json_value		*v;
	t_json_value_type	*paf_paf_paf_la_norm;
	t_texture			out;

	out = default_return;
	v = ft_json_search_pair_in_object(t,
		(t_json_string){.length = 4, .ptr = "size"});
	paf_paf_paf_la_norm =
		(t_json_value_type[2]){integer | number, integer | number};
	if (ft_json_check_array_types(v, 2, paf_paf_paf_la_norm))
	{
		out.stretch.x = ((t_json_array*)v->ptr)->value[0]->type == integer ?
			(cl_float) * (int*)((t_json_array*)v->ptr)->value[0]->ptr :
			(cl_float) * (double*)((t_json_array*)v->ptr)->value[0]->ptr;
		out.stretch.y = ((t_json_array*)v->ptr)->value[1]->type == integer ?
			(cl_float) * (int*)((t_json_array*)v->ptr)->value[1]->ptr :
			(cl_float) * (double*)((t_json_array*)v->ptr)->value[1]->ptr;
	}
	v = ft_json_search_pair_in_object(t,
		(t_json_string){.length = 4, .ptr = "file"});
	out.info_index = 0;
	return (out);
}

t_material			parse_material(t_json_value *m)
{
	t_json_value	*v[2];
	t_material		out;

	out = default_material();
	if (m == NULL || m->type != object || m->ptr == NULL)
		return (out);
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 5, .ptr = "color"});
	out.color = cl_vector_from_json_array(v[0], out.color);
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 7, .ptr = "diffuse"});
	(v[0] != NULL && v[0]->type == number && v[0]->ptr != NULL) ?
		out.diffuse = (cl_float) * (double*)v[0]->ptr : 0;
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 8, .ptr = "specular"});
	(v[0] != NULL && v[0]->type == number && v[0]->ptr != NULL) ?
		out.specular = (cl_float) * (double*)v[0]->ptr : 0;
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 7, .ptr = "refraction"});
	(v[0] != NULL && v[0]->type == number && v[0]->ptr != NULL) ?
		out.refraction = (cl_float) * (double*)v[0]->ptr : 0;
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 5, .ptr = "alpha"});
	(v[0] != NULL && v[0]->type == number && v[0]->ptr != NULL) ?
		out.alpha = (cl_float) * (double*)v[0]->ptr : 0;
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 10, .ptr = "refraction"});
	(v[0] != NULL && v[0]->type == number && v[0]->ptr != NULL) ?
		out.refraction = (cl_float) * (double*)v[0]->ptr : 0;
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 12, .ptr = "perturbation"});
	v[1] = ft_json_search_pair_in_object(v[0],
		(t_json_string){.length = 6, .ptr = "normal"});
	out.perturbation.normal = ft_json_check_string(v[1], 3,
	(char**)(size_t[3]){(size_t)NULL, (size_t)"sine", (size_t)"checkerboard"},
	out.perturbation.normal);
	v[1] = ft_json_search_pair_in_object(v[0],
		(t_json_string){.length = 6, .ptr = "color"});
	out.perturbation.color = ft_json_check_string(v[1], 3,
	(char**)(size_t[3]){(size_t)NULL, (size_t)"sine", (size_t)"checkerboard"},
	out.perturbation.color);
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 7, .ptr = "texture"});
	out.texture = parse_texture(v[0], out.texture);
	v[0] = ft_json_search_pair_in_object(m,
		(t_json_string){.length = 7, .ptr = "bumpmap"});
	out.bumpmap = parse_texture(v[0], out.bumpmap);
	return (out);
}

t_material			**materials(void)
{
	static t_material	*m;

	return (&m);
}

t_pair				new_material(t_json_pair *p, unsigned long i)
{
	t_pair	out;

	if (p == NULL || p->key == NULL || p->value == NULL || p->key->ptr == NULL
		|| p->key->length == 0 || (out.name = ft_strdup(p->key->ptr)) == NULL)
		return ((t_pair){.name = "default", .index = 0});
	out.index = ft_strcmp("default", out.name) ? i + 1 : 0;
	(*materials())[out.index] = parse_material(p->value);
	return (out);
}

t_pair				*parse_materials(t_json_value *m)
{
	t_json_object	*obj;
	t_pair			*out;
	unsigned long	i;

	out = NULL;
	if (m == NULL || m->type != object || m->ptr == NULL ||
	(obj = (t_json_object*)m->ptr)->nb_pairs == 0 || obj->pair == NULL ||
	(out = (t_pair*)ft_malloc(sizeof(t_pair) * obj->nb_pairs))
	== NULL || (*materials() =
	ft_malloc(sizeof(t_img_info) * (obj->nb_pairs + 1))) == NULL)
		return (out != NULL ? ft_free(out) : NULL);
	i = -1;
	(*materials())[0] = default_material();
	while (++i < obj->nb_pairs)
		out[i] = new_material(obj->pair[i], i);
	return (out);
}

t_primitive			parse_object(t_json_value *o)
{
	t_json_value	*v;
	t_primitive		p;

	if (o == NULL || o->type != object || o->ptr == NULL)
		ft_error(EINTERN, "Parser encountered invalid object\n");
	v = ft_json_search_pair_in_object(o,
		(t_json_string){.length = 4, .ptr = "type"});
	p.type = ft_json_check_string(v, 4,
		(char**)(size_t[4]){(size_t)NULL, (size_t)"sepia", (size_t)"grayscale",
			(size_t)"cartoon"}, argn()->filter);
	return (p);
}

void				parse_objects(t_json_value *o)
{
	t_json_array	*ar;
	unsigned long	i;

	if (o == NULL || o->type != array || (ar = (t_json_array*)o->ptr) == NULL ||
		ar->nb_values == 0 || ar->value == NULL ||
		(*prim() = (t_primitive*)ft_malloc(sizeof(t_primitive) * ar->nb_values))
		== NULL)
		return ;
	i = -1;
	while (++i < ar->nb_values)
		(*prim())[i] = parse_object(ar->value[i]);
}

void				parse_lights(t_json_value *l)
{
	(void)l;
}

int					check_null_cl_float4(cl_float4 v)
{
	return (v.x == 0 && v.y == 0 && v.z == 0 && v.w == 0);
}

int					check_parsed_data(void)
{
	if (argn()->screen_size.x <= 0 || argn()->screen_size.y <= 0 ||
	check_null_cl_float4(cam()->dir) || check_null_cl_float4(cam()->up) ||
	check_null_cl_float4(cam()->right) || cam()->vp_size.x <= 0 ||
	cam()->vp_size.y <= 0 || cam()->dist <= 0)
		return (0);
	return (1);
}

void				parse_textures(t_json_value *root)
{
	(void)root;
}

void				parser(const char *src)
{
	t_json_value	*root;
	t_json_value	*v;

	root = ft_json_parse_src(ft_str_clear_commentaries((char*)src));
	parse_textures(root);
	v = ft_json_search_pair_in_object(root,
		(t_json_string){.length = 14, .ptr = "render_options"});
	parse_render_options(v);
	print_argn();
	v = ft_json_search_pair_in_object(root,
		(t_json_string){.length = 6, .ptr = "camera"});
	parse_camera(v);
	print_cam();
	v = ft_json_search_pair_in_object(root,
		(t_json_string){.length = 9, .ptr = "materials"});
	parse_materials(v);
	print_materials();
	v = ft_json_search_pair_in_object(root,
		(t_json_string){.length = 7, .ptr = "objects"});
	parse_objects(v);
//	print_objects();
	v = ft_json_search_pair_in_object(root,
		(t_json_string){.length = 6, .ptr = "lights"});
	parse_lights(v);
//	print_lights();
	if (!check_parsed_data())
		ft_error(EINTERN, "Invalid json values\n");
	printf("%s\n", ft_realpath(NULL, NULL));
}
