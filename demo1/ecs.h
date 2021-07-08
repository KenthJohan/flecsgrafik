#pragma once

#include <flecs.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <cglm/cam.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "csc/csc_math.h"
#include "csc/csc_xlog.h"
#include "csc/csc_gl.h"
#include "csc/csc_gft.h"


typedef v2f32 Rectangle2;
typedef v2f32 Position2;
typedef v2f32 Scale2;

typedef struct Message
{
	char const * value;
} Message;

typedef struct Text
{
	char const * value;
} Text;

typedef struct Filename
{
	char const * value;
} Filename;


typedef struct OpenGL_Shader
{
	GLint id;
} OpenGL_Shader;

typedef struct OpenGL_Program
{
	GLint id;
} OpenGL_Program;

typedef struct OpenGL_VAO
{
	GLint id;
} OpenGL_VAO;

typedef struct OpenGL_VBO
{
	GLint id;
} OpenGL_VBO;

typedef struct OpenGL_Texture2D
{
	GLint id;
} OpenGL_Texture2D;

typedef struct Freetype_Face
{
	FT_Face face;
} Freetype_Face;

typedef struct Freetype_Library
{
	FT_Library library;
} Freetype_Library;

typedef struct Textbox
{
	int id;
} Textbox;


ECS_COMPONENT_DECLARE(Rectangle2);
ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Message);
ECS_COMPONENT_DECLARE(Filename);
ECS_COMPONENT_DECLARE(Textbox);
ECS_COMPONENT_DECLARE(Text);

ECS_COMPONENT_DECLARE(OpenGL_Shader);
ECS_COMPONENT_DECLARE(OpenGL_Program);
ECS_COMPONENT_DECLARE(OpenGL_VAO);
ECS_COMPONENT_DECLARE(OpenGL_VBO);
ECS_COMPONENT_DECLARE(OpenGL_Texture2D);

ECS_COMPONENT_DECLARE(Freetype_Face);
ECS_COMPONENT_DECLARE(Freetype_Library);


static void sys_Message_Print(ecs_iter_t *it)
{
	Message *msg = ecs_term(it, Message, 1);
	for (int i = 0; i < it->count; i ++)
	{
		printf("%s\n", msg[i].value);
	}
}


static void sys_create_shader_fromfilename (ecs_iter_t *it)
{
	OpenGL_Shader * shader = ecs_term (it, OpenGL_Shader, 1);
	Filename * filename = ecs_term (it, Filename, 2);
	for (int i = 0; i < it->count; i ++)
	{
		XLOG (XLOG_INF, XLOG_GENERAL, "Loading shader %s", filename[i].value);
		shader[i].id = csc_gl_shader_from_file (filename[i].value);
		//printf ("sys_create_shader_fromfilename %i\n", shader[i].id);
		//printf ("sys_create_shader_fromfilename %s\n", filename[i].value);
	}
}


static void sys_create_program (ecs_iter_t *it)
{
	OpenGL_Program * program = ecs_term (it, OpenGL_Program, 1);
	OpenGL_Shader * shader = ecs_term (it, OpenGL_Shader, 2);//Shared
	for (int i = 0; i < it->count; i ++)
	{
		XLOG (XLOG_INF, XLOG_GENERAL, "Add shader %i to %i", shader[0].id, program[i].id);
	}
	//ecs_enable (it->world, it->system, false);
}



static void sys_freetype_face_from_filename (ecs_iter_t *it)
{
	Freetype_Face * face = ecs_term (it, Freetype_Face, 1);
	Filename * filename = ecs_term (it, Filename, 2);
	Freetype_Library const * ft = ecs_singleton_get (it->world, Freetype_Library);
	ASSERT_NOTNULL (ft);
	for (int i = 0; i < it->count; i ++)
	{
		XLOG (XLOG_INF, XLOG_GENERAL, "Loading font %s", filename[i].value);
		//face[i].id = csc_gl_shader_from_file (filename[i].value);
		//printf ("sys_create_shader_fromfilename %i\n", shader[i].id);
		//printf ("sys_create_shader_fromfilename %s\n", filename[i].value);
		if (FT_New_Face(ft->library, filename[i].value, 0, &face[i].face))
		{
			XLOG (XLOG_ERR, XLOG_GENERAL, "Failed to load font");
			ASSERT(0);
		}
	}
}


