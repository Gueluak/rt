// raytracer kernel is pretty cool actually

typedef enum	e_prim_type
{
	SPHERE = 0, PLANE = 1, CONE = 2, CYLINDER = 3, PARABOLOID = 4
}				t_prim_type;

#define NONE 0
#define NO_PRIMITIVE -1

typedef enum	e_color_filter
{
	SEPIA = 1, GRAYSCALE = 2, CARTOON = 3
}				t_color_filter;

typedef enum	e_perturbation
{
	CHECKERBOARD = 1, SINE = 2
}				t_perturbation;

typedef enum	e_ray_type
{
	ORIGIN = 0, REFLECTED = 1, REFRACTED = 2
}				t_ray_type;

typedef struct	s_primitive
{
	t_prim_type		type;
	float4			position;
	float4			direction;
	float4			color;
	float			radius;
	float			ambient;
	float			diffuse;
	float			specular;
	float			reflection;
	float			refraction;
	float			transparency;
	float			r_index;
	float			normal_perturbation;
	t_perturbation	perturbation;
}				t_primitive;

typedef struct	s_light
{
	float4		position;
	float4		color;
	float		direct;
}				t_light;

typedef struct	s_argn		//structure containing the limit of out, rays and objects
{							//note the corespondance with the C structure (types end position of variables correspond, but types have a preceding "cl_")
	int2			screen_size;	//total screen size in pixels (accesed by .x and .y)
	int				nb_objects;		//total number of objects in the scene
	int				nb_lights;		//total number of lights in the scene
	float			gamma;
	t_color_filter	filter;
	int				antialias;
}				t_argn;		//norm42 magle

typedef struct	s_camera	//note: yes, this structure is not equivalent to s_camera in rtv1.h
{
	float4	pos;
	float4	dir;
	float4	up;
	float4	right;
	float4	vpul;
	int2	vp_size;
}				t_camera;

typedef struct	s_ray
{
	float4		origin;
	float4		direction;
	float		dist;
	float		weight;
	int			depth;
	int			primitive_id;
	t_ray_type	type;
	float		r_index;
	float4		transparency;
}				t_ray;

//j'aime les commentaires, et vous? :p
/*
   NOTE HYPER IMPORTANTE:
   pour la compatibilité AMD, il faut continuer d'utiliser les flags __global et les pointeurs
   (AMD ne copie pas les déférencements de pointeurs dans la stack, ce qui fait que les objets et la lumière n'étaient pas envoyé aux calculs)
   */

int		plane_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		sphere_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		cylinder_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		cone_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		paraboloid_intersect(__global t_primitive *obj, t_ray *ray, float *dist);

int		intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		solve_quadratic(float a, float b, float c, float *dist);
float4	get_normal(__global t_primitive *obj, float4 point);
float4	phong(float4 dir, float4 norm);
int		color_to_int(float4 color);
int		raytrace(t_ray *ray, float4 *color, float4 *point, int *result, __global t_primitive *objects, __global t_light *lights, __global t_argn *argn);

#if 0
# define DOT local_dot
# define NORMALIZE local_normalize
# define LENGTH local_length
# define CROSS local_cross

inline float4	local_normalize(float4 v);
inline float	local_dot(float4 v1, float4 v2);
inline float4	local_cross(float4 v1, float4 v2);
inline float	local_length(float4 v);

inline float4	local_cross(float4 v1, float4 v2)
{
	float4 res;

	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.z * v2.x - v1.x * v2.z;
	res.z = v1.x * v2.y - v1.y * v2.x;
	return (res);
}

