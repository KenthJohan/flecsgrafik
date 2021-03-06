#include "csc/csc_crossos.h"
#include "csc/csc_basic.h"
#include "csc/csc_assert.h"
#include "csc/csc_malloc_file.h"
#include "csc/csc_sdl_motion.h"
#include "csc/csc_gcam.h"
#include "csc/csc_gl.h"
#include "csc/csc_math.h"
#include "csc/csc_sdlglew.h"
#include "csc/csc_xlog.h"
//#include "csc/csc_gft.h"
#include "csc/csc_pixmap.h"
#include "csc/experiment/gtext1.h"


#include "gui_profiler.h"
#include "glx_vertex_manager.h"
#include "glx_texture_manager.h"
//#include "text.h"
#include "textedit_sample.h"
#include "../flecs/flecs.h"
#include "gui.h"
#include "types.h"


#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <cglm/cam.h>

#include <ft2build.h>
#include FT_FREETYPE_H


#define WIN_X SDL_WINDOWPOS_UNDEFINED
#define WIN_Y SDL_WINDOWPOS_UNDEFINED
#define WIN_W 640
#define WIN_H 480
#define WIN_TITLE "Demo2"
#define TEXLAYER_FONT 0
#define TEXLAYER_FPSPLOT 1
#define TEXLAYER_RANDOM 2




struct glx_texlist main_texlist = {};
struct glx_vao main_vao = {.capacity = 1000000};
struct vgraphics main_vgraphics3d = {.capacity = 1000000};
struct vgraphics main_vgraphics2d = {.capacity = 1000000};
struct gtext1_context main_textcontext = {0};
GLint uniform_mvp;




static void sys_cascade_transform (ecs_iter_t *it)
{
	quad_4f32        *p = ecs_term (it, quad_4f32, 1);//Parent
	quad_4f32        *c = ecs_term (it, quad_4f32, 2);//Child
	world_quad_4f32  *w = ecs_term (it, world_quad_4f32, 3);//Child

	if (p)
	{
		for (int i = 0; i < it->count; i ++)
		{

			/*
			float y0 = (-wp[i].y+win->h-rec[i].h);
			float x = (wp[i].x / win->w) * 2.0f - 1.0f;
			float y = (y0 / win->h) * 2.0f - 1.0f;
			float z = 0.0f;
			*/


			w[i].a.x = p[0].a.x + c[i].a.x;
			w[i].a.y = p[0].a.y + c[i].a.y;
			w[i].b.x = p[0].b.x + c[i].b.x;
			w[i].b.y = p[0].b.y + c[i].b.y;
			char const * name = ecs_get_name(it->world, it->entities[i]);
			printf("C%i: %s transformed\n", i, name);
		}
	}
	else
	{
		for (int i = 0; i < it->count; i ++)
		{
			w[i].a.x = c[i].a.x;
			w[i].a.y = c[i].a.y;
			w[i].b.x = c[i].b.x;
			w[i].b.y = c[i].b.y;
			char const * name = ecs_get_name(it->world, it->entities[i]);
			printf("R%i: %s transformed\n", i, name);
		}
	}
}


static void sys_quad (ecs_iter_t *it)
{
	quad_4f32      *q = ecs_term (it, quad_4f32,      1); // Output
	position_4f32  *p = ecs_term (it, position_4f32,  2); // Input
	rectangle_2f32 *r = ecs_term (it, rectangle_2f32, 3); // Input
	float kx = 0.5f;
	float ky = 0.5f;
	for (int i = 0; i < it->count; i++)
	{
		//q[i].a = (position_2f32){p[i].x - (0.5f+cx)*r[i].w, p[i].y + (0.5f+cy)*r[i].h};
		//q[i].b = (position_2f32){p[i].x + (0.5f-cx)*r[i].w, p[i].y + (0.5f-cy)*r[i].h};

		q[i].a = (position_2f32){p[i].x - (0.0f+kx)*r[i].w, p[i].y - (0.0f+ky)*r[i].h};
		q[i].b = (position_2f32){p[i].x + (1.0f-kx)*r[i].w, p[i].y + (1.0f-ky)*r[i].h};
	}
}