static void system_text_draw (ecs_iter_t *it)
{
	ECS_COLUMN (it, Text, text, 1);
	ECS_COLUMN (it, Position2, p, 2);
	ECS_COLUMN (it, Scale2, s, 3);
	ECS_COLUMN (it, OpenGL_Program, program, 4);
	ECS_COLUMN (it, OpenGL_VAO, vao, 5);
	ECS_COLUMN (it, OpenGL_Texture2D, tex, 6);
	ECS_COLUMN (it, OpenGL_VBO, vbo, 7);
	GLint uniform_tex = glGetUniformLocation (program->id, "tex0");
	GLint uniform_mvp = glGetUniformLocation (program->id, "mvp");

	struct gft_char c[128];
	struct gft_atlas atlas;
	//XLOG (XLOG_INF, XLOG_ECS, "%i", it->count);
	v4f32 pos[6 * 100];
	uint32_t n = 0;
	for (int32_t i = 0; i < it->count; ++i)
	{
		gft_gen_pos (((float*)pos) + 0, 100, 2, text[i].value, c, p[i].x, p[i].y, s[i].x, s[i].y);
		gft_gen_uv (((float*)pos) + 2, 100, 2, text[i].value, c, atlas.w, atlas.h);
	}
	glBindVertexArray (vao->id);
	glUseProgram (program->id);
	glBindTexture (GL_TEXTURE_2D, tex->id);
	glUniform1i (uniform_tex, 0);
	//glUniformMatrix4fv (gtext_ctx.uniform_mvp, 1, GL_FALSE, (const GLfloat *) global_gcam.mvp.m);
	glBindBuffer (GL_ARRAY_BUFFER, vbo->id);
	glBufferData (GL_ARRAY_BUFFER, sizeof(v4f32)*n, pos, GL_DYNAMIC_DRAW);
	glDrawArrays (GL_TRIANGLES, 0, n);
}