inline float	local_dot(float4 v1, float4 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline float4	local_normalize(float4 v)
{
	float	t;

	if ((v.x == 0.0f && v.y == 0.0f && v.z == 0.0f) || (t = (v.x * v.x + v.y * v.y + v.z * v.z)) == 1.0f)
		return (v);
	return (v / sqrt(t));
}

inline float	local_length(float4 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
#else
# define DOT dot
# define NORMALIZE normalize
# define LENGTH length
# define CROSS cross
#endif

// error correction
#ifndef EPSILON
# define EPSILON 0.0001f
#endif

#ifndef SHADOW_E
# define SHADOW_E 0.1f
#endif

// maximum ray depth
#ifndef TRACE_DEPTH
# define TRACE_DEPTH 3
#endif

// TODO: generate this in the client
#ifndef MAX_RAY_COUNT
# define MAX_RAY_COUNT (1 << TRACE_DEPTH)
#endif

// minimum direct lighting coefficient
#ifndef MIN_DIRECT
# define MIN_DIRECT 0.95f
#endif

// normal perturbation size
#ifndef NORMAL_PERTURBATION_SIZE
# define NORMAL_PERTURBATION_SIZE 10.0f
#endif

// color perturbation checkerboard size
#ifndef CHECKER_SIZE
# define CHECKER_SIZE 10
#endif

// cartoon effect steps
#ifndef CARTOON_STEPS
# define CARTOON_STEPS 3
#endif

inline float	addv(float4 v)
{
	return (v.x + v.y + v.z);
}

int		plane_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float d = DOT(obj->direction, ray->direction);

	// facing the plane (d == 0)
	if (d > -EPSILON && d < EPSILON)
		return (0);

	float new_dist = DOT(obj->position - ray->origin, obj->direction) / d;

	if (new_dist > EPSILON && new_dist < *dist)
	{
		*dist = new_dist;
		return (d > 0 ? -1 : 1);
	}
	return (0);
}

int		sphere_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = obj->position - ray->origin;

	float a = DOT(ray->direction, ray->direction);
	float b = 2 * DOT(ray->direction, pos);
	float c = DOT(pos, pos) - obj->radius * obj->radius;
	return solve_quadratic(a, b, c, dist);
}

int		cylinder_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = obj->position - ray->origin;
	float4 p = CROSS(pos, obj->direction);
	float4 r = CROSS(ray->direction, obj->direction);

	float a = DOT(r, r);
	float b = 2 * DOT(r, p);
	float c = DOT(p, p) - obj->radius * obj->radius * DOT(obj->direction, obj->direction);
	return solve_quadratic(a, b, c, dist);
}

int		cone_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = ray->origin - obj->position;
	float4 dir = -ray->direction;

	float r = 1.0f + obj->radius * obj->radius;
	float dd = DOT(dir, obj->direction);
	float xv = DOT(pos, obj->direction);

	float a = DOT(dir, dir) - (r * dd * dd);
	float b = 2.0f * (DOT(dir, pos) - (r * dd * xv));
	float c = DOT(pos, pos) - (r * xv * xv);

	return solve_quadratic(a, b, c, dist);
}

int		paraboloid_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = ray->origin - obj->position;
	float4 dir = -ray->direction;

	float dv = DOT(dir, obj->direction);
	float xv = DOT(pos, obj->direction);

	float a = DOT(dir, dir) - dv * dv;
	float b = 2.0f * (DOT(dir, pos) - dv * (xv + 2.0f * obj->radius));
	float c = DOT(pos, pos) - xv * (xv + 4.0f * obj->radius);

	return solve_quadratic(a, b, c, dist);
}

int		solve_quadratic(float a, float b, float c, float *dist)
{
	float delta = b * b - 4.0f * a * c;
	if (delta < 0)
		return (0);

	delta = sqrt(delta);
	float x1 = (b - delta) / (2.0f * a);
	float x2 = (b + delta) / (2.0f * a);

	if (x1 < EPSILON) // use x2 is x1 is negative
	{
		if (x2 < EPSILON) // both are negative
			return (0);
		else if (x2 < *dist) // x2 positive
		{
			*dist = x2;
			return (-1);
		}
	}
	else if (x1 < *dist) // x1 positive
	{
		*dist = x1;
		return (1);
	}
	return (0);
}

// TODO: implement limit structures
inline int		limit(__global t_primitive *obj, float4 point)
{
	//if (point.y > 150)
	//	return (1);
	return (0);
}

inline int		intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	int i = 0;
	float d = *dist;

	switch (obj->type)
	{
		case SPHERE:
			i = sphere_intersect(obj, ray, dist);
			break;
		case PLANE:
			i = plane_intersect(obj, ray, dist);
			break;
		case CONE:
			i = cone_intersect(obj, ray, dist);
			break;
		case CYLINDER:
			i = cylinder_intersect(obj, ray, dist);
			break;
		case PARABOLOID:
			i = paraboloid_intersect(obj, ray, dist);
			break;
	}

	// limit our object
	float4 point = ray->origin + ray->direction * (*dist);
	if (limit(obj, point))
	{
		*dist = d;
		return (0);
	}

	return (i);
}

