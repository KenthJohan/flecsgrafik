#include <flecs.h>
#include <stdio.h>

#if defined(WIN32)
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif
static void csc_crossos_enable_ansi_color ()
{
#if defined(WIN32)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode (hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode (hOut, dwMode);
#endif
}

typedef struct
{
	float v;
} nutrients;

typedef struct
{
	float v;
} hunger;

typedef struct
{
	float v;
} eats1;

ECS_COMPONENT_DECLARE(nutrients);
ECS_COMPONENT_DECLARE(hunger);
ECS_COMPONENT_DECLARE(eats1);
ECS_TAG_DECLARE(eats);

/*
void cb(ecs_iter_t *it)
{
	ecs_id_t id = ecs_term_id(it, 1);
	ecs_entity_t o = ecs_pair_object(it->world, id);
	ecs_entity_t r = ecs_pair_relation(it->world, id);
	for (int i = 0; i < it->count; i ++)
	{
		char * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		printf("%s, (%s,%s) : %s\n",
		ecs_get_name(it->world, it->entities[i]),
		ecs_get_name(it->world, r),
		ecs_get_name(it->world, o),
		typestr);
	}
}
*/

void cb(ecs_iter_t *it)
{
	hunger *n = ecs_term(it, hunger, 1);
	ecs_entity_t o = ecs_pair_object(it->world, ecs_term_id(it, 2));
	for (int i = 0; i < it->count; i ++)
	{
		//char * typestr = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		//printf("%s\n", typestr);
		//printf("%s eats %s\n", ecs_get_name(it->world, it->entities[i]), ecs_get_name(it->world, o));
		n[i].v -= ecs_get(it->world, o, nutrients)->v;
	}
}

int main(int argc, char * argv[])
{
	csc_crossos_enable_ansi_color();
	ecs_world_t *world = ecs_init();
	ECS_COMPONENT_DEFINE (world, nutrients);
	ECS_COMPONENT_DEFINE (world, hunger);
	ECS_COMPONENT_DEFINE (world, eats1);
	ECS_TAG_DEFINE(world, eats);

	ecs_entity_t f1 = ecs_set_name(world, 0, "Apples");
	ecs_entity_t f2 = ecs_set_name(world, 0, "Pears");
	ecs_entity_t f3 = ecs_set_name(world, 0, "Pomegranate");
	ecs_entity_t p1 = ecs_set_name(world, 0, "Alice");
	ecs_entity_t p2 = ecs_set_name(world, 0, "Bob");
	ecs_set(world, p1, hunger, {10.0f});
	ecs_set(world, p2, hunger, {10.0f});
	ecs_set(world, f1, nutrients, {5.0f});
	ecs_set(world, f2, nutrients, {6.0f});
	ecs_set(world, f3, nutrients, {7.0f});

	ECS_OBSERVER(world, cb, EcsOnAdd, hunger, (eats1, *));

	ecs_add_pair(world, p1, eats, f1);
	ecs_add_pair(world, p1, eats, f2);
	ecs_add_pair(world, p1, eats, f3);
	ecs_add_pair(world, p2, eats, f1);
	ecs_add_pair(world, p2, eats, f2);

	ecs_set_pair(world, p1, eats1, f1, {1.0f});
	ecs_set_pair(world, p1, eats1, f2, {1.0f});
	ecs_set_pair(world, p1, eats1, f3, {1.0f});
	ecs_set_pair(world, p2, eats1, f1, {1.0f});
	ecs_set_pair(world, p2, eats1, f2, {1.0f});

	printf("%f\n", ecs_get(world, p1, hunger)->v);
	ecs_progress(world, 0);

	return ecs_fini(world);
}
