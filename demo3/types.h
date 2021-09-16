#pragma once
#include "../flecs/flecs.h"




typedef struct
{
	float x;
	float y;
} position_2f32;

typedef struct
{
	position_2f32 a;
	position_2f32 b;
} quad_4f32;

typedef struct
{
	position_2f32 a;
	position_2f32 b;
} world_quad_4f32;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} position_4f32;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} world_position_4f32;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} local_position_4f32;

typedef struct
{
	float w;
	float h;
} rectangle_2f32;

typedef struct
{
	float w;
	float h;
} local_rectangle_2f32;

typedef struct
{
	float length;
} length_f32;

typedef struct
{
	float w;
	float h;
} world_rectangle_2f32;

typedef struct
{
	float w;
	float h;
} textsize_2f32;

typedef struct
{
	float w;
	float h;
} window_rectangle_2f32;

typedef struct
{
	float l;
	float r;
	float b;
	float t;
} padding_4f32;

typedef struct
{
	float u;
	float v;
	float w;
	float h;
} uvwh_4f32;

typedef struct
{
	uint32_t layer;
} texture_layer;

typedef struct
{
	char const * text;
} text_cstring;

typedef struct
{
	uint32_t vbo;
} opengl_vbo;


ECS_COMPONENT_DECLARE(local_position_4f32);
ECS_COMPONENT_DECLARE(world_position_4f32);
ECS_COMPONENT_DECLARE(quad_4f32);
ECS_COMPONENT_DECLARE(world_quad_4f32);
ECS_COMPONENT_DECLARE(position_4f32);
ECS_COMPONENT_DECLARE(world_rectangle_2f32);
ECS_COMPONENT_DECLARE(local_rectangle_2f32);
ECS_COMPONENT_DECLARE(rectangle_2f32);
ECS_COMPONENT_DECLARE(window_rectangle_2f32);
ECS_COMPONENT_DECLARE(uvwh_4f32);
ECS_COMPONENT_DECLARE(padding_4f32);
ECS_COMPONENT_DECLARE(text_cstring);
ECS_COMPONENT_DECLARE(texture_layer);
ECS_COMPONENT_DECLARE(textsize_2f32);
ECS_COMPONENT_DECLARE(length_f32);
ECS_COMPONENT_DECLARE(opengl_vbo);


ECS_TAG_DECLARE(draw_rectangle);
ECS_TAG_DECLARE(draw_text);





static void types_ecsinit(ecs_world_t *world)
{
	ECS_COMPONENT_DEFINE (world, local_position_4f32);
	ECS_COMPONENT_DEFINE (world, world_position_4f32);
	ECS_COMPONENT_DEFINE (world, world_rectangle_2f32);
	ECS_COMPONENT_DEFINE (world, local_rectangle_2f32);
	ECS_COMPONENT_DEFINE (world, position_4f32);
	ECS_COMPONENT_DEFINE (world, quad_4f32);
	ECS_COMPONENT_DEFINE (world, world_quad_4f32);
	ECS_COMPONENT_DEFINE (world, rectangle_2f32);
	ECS_COMPONENT_DEFINE (world, textsize_2f32);
	ECS_COMPONENT_DEFINE (world, padding_4f32);
	ECS_COMPONENT_DEFINE (world, window_rectangle_2f32);
	ECS_COMPONENT_DEFINE (world, uvwh_4f32);
	ECS_COMPONENT_DEFINE (world, text_cstring);
	ECS_COMPONENT_DEFINE (world, texture_layer);
	ECS_COMPONENT_DEFINE (world, length_f32);
	ECS_COMPONENT_DEFINE (world, opengl_vbo);
	ECS_TAG_DEFINE (world, draw_rectangle);
	ECS_TAG_DEFINE (world, draw_text);
}