inline float4	get_normal(__global t_primitive *obj, float4 point)
{
	float4 n = (float4)(0, 0, 0, 0);

	switch (obj->type)
	{
		case SPHERE:
			n = point - obj->position;
			break;
		case PLANE:
			n = obj->direction;
			break;
		case CYLINDER:
			n = DOT(obj->direction, obj->position - point) * obj->direction + (point - obj->position);
			break;
		case CONE:
			n = point - obj->position + (obj->direction * -DOT(point, obj->direction) / pow(cos(obj->radius), 2));
			break;
		case PARABOLOID:
			n = point - obj->position - obj->direction * obj->radius;
			break;
	}

	// normal perturbation
	if (obj->normal_perturbation > 0.0f)
	{
		float len = LENGTH(n);
		//n.x = n.x + sin(point.x / NORMAL_PERTURBATION_SIZE) * obj->normal_perturbation * (len / NORMAL_PERTURBATION_SIZE);
		n.y = n.y + cos(point.y / NORMAL_PERTURBATION_SIZE) * obj->normal_perturbation * (len / NORMAL_PERTURBATION_SIZE);
	}
	return (NORMALIZE(n));
}

inline float2	texture_map(__global t_primitive *prim, float4 normal)
{
	float2 pos = (float2)(0, 0);

	switch (prim->type)
	{
		case SPHERE:
			pos.x = 0.5f + atan2(normal.z, normal.x) / (2.0f * M_PI);
			pos.y = 0.5f + asinpi(normal.y);
			break;
	}
	return (pos);
}

inline float4	color_perturbation(float4 color, __global t_primitive *prim, float4 normal)
{
	float2 pos = texture_map(prim, normal);

	switch (prim->perturbation)
	{
		case CHECKERBOARD:
			if (fmod(pos.x * CHECKER_SIZE, 2) <= 1.0f ^ fmod(pos.y * CHECKER_SIZE, 2) <= 1.0f)
				color /= 2.0f;
			break;
		case SINE:
			color *= fabs(cos((pos.x + pos.y) * 36 * CHECKER_SIZE) - 1);
			break;
	}
	return (color);
}

inline float4	phong(float4 dir, float4 norm)
{
	return (dir - 2 * DOT(dir, norm) * norm);
}

inline int		color_to_int(float4 color)
{
	color = clamp(color * 255.0f, 0.0f, 255.0f);
	return (int)(((int)color.x << 16) + ((int)color.y << 8) + (int)color.z);
}

inline float4		color_filter(float4 color, t_color_filter filter)
{
	float4	out;
	float	t;

	switch (filter)
	{
		case SEPIA:
			out.x = color.x * 0.393f + color.y * 0.769f + color.z * 0.189f;
			out.y = color.x * 0.349f + color.y * 0.686f + color.z * 0.168f;
			out.z = color.x * 0.272f + color.y * 0.534f + color.z * 0.131f;
			break;
		case GRAYSCALE:
			t = (color.x + color.y + color.z) / 3.0f;
			out.x = t;
			out.y = t;
			out.z = t;
			break;
		case CARTOON:
			out.x = floor(color.x * CARTOON_STEPS) / CARTOON_STEPS;
			out.y = floor(color.y * CARTOON_STEPS) / CARTOON_STEPS;
			out.z = floor(color.z * CARTOON_STEPS) / CARTOON_STEPS;
			break;
		case NONE:
			out = color;
			break;
	}

	return (out);
}

