#pragma once
#include "flecs.h"
#include "csc/csc_math.h"
//Shared libraries
#include <stdio.h>
#include <enet/enet.h>

#ifdef __cplusplus
extern "C" {
#endif


ECS_STRUCT(Point, {
	int32_t x;
	int32_t y;
});

ECS_STRUCT(Line, {
	Point start;
	Point stop;
});



ecs_entity_t comp_position;
ecs_entity_t comp_velocity;
ecs_entity_t comp_acceleration;
ecs_entity_t comp_quantity;
ecs_entity_t comp_v3f32;
ecs_entity_t comp_m3f32;

void init_components(ecs_world_t * world)
{
	ECS_META_COMPONENT (world, Point);
	ECS_META_COMPONENT (world, Line);
	{
		ecs_component_desc_t desc =
		{
		.entity = {.name = "v3f32"},
		.size = sizeof(v3f32),
		.alignment = ECS_ALIGNOF(v3f32)
		};
		comp_v3f32 = ecs_component_init(world, &desc);
	}
	{
		ecs_component_desc_t desc =
		{
		.entity = {.name = "m3f32"},
		.size = sizeof(m3f32),
		.alignment = ECS_ALIGNOF(m3f32)
		};
		comp_m3f32 = ecs_component_init(world, &desc);
	}
	{
		ecs_entity_desc_t desc =
		{
		.name = "position"
		};
		comp_position = ecs_entity_init(world, &desc);
	}
	{
		ecs_entity_desc_t desc =
		{
		.name = "velocity"
		};
		comp_velocity = ecs_entity_init(world, &desc);
	}
	{
		ecs_entity_desc_t desc =
		{
		.name = "acceleration"
		};
		comp_acceleration = ecs_entity_init(world, &desc);
	}
	{
		ecs_type_desc_t desc =
		{
		.ids_expr = "position, velocity, acceleration"
		};
		comp_quantity = ecs_type_init(world, &desc);
	}
}


struct idpack
{
	uint32_t id;
	uint8_t data[0];
};

#define ENTMAP_MAX 100
struct entmap
{
	ecs_entity_t ents[ENTMAP_MAX];
	ecs_entity_t comps[ENTMAP_MAX];
	size_t sizes[ENTMAP_MAX];
};

void entmap_init(ecs_world_t * world, entmap * map)
{
	for (int i = 0; i < ENTMAP_MAX; ++i)
	{
		map->ents[i] = ecs_new(world, 0);
	}
}


void receive_handle(ecs_world_t * world, entmap * emap, uint32_t length, void const * data)
{
	do
	{
		idpack const * pack = (idpack const *)data;
		uint32_t eid = emap->ents[pack->id]; //Entity id
		ecs_entity_t cid = emap->comps[eid]; //Component id
		ecs_entity_t csize = emap->sizes[eid]; //Component size
		ecs_set_id(world, eid, cid, csize, pack->data);
		length -= (csize <= length) ? csize : length; //Wrap-under protection
	}
	while(length);
}


void receive(ecs_world_t * world, ENetHost * server)
{
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	if (enet_host_service (server, & event, 1) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf ("A new client connected from %x:%u.\n",
			event.peer -> address.host,
			event.peer -> address.port);
			/* Store any relevant client information here. */
			event.peer -> data = (void*)"Client information";
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
			event.packet -> dataLength,
			event.packet -> data,
			event.peer -> data,
			event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy (event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf ("%s disconnected.\n", event.peer -> data);
			/* Reset the peer's client information. */
			event.peer -> data = NULL;
		default:
			break;
		}
	}
}








ENetHost * server_connect()
{
	ENetHost * server;
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 9002;
	size_t peerCount = 32; //the maximum number of peers that should be allocated for the host.
	size_t channelLimit = 2; //the maximum number of channels allowed; if 0, then this is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
	enet_uint32 incomingBandwidth = 0; //downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
	enet_uint32 outgoingBandwidth = 0; //upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
	server = enet_host_create (&address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
	if (server == NULL)
	{
		fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
		exit (EXIT_FAILURE);
	}
}

















#ifdef __cplusplus
}
#endif
