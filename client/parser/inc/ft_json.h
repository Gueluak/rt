/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_json.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/14 18:48:15 by hmartzol          #+#    #+#             */
/*   Updated: 2017/01/19 23:24:29 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_JSON_H
# define FT_JSON_H

typedef struct s_json_object	t_json_object;
typedef struct s_json_pair		t_json_pair;
typedef struct s_json_value		t_json_value;
typedef struct s_json_array		t_json_array;
typedef struct s_json_string	t_json_string;

typedef enum e_json_value_type	t_json_value_type;

enum							e_json_value_type
{
	none, null, boolean, array, object, integer, number, string
};

/*
** t_json_string: (string with potential no terminating '\0')
**  -length: explicit
**  -ptr: pointer to allocated memory of size 'length' and containing an ascii
**   string
*/

struct							s_json_string
{
	unsigned long		length;
	char				*ptr;
};

/*
** t_json_value: (value container with type identifier)
**  -parent: pointer to the parent value containing the current value, or a
**   pointer to himself if the value is the root of all values
**  -type: enum of all type that can be pointed by ptr
**  -ptr: pointer to the data contained
*/

struct							s_json_value
{
	t_json_value		*parent;
	t_json_value_type	type;
	void				*ptr;
};

/*
** t_json_pair: (pair container, stores values with a string for key)
**  -key: json_string (length known)
**  -value: pointer to a value container
*/

struct							s_json_pair
{
	t_json_string		*key;
	t_json_value		*value;
};

/*
** t_json_object: (object container, stores 'nb_pairs' concidered unordered
** pairs)
**  -nb_pairs: total number of pairs stored in this object
**  -pair: pointer to allocated memory of size 'nb_pairs' and containing
**   pointers to pairs
*/

struct							s_json_object
{
	unsigned long		nb_pairs;
	t_json_pair			**pair;
};

/*
** t_json_array: (array container, stores 'nb_values' concidered ordered values)
**  -nb_values: total number of values stored in this array
**  -value: pointer to allocated memory of size 'nb_values' and containing
**   pointers to values
*/

struct							s_json_array
{
	unsigned long		nb_values;
	t_json_value		**value;
};

t_json_value					*ft_json_parse_src(const char *src);

t_json_value					*ft_json_new_value(const char *src,
		const unsigned long length, unsigned long *pos, t_json_value *parent);

t_json_value_type				ft_json_evaluate_value_type(const char *src,
								const unsigned long length, unsigned long pos);

t_json_value					*ft_json_pack_value(t_json_value *parent,
											t_json_value_type type, void *ptr);

t_json_value					*ft_json_new_number(const char *src,
		const unsigned long length, unsigned long *pos, t_json_value *parent);

t_json_string					*ft_json_new_string(const char *src,
								const unsigned long length, unsigned long *pos);

t_json_value					*ft_json_new_array(const char *src,
		const unsigned long length, unsigned long *pos, t_json_value *parent);

unsigned long					ft_json_evaluate_array_size(const char *src,
								const unsigned long length, unsigned long pos);

t_json_value					*ft_json_new_object(const char *src,
		const unsigned long length, unsigned long *pos, t_json_value *parent);

unsigned long					ft_json_evaluate_object_size(const char *src,
								const unsigned long length, unsigned long pos);

t_json_pair						*ft_json_new_pair(const char *src,
		const unsigned long length, unsigned long *pos, t_json_value *parent);

int								ft_json_print(t_json_value *node);
int								ft_json_print_array(t_json_array *array);
int								ft_json_print_object(t_json_object *obj);
int								ft_json_print_pair(t_json_pair *pair);
int								ft_json_print_string(t_json_string *str);

void							ft_json_free(t_json_value *node);
void							ft_json_free_string(t_json_string *string);
void							ft_json_free_array(t_json_array *array);
void							ft_json_free_object(t_json_object *object);
void							ft_json_free_pair(t_json_pair *pair);

#endif