int		raytrace(t_ray *ray, float4 *color, float4 *point, int *result, __global t_primitive *objects, __global t_light *lights, __global t_argn *argn)
{
	int id = NO_PRIMITIVE;
	int cur;
	int temp;

	// hit test against all primitives
	for (cur = 0; cur < argn->nb_objects; cur++)
	{
		if ((temp = intersect(&objects[cur], ray, &ray->dist)))
		{
			id = cur;
			*result = temp;
		}
	}

	// if we hit something, get relevant info
	__global t_primitive *prim;
	float4 collision = (float4)(0, 0, 0, 0);
	float4 norm;

	float4 cur_color = (float4)(0, 0, 0, 0);
	float4 add_color = (float4)(0, 0, 0, 0);

	if (id != NO_PRIMITIVE)
	{
		// we have our primitive!
		prim = &objects[id];
		collision = ray->origin + ray->direction * ray->dist;

		// get the normal for this intersection point
		norm = get_normal(prim, collision);

		// invert the normal if we're "inside" the primitive
		if (*result == -1)
			norm = -norm;

		// add ambient light
		cur_color += prim->color * prim->ambient;
	}

	// lights, maestro!
	int cur_l;
	t_ray ray_l;
	float dist_l;
	float scal;
	for (cur_l = 0; cur_l < argn->nb_lights; cur_l++)
	{
		t_light light = lights[cur_l];

		// calculate direct lighting
		if (light.direct > 0)
		{
			dist_l = LENGTH(light.position - ray->origin);
			ray_l.direction = NORMALIZE(light.position - ray->origin);
			ray_l.origin = ray->origin;

			// if our light is closer than intersect
			if (dist_l < ray->dist)
			{
				// if our light is between us and the object, dot will
				// be positive
				scal = DOT(ray_l.direction, ray->direction);
				if (scal > EPSILON)
				{
					scal = pow(scal, dist_l / 100);
					if (scal > MIN_DIRECT)
						add_color += (light.color * (scal - MIN_DIRECT) / (1.0f - MIN_DIRECT)) * light.direct;
				}
			}
		}

		// if we didn't hit anything, don't calculate light for object
		if (id == NO_PRIMITIVE)
			continue;

		// check if our light source is blocked by an object
		// shadows start a tiny amount from the actual sphere to prevent
		// rounding errors
		dist_l = LENGTH(light.position - collision);
		ray_l.direction = NORMALIZE(light.position - collision);
		ray_l.origin = collision + ray_l.direction * SHADOW_E;
		int shadow = 0;

		float dist = MAXFLOAT;
		for (cur = 0; cur < argn->nb_objects; cur++)
		{
			if ((shadow = intersect(&objects[cur], &ray_l, &dist)) > 0)
			{
				if (dist > EPSILON && dist < dist_l) // it is between us
					break ;
				else // it is behind us
					shadow = 0;
			}
		}

		// did we hit something? it's a shadow... spooky!
		if (shadow)
			continue ;

		// diffuse lighting
		if (prim->diffuse > 0 && (scal = DOT(ray_l.direction, norm)) > EPSILON)
			cur_color += prim->color * light.color * scal * prim->diffuse;

		// specular highlights (needs pow to make the curve sharper)
		float4 ir = phong(-ray_l.direction, norm);
		if (prim->specular > 0 && scal > EPSILON && (scal = DOT(ray_l.direction, ir)) > EPSILON)
			cur_color += light.color * pow(scal, dist_l / 40) * prim->specular;
	}

	// calculate final color
	//*color += clamp(cur_color / ((float)argn->nb_lights * argn->gamma), 0.0f, 1.0f);
	*color += clamp(cur_color / ((float)argn->nb_lights * argn->gamma), 0.0f, 1.0f);
	// perturbation if we hit something
	if (id != NO_PRIMITIVE)
		*color = color_perturbation(*color, prim, norm);
	// added color
	//*color += clamp(add_color, 0.0f, 1.0f);
	*color += add_color;

	*point = collision;
	return (id);
}

#define PUSH_RAY(q, r, c) q[c++] = r;
#define POP_RAY(q, r, c) r = q[--c];

