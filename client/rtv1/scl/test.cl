typedef enum	e_prim_type
{
	SPHERE = 0, PLANE = 1, CONE = 2, CYLINDER = 3
}				t_prim_type;

typedef struct	s_primitive
{
	t_prim_type		type;
	float4		position;
	float4		direction;
	float4		color;
	float		radius;
}				t_primitive;

typedef struct	s_light
{
	float4		position;
	float4		color;
	float		direct;
}				t_light;

typedef struct	s_argn		//structure containing the limit of out, rays and objects
{							//note the corespondance with the C structure (types end position of variables correspond, but types have a preceding "cl_")
	int2	screen_size;	//total screen size in pixels (accesed by .x and .y)
	int		nb_objects;		//total number of objects in the scene
	int		nb_lights;		//total number of lights in the scene
}				t_argn;		//norm42 magle

typedef struct	s_camera	//note: yes, this structure is not equivalent to s_camera in rtv1.h
{
	float4	pos;			//
	float4	dir;			//
	float4	up;				//
	float4	right;			//
	float4	vpul;			//
	int2	vp_size;		//
}				t_camera;

typedef struct	s_ray
{
	float4		origin;
	float4		direction;
}				t_ray;

//j'aime les commentaires, et vous? :p
/*
   NOTE HYPER IMPORTANTE:
   pour la compatibilité AMD, il faut continuer d'utiliser les flags __global et les pointeurs
   (AMD ne copie pas les déférencements de pointeurs dans la stack, ce qui fait que les objets et la lumière n'étaient pas envoyé aux calculs)
   */
inline float	addv(float4 v);

int		plane_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		sphere_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		cylinder_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		cone_intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		intersect(__global t_primitive *obj, t_ray *ray, float *dist);
int		solve_quadratic(float a, float b, float c, float *dist);
float4	get_normal(__global t_primitive *obj, float4 point);
float4	phong(float4 dir, float4 norm);
int		color_to_int(float4 color);

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

// minimum direct lighting coefficient
#ifndef MIN_DIRECT
# define MIN_DIRECT 0.95f
#endif

// antialias 0 = 1x1 (none), 1 = 3x3, 2 = 5x5, 3 = 7x7, etc.
#ifndef AA
# define AA 0
#endif

#define SEPIA 0

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
	float c = DOT(pos, pos) - pow(obj->radius, 2);
	return solve_quadratic(a, b, c, dist);
}

int		cylinder_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = obj->position - ray->origin;
	float4 p = CROSS(pos, obj->direction);
	float4 r = CROSS(ray->direction, obj->direction);

	float a = DOT(r, r);
	float b = 2 * DOT(r, p);
	float c = DOT(p, p) - pow(obj->radius, 2) * DOT(obj->direction, obj->direction);
	return solve_quadratic(a, b, c, dist);
}

int		cone_intersect(__global t_primitive *obj, t_ray *ray, float *dist)
{
	float4 pos = ray->origin - obj->position;
	float4 dir = -ray->direction;

	float r = 1 + pow(obj->radius, 2);
	float dd = DOT(dir, obj->direction);

	float a = DOT(dir, dir) - (r * pow(dd, 2));
	float b = 2 * (DOT(dir, pos) - (r * dd * DOT(pos, obj->direction)));
	float c = DOT(pos, pos) - (r * pow(DOT(pos, obj->direction), 2));

	return solve_quadratic(a, b, c, dist);
}

int		solve_quadratic(float a, float b, float c, float *dist)
{
	float delta = b * b - 4.0f * a * c;
	if (delta < EPSILON)
		return (0);

	delta = sqrt(delta);
	float x1 = (b - delta) / (2.0f * a);
	float x2 = (b + delta) / (2.0f * a);
	if (x2 > EPSILON)
	{
		if (x1 < 0)
		{
			if (x2 < *dist)
			{
				*dist = x2;
				return (-1);
			}
		}
		else
		{
			if (x1 < *dist)
			{
				*dist = x1;
				return (1);
			}
		}
	}
	return (0);
}

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
	}


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
	float t;

	switch (obj->type)
	{
		case SPHERE:
			n = NORMALIZE(point - obj->position);
			break;
		case PLANE:
			n = obj->direction;
			break;
		case CYLINDER:
		case CONE:
			t = DOT(-obj->direction, obj->position) + DOT(point, obj->direction) / addv(pow(obj->direction, 2));
			n = NORMALIZE(point - (obj->position + obj->direction * t));
			break;
	}
	n.y = n.y + cos(point.y / 10.0f) * LENGTH(n) / 10.0f;
	return (n);
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

inline float4		sepia_color(float4 color)
{
	float4 out;

	out.x = color.x * 0.393f + color.y * 0.769f + color.z * 0.189f;
	out.y = color.x * 0.349f + color.y * 0.686f + color.z * 0.168f;
	out.z = color.x * 0.272f + color.y * 0.534f + color.z * 0.131f;

	return (out);
}

