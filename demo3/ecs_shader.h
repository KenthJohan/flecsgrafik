#pragma once
#include <flecs.h>
#include <stdio.h>
#include "types.h"

static void ECSCB_opengl_attach (ecs_iter_t *it)
{
	opengl_program *p = ecs_term(it, opengl_program, 1);
	ecs_entity_t o = ecs_pair_object(it->world, ecs_term_id(it, 2));
	opengl_shader *s = ecs_get(it->world, o, opengl_shader);
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		char const * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		//const opengl_program *p = ecs_get_pair_object(it->world, it->entities[i], EcsChildOf, opengl_program);
		//printf("Attach %s %s\n", name, typestr);
		printf("Attaching %s(%i) - %s(%i)\n", name, p[i], ecs_get_name(it->world, o), (s ? s[0].id : -1));
		//printf("%i %s, p:%i\n", shader[i].id, filename[i].text, p ? p->id : -1);
		//printf("%s: \n", ecs_get_name(it->world, it->entities[i]));
	}
}

static void ECSCB_opengl_detach (ecs_iter_t *it)
{
	ecs_entity_t o = ecs_pair_object(it->world, ecs_term_id(it, 1));
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		char const * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		//const opengl_program *p = ecs_get_pair_object(it->world, it->entities[i], EcsChildOf, opengl_program);
		//printf("%s %s\n", name, typestr);
		printf("Detach %s - %s\n", name, ecs_get_name(it->world, o));
		//printf("%i %s, p:%i\n", shader[i].id, filename[i].text, p ? p->id : -1);
		//printf("%s: \n", ecs_get_name(it->world, it->entities[i]));
	}
}


static void ECSCB_opengl_createprogram (ecs_iter_t *it)
{
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		char const * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		//const opengl_program *p = ecs_get_pair_object(it->world, it->entities[i], EcsChildOf, opengl_program);
		printf("Create program %s %s\n", name, typestr);
		//printf("Create %s - %s\n", name, ecs_get_name(it->world, o));
		//printf("%i %s, p:%i\n", shader[i].id, filename[i].text, p ? p->id : -1);
		//printf("%s: \n", ecs_get_name(it->world, it->entities[i]));
	}
}



static void ECSCB_opengl_createshader (ecs_iter_t *it)
{
	opengl_shader *s = ecs_term(it, opengl_shader, 1);
	EcsIdentifier *n = ecs_term(it, EcsIdentifier, 2);
	for (int i = 0; i < it->count; i ++)
	{
		char const * name = ecs_get_name(it->world, it->entities[i]);
		char const * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		//const opengl_program *p = ecs_get_pair_object(it->world, it->entities[i], EcsChildOf, opengl_program);
		//printf("%s %s\n", name, typestr);
		printf("Create shader %s\n", n->value);
		//printf("Create %s - %s\n", name, ecs_get_name(it->world, o));
		//printf("%i %s, p:%i\n", shader[i].id, filename[i].text, p ? p->id : -1);
		//printf("%s: \n", ecs_get_name(it->world, it->entities[i]));
	}
}