static void sys_draw_rectangle (ecs_iter_t *it)
{
	world_quad_4f32         *q = ecs_term (it, world_quad_4f32, 1);
	uvwh_4f32              *uv = ecs_term (it, uvwh_4f32, 2);
	texture_layer          *tl = ecs_term (it, texture_layer, 3);
	//opengl_vbo            *vbo = ecs_term (it, opengl_vbo, 5);//Shared
	//glBindBuffer (GL_ARRAY_BUFFER, vbo[0].vbo);
	//void * ptr = glMapBuffer (GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	struct glx_vertex * vertices = main_vgraphics2d.v + main_vgraphics2d.last;
	uint32_t stride = sizeof(struct glx_vertex) / sizeof(float);

	for (int i = 0; i < it->count; i++)
	{
		//printf("{.w = %f, .h = %f} {.w = %f, .h = %f}\n",win->w, win->h, r[i].w, r[i].h);
		float ax = q[i].a.x;
		float ay = q[i].a.y;
		float bx = q[i].b.x;
		float by = q[i].b.y;
		float z = 0.0f;
		float l = tl[i].layer;
		float u = uv[i].u;
		float v = uv[i].v;
		float du = uv[i].w;
		float dv = uv[i].h;
		/*
		ax = 200.0f;
		ay = 200.0f;
		bx = 210.0f;
		by = 300.0f;
		*/
		primf32_make6_rectangle4_ab (vertices->xyzw.e, stride, ax, ay, bx, by, z, 0.0f);
		primf32_make_rectangle4_xywh (vertices->uvl.e, stride, u, v, du, dv, l, 0.0f);
		vertices += 6;


		//primf32_make_rectangle4_xywh (main_vgraphics2d.v->xyzw.e, stride, x, y, w, h, 0.0f, 0.0f);
		//primf32_make_rectangle4_xywh (vertices->uvl.e, stride, u, v, du, dv, l, 0.0f);
	}
	main_vgraphics2d.last += it->count*6;
	//glUnmapBuffer(GL_ARRAY_BUFFER);
}


static void sys_draw_text (ecs_iter_t *it)
{
	position_4f32          *wp = ecs_term (it, position_4f32, 1);
	text_cstring         *text = ecs_term (it, text_cstring, 2);
	textsize_2f32         *rec = ecs_term (it, textsize_2f32, 3);
	texture_layer    *texlayer = ecs_term (it, texture_layer, 4);
	for (int i = 0; i < it->count; i++)
	{
		float x = wp[i].x;
		float y = wp[i].y;
		float z = 0.0f;
		float w = rec[i].w / main_textcontext.pixel_width;
		float h = rec[i].h / main_textcontext.pixel_height;
		float l = texlayer[i].layer;
		float line_distance = rec[i].h * 1.0f;
		vgraphics_drawtext (&main_vgraphics2d, main_textcontext.c, &main_textcontext.atlas, x, y, z, w, h, l, line_distance, text[i].text);
	}
}









static ecs_entity_t gui_spawn_box
(ecs_world_t *world, char const * name, float x, float y, float w, float h)
{
	ecs_entity_t e = ecs_set_name(world, 0, name);
	ecs_set(world, e, quad_4f32, {{0, 0}, {0, 0}});
	ecs_set(world, e, world_quad_4f32, {{0, 0}, {0, 0}});
	ecs_set(world, e, position_4f32, {x, y, 0, 0});
	ecs_set(world, e, rectangle_2f32, {w, h});
	ecs_set(world, e, uvwh_4f32, {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, 0.0f, 0.0f});
	ecs_set(world, e, texture_layer, {TEXLAYER_RANDOM});
	ecs_add(world, e, draw_rectangle);//Tag
	return e;
}


static ecs_entity_t gui_spawn_text
(ecs_world_t *world, char const * name, float x, float y, float fw, float fh, char const * text)
{
	ecs_entity_t e = ecs_set_name(world, 0, name);
	ecs_set(world, e, position_4f32, {0, 0, 0, 0});
	ecs_set(world, e, local_position_4f32, {x, y, 0, 0});
	ecs_set(world, e, textsize_2f32, {fw, fh});
	ecs_set(world, e, text_cstring, {text});
	ecs_set(world, e, texture_layer, {TEXLAYER_FONT});
	ecs_add(world, e, draw_text);//Tag
	return e;
}