__kernel void	example(							//main kernel, called for each ray
		__global int *out,				//int bitmap, his size is equal to screen_size.x * screen_size.y
		__global t_argn *argn,			//structure containing important info on how to acces out, rays and objects
		__global t_primitive *objects,	//set of objects in the scene, the number of objects (and so the limit of this array), is stored in argn
		__global t_light *lights,
		__global t_camera *cam)			//vector3 rays stored has float4 for convenience, the size of the array is screen_size.x * screen_size.y
{
	//mode 2: we use 1D Kernels:
	size_t i = get_global_id(0); //id of the kernel in the global call

	// the amount of kernels executed can be more than the screen_size, protect
	// against bad access
	if (i >= (size_t)argn->screen_size.x * (size_t)argn->screen_size.y)
		return ;

	float x = (float)(i % argn->screen_size.x);
	float y = (float)(i / argn->screen_size.x);

	t_ray		ray;

	// ray queue to emulate recursion
	t_ray		queue[MAX_RAY_COUNT];
	int			queue_pos = 0;

	int aa_x;
	int aa_y;
	int count = 0;

	float4 color = (float4)(0, 0, 0, 0);

	// antialias loop
	for (aa_y = 0; aa_y < argn->antialias; aa_y++)
	{
		for (aa_x = 0; aa_x < argn->antialias; aa_x++)
		{
			float2 aa;

			// subpixel positions
			aa.x = x + (aa_x - argn->antialias / 2.0f) / argn->antialias;
			aa.y = y + (aa_y - argn->antialias / 2.0f) / argn->antialias;

			// get our subray
			ray.direction = NORMALIZE(cam->vpul + NORMALIZE(cam->right) * (aa.x) - NORMALIZE(cam->up) * (aa.y));
			ray.origin = cam->pos;
			ray.dist = MAXFLOAT;
			ray.type = ORIGIN;
			ray.depth = 0;
			ray.weight = 1.0f;
			ray.primitive_id = NO_PRIMITIVE;
			ray.r_index = 1.0f;
			ray.transparency = (float4)(1, 1, 1, 0);

			PUSH_RAY(queue, ray, queue_pos);

			// start iterating over our emulated stack
			while (queue_pos > 0)
			{
				t_ray cur_ray;
				float4 cur_color = (float4)(0, 0, 0, 0);
				float4 collision;

				// get our ray
				POP_RAY(queue, cur_ray, queue_pos);

				// raytrace!
				int result;
				int cur_id = raytrace(&cur_ray, &cur_color, &collision, &result, objects, lights, argn);

				// do things based on ray type
				switch (cur_ray.type)
				{
					case ORIGIN:
						color += cur_color * cur_ray.weight;
						break;
					case REFLECTED:
						color += cur_color * cur_ray.weight * objects[cur_ray.primitive_id].color * cur_ray.transparency;
						break;
					case REFRACTED:
						color += cur_color * cur_ray.weight * cur_ray.transparency;
						break;
				}
				count++;

				// if we have exceeded our depth or didnt hit anything, skip
				if (cur_ray.depth >= TRACE_DEPTH || cur_id == -1)
					continue;

				// beautiful reflections
				float refl = objects[cur_id].reflection;
				float4 normal = get_normal(&objects[cur_id], collision);
				if (refl > 0.0f)
				{
					float4 reflect = cur_ray.direction - 2.0f * DOT(cur_ray.direction, normal) * normal;

					t_ray r_ray;
					r_ray.dist = MAXFLOAT;
					r_ray.origin = collision + reflect * SHADOW_E;
					r_ray.direction = reflect;
					r_ray.depth = cur_ray.depth + 1;
					r_ray.weight = refl * cur_ray.weight;
					r_ray.type = REFLECTED;
					r_ray.primitive_id = cur_id;
					r_ray.r_index = cur_ray.r_index;
					r_ray.transparency = cur_ray.transparency;

					if (r_ray.weight > EPSILON)
						PUSH_RAY(queue, r_ray, queue_pos);
				}

				// refract it yeah :(?
				/*
				float refr = objects[cur_id].refraction;
				if (refr > 0.0f)
				{
					float r_index = objects[cur_id].r_index;
					float n = cur_ray.r_index / r_index;
					normal *= result;
					float cosi = -DOT(normal, cur_ray.direction);
					float cost2 = 1.0f - n * n * (1.0f - cosi * cosi);
					if (cost2 > EPSILON)
					{
						float4 refract = n * cur_ray.direction + (n * cosi - sqrt(cost2)) * normal;
						t_ray r_ray;
						r_ray.dist = MAXFLOAT;
						r_ray.origin = collision + refract * EPSILON;
						r_ray.direction = refract;
						r_ray.depth = cur_ray.depth + 1;
						r_ray.weight = cur_ray.weight;
						r_ray.type = REFRACTED;
						r_ray.primitive_id = cur_id;
						r_ray.r_index = r_index;
						r_ray.transparency = cur_ray.transparency * (exp(objects[cur_id].color * (-cur_ray.dist))) * objects[cur_id].transparency;
						PUSH_RAY(queue, r_ray, queue_pos);
					}
				}*/
			}
		}
	}

	// divide by the total amount of samples
	color /= count;

	// apply color filter
	if (argn->filter != NONE)
		color = color_filter(color, argn->filter);

	// return the pixel color
	out[i] = color_to_int(color);
}