__kernel void	example(							//main kernel, called for each ray
		__global int *out,				//int bitmap, his size is equal to screen_size.x * screen_size.y
		__global t_argn *argn,			//structure containing important info on how to acces out, rays and objects
		__global t_primitive *objects,	//set of objects in the scene, the number of objects (and so the limit of this array), is stored in argn
		__global t_light *lights,
		__global t_camera *cam)			//vector3 rays stored has float4 for convenience, the size of the array is screen_size.x * screen_size.y
{
	//mode 2: we use 1D Kernels:
	size_t i = get_global_id(0); //id of the kernel in the global call
	out[i] = 0;

	// the amount of kernels executed can be more than the screen_size, protect
	// against bad access
	if (i >= (size_t)argn->screen_size.x * (size_t)argn->screen_size.y)
		return ;

	float x = (float)(i % argn->screen_size.x);
	float y = (float)(i / argn->screen_size.x);
	t_ray ray;
	ray.origin = cam->pos;

	int aa_x;
	int aa_y;
	float4 color = (float4)(0, 0, 0, 0);

	// antialias
	int samples = 0;
	for (aa_y = -AA; aa_y <= AA; aa_y++)
	{
		for (aa_x = -AA; aa_x <= AA; aa_x++)
		{
			ray.direction = NORMALIZE(cam->vpul + NORMALIZE(cam->right) * (x + (aa_x / ((AA + 1) * 2.0f))) - NORMALIZE(cam->up) * (y + (aa_y / ((AA + 1) * 2.0f))));

			float dist = MAXFLOAT;
			int id = -1;

			int cur;
			int	t_hit;
			int s_hit;
			for (cur = 0; cur < argn->nb_objects; cur++)
			{
				if ((t_hit = intersect(&objects[cur], &ray, &dist)))
				{
					id = cur;
					s_hit = t_hit;
				}
			}

			// calculate direct lighting
			__global t_primitive *prim;
			float4 collision;
			float4 norm;

			float4 cur_color = (float4)(0, 0, 0, 0);
			float4 add_color = (float4)(0, 0, 0, 0);

			// if we hit something, get the related variables
			if (id != -1)
			{
				prim = &objects[id];
				collision = ray.origin + ray.direction * dist;

				// get the normal for this intersection point
				norm = get_normal(prim, collision);

				// invert the normal if we're inside the primitive
				if (s_hit == -1)
					norm = -norm;

				// add ambient light
				cur_color += prim->color * 0.1f; // TODO: ambient amount should be configurable
			}

			// lights, maestro!
			int cur_l;
			t_ray ray_l;
			float dist_l;
			float scal;
			float dist2 = dist;
			for (cur_l = 0; cur_l < argn->nb_lights; cur_l++)
			{
				t_light light = lights[cur_l];

				// calculate direct lighting
				if (light.direct > EPSILON)
				{
					dist_l = LENGTH(light.position - ray.origin);
					ray_l.direction = NORMALIZE(light.position - ray.origin);
					ray_l.origin = ray.origin;

					// if our light is closer than intersect
					if (dist_l < dist2)
					{
						// if our light is between us and the object, dot will
						// be positive
						scal = DOT(ray_l.direction, ray.direction);
						if (scal > EPSILON)
						{
							scal = pow(scal, dist_l / 50);
							if (scal > MIN_DIRECT)
								add_color += (light.color * (scal - MIN_DIRECT) / (1.0f - MIN_DIRECT)) * light.direct;
						}
					}
				}

				// if we didn't hit anything, don't calculate light for object
				if (id == -1)
					continue;

				// check if our light source is blocked by an object
				// shadows start a tiny amount from the actual sphere to prevent
				// rounding errors
				dist_l = LENGTH(light.position - collision);
				ray_l.direction = NORMALIZE(light.position - collision);
				ray_l.origin = collision + ray_l.direction * SHADOW_E;
				int hit = 0;

				dist = MAXFLOAT;
				for (cur = 0; cur < argn->nb_objects; cur++)
				{
					if ((hit = intersect(&objects[cur], &ray_l, &dist)))
					{
						if (dist > EPSILON && dist < dist_l) // it is between us
							break ;
						else // it is behind us
							hit = 0;
					}
				}

				// did we hit something? then it's definitely a shadow, hey!
				if (hit)
					continue ;

				// diffuse lighting
				if ((scal = DOT(ray_l.direction, norm)) > EPSILON)
					cur_color += prim->color * light.color * scal; // TODO: diffuse coef

				// specular highlights (needs pow to make the curve sharper)
				float4 ir = phong(-ray_l.direction, norm);
				if (scal > EPSILON && (scal = DOT(ray_l.direction, ir)) > EPSILON)
					cur_color += light.color * pow(scal, 20); // TODO: specular coef
			}

			color += clamp(cur_color / ((float)argn->nb_lights * 0.5f), 0.0f, 1.0f); // TODO: attenuate
			color += clamp(add_color, 0.0f, 1.0f);
			samples++;
		}
	}

	// divide by the total amount of samples
	color /= samples;

	if (SEPIA)
		color = sepia_color(color);

	out[i] = color_to_int(color);
}
