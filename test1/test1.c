#include <flecs.h>
#include <stdio.h>

void cb(ecs_iter_t *it)
{
	ecs_id_t id = ecs_term_id(it, 1);
	ecs_entity_t o = ecs_pair_object(it->world, id);
	ecs_entity_t r = ecs_pair_relation(it->world, id);
	for (int i = 0; i < it->count; i ++)
	{
		char * c = ecs_type_str(it->world, ecs_get_type(it->world, it->entities[i]));
		printf("%s, (%s,%s) : %s\n",
			   ecs_get_name(it->world, it->entities[i]),
			   ecs_get_name(it->world, r),
			   ecs_get_name(it->world, o),
			   c);
	}
}

int main(int argc, char * argv[])
{
	ecs_world_t *world = ecs_init();

	ecs_entity_t Bob = ecs_new_id(world);
	ecs_entity_t Alice = ecs_new_id(world);
	ecs_entity_t Eats = ecs_new_id(world);
	ecs_entity_t Apples = ecs_new_id(world);
	ecs_entity_t Pears = ecs_new_id(world);

	ecs_set(world, Bob, EcsName, {.value = "Bob"});
	ecs_set(world, Alice, EcsName, {.value = "Alice"});
	ecs_set(world, Eats, EcsName, {.value = "Eats"});
	ecs_set(world, Apples, EcsName, {.value = "Apples"});
	ecs_set(world, Pears, EcsName, {.value = "Pears"});

	ecs_filter_desc_t filter = (ecs_filter_desc_t){.terms = {{.id = ecs_pair(Eats, EcsWildcard)}}};

	ecs_observer_init(world, &(ecs_observer_desc_t){
	.filter = filter,
	.events = {EcsOnAdd},
	.callback = cb
	});

	printf("ecs_observer_init: \n");
	ecs_add_pair(world, Bob, Eats, Apples);
	ecs_add_pair(world, Alice, Eats, Apples);
	ecs_add_pair(world, Alice, Eats, Pears);
	//ecs_remove_pair(world, Alice, Eats, Pears);
	ecs_add_pair(world, Alice, Eats, Pears);
	ecs_add_pair(world, Alice, Eats, Bob);

	printf("ecs_query_init: \n");
	ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){.filter = filter});
	ecs_iter_t it = ecs_query_iter(q);
	while (ecs_query_next(&it))
	{
		cb(&it);
	}

	return ecs_fini(world);
}