int main (int argc, char * argv[])
{
	setbuf (stdout, NULL);
	//printf ("%jd\n", (intmax_t)1);
	csc_crossos_enable_ansi_color ();
	ASSERT (argc);
	ASSERT (argv);

	ecs_world_t *world = ecs_init_w_args(argc, argv);
	types_ecsinit (world);

	uint32_t main_flags = CSC_SDLGLEW_RUNNING;

	SDL_Window * window;
	SDL_GLContext context;
	csc_sdlglew_create_window (&window, &context, WIN_TITLE, WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

	glEnable (GL_VERTEX_PROGRAM_POINT_SIZE);
	glLineWidth (4.0f);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	/*
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	*/

	struct csc_gcam cam;
	csc_gcam_init (&cam);
	cam.p.z = -3.0f;
	const uint8_t * keyboard = SDL_GetKeyboardState (NULL);

	FT_Library ft;
	if (FT_Init_FreeType (&ft))
	{
		XLOG(XLOG_ERR, XLOG_FT, "Could not init FreeType Library");
		exit(1);
	}



	GLint program = csc_gl_program_from_files1 (CSC_SRCDIR"shader.glfs;"CSC_SRCDIR"shader.glvs");
	GLint uniform_tex = glGetUniformLocation (program, "tex0");
	uniform_mvp = glGetUniformLocation (program, "mvp");
	ASSERT (uniform_tex >= 0);
	ASSERT (uniform_mvp >= 0);
	glUseProgram (program);
	glUniform1i (uniform_tex, 0);



	main_texlist.texarray[0].w = 512;
	main_texlist.texarray[0].h = 512;
	main_texlist.texarray[0].l = 3;
	glx_texlist_setup (&main_texlist);
	glx_texlist_gen_gradient (&main_texlist, 0, 2);



	glx_vao_init (&main_vao);
	vgraphics_init (&main_vgraphics3d);
	vgraphics_init (&main_vgraphics2d);




	main_textcontext.filename = CSC_SRCDIR"consola.ttf";
	main_textcontext.ft = ft;
	main_textcontext.maxwidth = 512;
	main_textcontext.pixel_width = 48;
	main_textcontext.pixel_height = 48;
	glBindTexture (GL_TEXTURE_2D_ARRAY, main_texlist.tex[0]);
	gtext1_setup (&main_textcontext);


	struct gui_profiler gprofiler = {0};




	//ECS_SYSTEM (world, sys_transform1, EcsOnUpdate, position_4f32, local_position_4f32);

	ECS_SYSTEM (world, sys_quad, EcsOnUpdate, quad_4f32, position_4f32, rectangle_2f32);
	ECS_SYSTEM (world, sys_cascade_transform, EcsOnUpdate, quad_4f32(parent|cascade), quad_4f32, world_quad_4f32);
	ECS_SYSTEM (world, sys_draw_rectangle, EcsOnUpdate, world_quad_4f32, uvwh_4f32, texture_layer, draw_rectangle);
	ECS_SYSTEM (world, sys_draw_text, EcsOnUpdate, position_4f32, text_cstring, textsize_2f32, texture_layer, draw_text);




	//ECS_SYSTEM (world, sys_cascade_calc, EcsOnUpdate, PARENT:length_f32, length_f32);


	/*
	ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
	.filter.expr = "length_f32",
	.events = {EcsOnSet},
	.callback = sys_cascade_calc,
	//.ctx = &ctx
	});
	*/



	/*
	{
		ecs_entity_t e = ecs_set_name(world, 0, "fpsplot");
		ecs_add(world, e, window_rectangle_2f32);//Singleton
		ecs_add(world, e, draw_rectangle);//Tag
		ecs_set(world, e, position_4f32, {10, 400, 0, 0});
		ecs_set(world, e, rectangle_2f32, {WIN_W-20, 100});
		ecs_set(world, e, uvwh_4f32, {0.0f, 0.0f, 1.0f, 1.0f});
		ecs_set(world, e, texture_layer, {TEXLAYER_FPSPLOT});
	}
	*/







	ecs_entity_t e0 = gui_spawn_box (world, "box1", 0, 0, 20, 20);
	ecs_entity_t e1 = gui_spawn_box (world, "box2", 30, 30, 10, 10);
	//ecs_entity_t e2 = gui_spawn_box (world, "box3", 300, 300, 400, 40);
	ecs_entity_t e3 = gui_spawn_text (world, "text1", 0, 0, 50, 50, "Hellog\nWorgd!\nBagaGa");
	ecs_add_pair(world, e1, EcsChildOf, e0);
	//ecs_add_pair(world, e2, EcsChildOf, e0);
	//ecs_add_pair(world, e3, EcsChildOf, e2);


	//gui_update (world);




	while (main_flags & CSC_SDLGLEW_RUNNING)
	{
		//main_profile_start(0);
		gui_profiler_start (&gprofiler);

		{
			int w;
			int h;
			SDL_GetWindowSize (window, &w, &h);
			ecs_singleton_set(world, window_rectangle_2f32, {(float)w, (float)h});
		}

		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			csc_sdlglew_event_loop (window, &event, &main_flags, &cam);
		}

		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (keyboard [SDL_SCANCODE_0])
		{
			//ecs_set(world, e0, length_f32, {12});
		}

		if (keyboard [SDL_SCANCODE_1])
		{
			//ecs_set(world, e1, length_f32, {13});
		}

		if(1)
		{
			//Control graphics camera
			csc_sdl_motion_wasd (keyboard, &cam.d);
			csc_sdl_motion_pyr (keyboard, &cam.pyr_delta);

			int mdltx = 0, mdlty = 0;
			SDL_GetRelativeMouseState(&mdltx, &mdlty);

			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				cam.pyr_delta.x = (float)mdlty * -0.1f;
				cam.pyr_delta.y = (float)mdltx * 0.1f;
			}

			if (SDL_GetModState() & KMOD_CAPS)
			{
				v3f32_mul (&cam.d, &cam.d, 0.001f);
				v3f32_mul (&cam.pyr_delta, &cam.pyr_delta, 0.001f);
			}
			else
			{
				v3f32_mul (&cam.d, &cam.d, 0.01f);
				v3f32_mul (&cam.pyr_delta, &cam.pyr_delta, 0.01f);
			}


			csc_gcam_update (&cam);
		}

		/*
		{

			static uint64_t counter = 0;
			counter++;
			for (uint64_t j = 0; j < 100; ++j)
			{
				for (uint64_t i = 0; i < 100; ++i)
				{
					//vgraphics_drawtextf (&vg, tctx.c, &tctx.atlas, sin(i*0.1f + counter*0.01f) - j, cos(i*0.1f + counter*0.01f), i * -0.1f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "%jx", i*j*counter);
				}
			}
		}
		*/


		gui_profiler_draw (&gprofiler, &main_textcontext);
		ecs_progress (world, 0);



		{
			//Draw GUI
			int w;
			int h;
			SDL_GetWindowSize (window, &w, &h);
			//ecs_singleton_set(world, window_rectangle_2f32, {(float)w, (float)h});
			m4f32 m = (m4f32)M4F32_IDENTITY;
			m.m11 = 2.0f / (float)w;
			m.m22 = 2.0f / (float)h;
			m.m14 = -1.0f;
			m.m24 = -1.0f;
			/*
			m = cam.mvp;
			m4f32 s = (m4f32)M4F32_IDENTITY;
			m4f32_scale_xyz (&s, 1.0f/100.0f, 1.0f/100.0f, 1.0f/100.0f);
			m4f32_mul (&m, &m, &s);
			*/
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, m.m);
			glx_vao_flush (&main_vao, &main_vgraphics2d);
		}

		{
			//Draw vertices from camera perspective
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp.m);
			glBindTexture (GL_TEXTURE_2D_ARRAY, main_texlist.tex[0]);
			glx_vao_flush (&main_vao, &main_vgraphics3d);
		}



		//SDL_Delay (10);
		SDL_GL_SwapWindow (window);
		gui_profiler_end (&gprofiler);




	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit ();
	return 0;
}