void main_ecs_register(ecs_world_t * world)
{
	ECS_COMPONENT_DEFINE (world, Rectangle2);
	ECS_COMPONENT_DEFINE (world, Position2);
	ECS_COMPONENT_DEFINE (world, Message);
	ECS_COMPONENT_DEFINE (world, Filename);
	ECS_COMPONENT_DEFINE (world, Textbox);
	ECS_COMPONENT_DEFINE (world, Text);

	ECS_COMPONENT_DEFINE (world, OpenGL_Shader);
	ECS_COMPONENT_DEFINE (world, OpenGL_VAO);
	ECS_COMPONENT_DEFINE (world, OpenGL_VBO);
	ECS_COMPONENT_DEFINE (world, OpenGL_Program);
	ECS_COMPONENT_DEFINE (world, OpenGL_Texture2D);

	ECS_COMPONENT_DEFINE (world, Freetype_Face);
	ECS_COMPONENT_DEFINE (world, Freetype_Library);

	ecs_observer_init (world, &(ecs_observer_desc_t){
	.filter.terms = {
	{.id = ecs_id(OpenGL_Shader)},
	{.id = ecs_id(Filename)},
	},
	.events = {EcsOnSet},
	.callback = sys_create_shader_fromfilename
	});

	ecs_observer_init (world, &(ecs_observer_desc_t){
	.filter.terms = {
	{.id = ecs_id(Freetype_Face)},
	{.id = ecs_id(Filename)},
	},
	.events = {EcsOnSet},
	.callback = sys_freetype_face_from_filename
	});


	ECS_SYSTEM(world, sys_create_program, EcsOnSet, OpenGL_Program, (*, OpenGL_Shader));
	//ecs_enable (world, sys_create_program, false);


	{
		FT_Library ft = NULL;
		// All functions return a value different than 0 whenever an error occurred
		if (FT_Init_FreeType (&ft))
		{
			XLOG (XLOG_ERR, XLOG_GENERAL, "Could not init FreeType Library");
			ASSERT(0);
		}

		/*
		if (FT_New_Face(ft, "consola.ttf", 0, &gtext_ctx.face))
		{
			XLOG (XLOG_ERR, XLOG_GENERAL, "Failed to load font");
			ASSERT(0);
		}
		FT_Set_Pixel_Sizes (gtext_ctx.face, 0, 24);
		*/
		ecs_singleton_set (world, Freetype_Library, {ft});
		ASSERT_NOTNULL (ecs_singleton_get (world, Freetype_Library)->library);
	}

	{
		ecs_entity_t s1 = ecs_new_id (world);
		ecs_entity_t s2 = ecs_new_id (world);
		ecs_entity_t p1 = ecs_new_id (world);
		ecs_entity_t p2 = ecs_new_id (world);
		ecs_entity_t r = ecs_new_id (world);

		//ecs_entity_t r1 = ecs_new_id (world);
		//ecs_entity_t r2 = ecs_new_id (world);
		ECS_TAG(world, r1);
		ECS_TAG(world, r2);

		/*
		ecs_set(world, p1, OpenGL_Program, {glCreateProgram()});
		ecs_set(world, p2, OpenGL_Program, {glCreateProgram()});
		ecs_set(world, p1, EcsName, {.value = "Program 1"});
		ecs_set(world, p2, EcsName, {.value = "Program 2"});
		ecs_set(world, s1, EcsName, {.value = "Shader 1"});
		ecs_set(world, s2, EcsName, {.value = "Shader 2"});
		ecs_set (world, s1, Filename, {CSC_SRCDIR"shader_text.glvs"});
		ecs_set (world, s2, Filename, {CSC_SRCDIR"shader_text.glfs"});
		//printf ("s1: %i, s2: %i, p1: %i, p2: %i\n", ecs_get(world,s1,OpenGL_Shader)->id, ecs_get(world,s2,OpenGL_Shader)->id, ecs_get(world,p1,OpenGL_Program)->id, ecs_get(world,p2,OpenGL_Program)->id);
		ecs_add_pair(world, p1, EcsIsA, s1);
		ecs_add_pair(world, p1, EcsIsA, s2);
		*/
		/*
		ecs_add_pair(world, p2, EcsIsA, s1);
		ecs_add_pair(world, p2, EcsIsA, s2);
		*/
		/*
		ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
		.add = {ecs_pair(r1, ecs_id(OpenGL_Shader))} });
		*/


		ecs_set(world, p1, OpenGL_Program, {glCreateProgram()});
		ecs_set(world, p2, OpenGL_Program, {glCreateProgram()});
		ecs_set(world, s1, OpenGL_Shader, {glCreateShader(GL_VERTEX_SHADER)});
		ecs_set(world, s2, OpenGL_Shader, {glCreateShader(GL_FRAGMENT_SHADER)});
		ecs_set(world, p1, EcsName, {.value = "Program 1"});
		ecs_set(world, p2, EcsName, {.value = "Program 2"});
		ecs_set(world, s1, EcsName, {.value = "Shader 1"});
		ecs_set(world, s2, EcsName, {.value = "Shader 2"});
		ecs_add_pair(world, s1, EcsIsA, p1);
		ecs_add_pair(world, s2, EcsIsA, p1);
		ecs_add_pair(world, s1, EcsIsA, p2);
		ecs_add_pair(world, s2, EcsIsA, p2);

		//ecs_query_t *q = ecs_query_new(world, "OpenGL_Shader, ANY:OpenGL_Program");
		ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
		.filter.terms = {
		{ecs_id(OpenGL_Shader)},
		{ecs_pair(EcsIsA, EcsWildcard)}
		}
		});
		ecs_iter_t it = ecs_query_iter(q);
		while (ecs_query_next (&it))
		{
			OpenGL_Shader * s = ecs_term(&it, OpenGL_Shader, 1);
			ecs_id_t id = ecs_term_id(&it, 2);
			ecs_entity_t obj = ecs_pair_object(world, id);
			//OpenGL_Program * p = ecs_term (&it, OpenGL_Program, 2);
			//OpenGL_Shader * s = ecs_term (&it, OpenGL_Shader, 1);
			for (int i = 0; i < it.count; i ++)
			{
				//printf ("%s\n", ecs_get_name(it.world, it.entities[i]));
				printf ("%s %s %i\n", ecs_get_name(it.world, it.entities[i]), ecs_get_name(world, obj), ecs_get(world, obj, OpenGL_Program)->id);
				//printf ("%s %i\n", ecs_get_name(it.world, it.entities[i]), p[0]);

				//printf ("%s: %i, %i\n", ecs_get_name(it.world, it.entities[i]), p[0].id, s[i].id);
			}
		}

	}




	{
		ecs_entity_t ent_face;
		ent_face = ecs_new_id (world);
		ecs_add (world, ent_face, Freetype_Face);
		ecs_set (world, ent_face, Filename, {CSC_SRCDIR"consola.ttf"});
		ecs_entity_t e = ecs_new_id (world);
		ecs_add (world, e, Textbox);
		ecs_add_pair (world, e, EcsIsA, ent_face);
		ecs_query_t *q = ecs_query_new(world, "Textbox, SHARED:Freetype_Face");
		ecs_iter_t it = ecs_query_iter(q);
		ecs_query_next(&it);
		ASSERT (it.count == 1);
	}




}




















