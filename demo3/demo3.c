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

/*
static void gui_draw (struct gui_context * ctx, struct vgraphics * graphics, int sw, int sh)
{
	srand (1);
	struct gui_position * pos = ctx->containers[0].pos;
	struct gui_rectangle * rec = ctx->containers[0].rec;
	uint32_t last = ctx->containers[0].last;
	for (uint32_t i = 0; i < last; ++i)
	{
		float x = (pos[i].x / (float)sw) - 1.0f;
		float y = (pos[i].y / (float)sh) - 1.0f;
		float w = rec[i].w / (float)sw;
		float h = rec[i].h / (float)sh;
		vgraphics_drawrect1 (graphics, x, y, w, h, 2, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
	}
	//ctx->last = 0;
}
*/

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} Position_4f32;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} World_Position_4f32;

typedef struct
{
	float w;
	float h;
} Rectangle_2f32;

typedef struct
{
	float w;
	float h;
} Window_Rectangle_2f32;

typedef struct
{
	float u;
	float v;
} uv_coordinate_2f32;


ECS_COMPONENT_DECLARE(Position_4f32);
ECS_COMPONENT_DECLARE(World_Position_4f32);
ECS_COMPONENT_DECLARE(Rectangle_2f32);
ECS_COMPONENT_DECLARE(Window_Rectangle_2f32);
ECS_COMPONENT_DECLARE(uv_coordinate_2f32);






struct glx_texlist main_texlist = {};
struct glx_vao main_vao = {.capacity = 1000000};
struct vgraphics main_vgraphics = {.capacity = 1000000};
struct gtext1_context main_textcontext = {0};
GLint uniform_mvp;





void Transform(ecs_iter_t *it)
{
	/* Get the two columns from the system signature */
	World_Position_4f32 *parent_wp = ecs_term(it, World_Position_4f32, 1);
	World_Position_4f32 *wp = ecs_term(it, World_Position_4f32, 2);
	Position_4f32 *p = ecs_term(it, Position_4f32, 3);

	if (!parent_wp)
	{
		for (int i = 0; i < it->count; i ++)
		{
			wp[i].x = p[i].x;
			wp[i].y = p[i].y;
			//printf("%s transformed to {.x = %f, .y = %f} <<root>>\n",ecs_get_name(it->world, it->entities[i]),wp[i].x, wp[i].y);
		}
	}
	else
	{
		for (int i = 0; i < it->count; i ++)
		{
			wp[i].x = parent_wp->x + p[i].x;
			wp[i].y = parent_wp->y + p[i].y;
			//printf("%s transformed to {.x = %f, .y = %f} <<child>>\n",ecs_get_name(it->world, it->entities[i]),wp[i].x, wp[i].y);
		}
	}
}



void Draw(ecs_iter_t *it)
{
	World_Position_4f32 *wp = ecs_term(it, World_Position_4f32, 1);
	Rectangle_2f32 *r = ecs_term(it, Rectangle_2f32, 2);
	uv_coordinate_2f32 *uv = ecs_term(it, uv_coordinate_2f32, 3);
	Window_Rectangle_2f32 *win = ecs_term(it, Window_Rectangle_2f32, 4);
	for (int i = 0; i < it->count; i ++)
	{
		//printf("{.w = %f, .h = %f} {.w = %f, .h = %f}\n",win->w, win->h, r[i].w, r[i].h);
		float x = (wp[i].x / win->w) - 1.0f;
		float y = (wp[i].y / win->h) - 1.0f;
		float w = r[i].w / win->w;
		float h = r[i].h / win->h;
		vgraphics_drawrect1 (&main_vgraphics, x, y, w, h, 2, uv[i].u, uv[i].v);
	}

	m4f32 m;
	m = (m4f32)M4F32_IDENTITY;
	glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, m.m);
	glx_vao_flush (&main_vao, &main_vgraphics);
}

















int main (int argc, char * argv[])
{
	printf ("%jd\n", (intmax_t)1);
	csc_crossos_enable_ansi_color ();
	ASSERT (argc);
	ASSERT (argv);

	ecs_world_t *world = ecs_init_w_args(argc, argv);
	ECS_COMPONENT_DEFINE(world, Position_4f32);
	ECS_COMPONENT_DEFINE(world, World_Position_4f32);
	ECS_COMPONENT_DEFINE(world, Rectangle_2f32);
	ECS_COMPONENT_DEFINE(world, Window_Rectangle_2f32);
	ECS_COMPONENT_DEFINE(world, uv_coordinate_2f32);

	srand (42);
	setbuf (stdout, NULL);

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
	vgraphics_init (&main_vgraphics);




	main_textcontext.filename = CSC_SRCDIR"consola.ttf";
	main_textcontext.ft = ft;
	main_textcontext.maxwidth = 512;
	main_textcontext.pixel_width = 48;
	main_textcontext.pixel_height = 48;
	glBindTexture (GL_TEXTURE_2D_ARRAY, main_texlist.tex[0]);
	gtext1_setup (&main_textcontext);


	struct gui_profiler gprofiler = {0};




	ECS_SYSTEM (world, Transform, EcsOnUpdate, CASCADE:World_Position_4f32, World_Position_4f32, Position_4f32);
	ECS_SYSTEM (world, Draw, EcsOnUpdate, World_Position_4f32, Rectangle_2f32, uv_coordinate_2f32, $Window_Rectangle_2f32);



	ecs_entity_t e1 = ecs_set_name(world, 0, "e1");
	ecs_entity_t e2 = ecs_set_name(world, 0, "e2");

	ecs_add(world, e1, World_Position_4f32);
	ecs_add(world, e1, Window_Rectangle_2f32);
	ecs_set(world, e1, Position_4f32, {10, 10, 0, 0});
	ecs_set(world, e1, Rectangle_2f32, {200, 100});
	ecs_set(world, e1, uv_coordinate_2f32, {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX});

	ecs_add(world, e2, World_Position_4f32);
	ecs_add(world, e2, Window_Rectangle_2f32);
	ecs_set(world, e2, Position_4f32, {10, 0, 0, 0});
	ecs_set(world, e2, Rectangle_2f32, {50, 50});
	ecs_set(world, e2, uv_coordinate_2f32, {(float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX});


	ecs_add_pair(world, e2, EcsChildOf, e1);



	while (main_flags & CSC_SDLGLEW_RUNNING)
	{
		//main_profile_start(0);
		gui_profiler_start (&gprofiler);

		{
			int w;
			int h;
			SDL_GetWindowSize (window, &w, &h);
			ecs_singleton_set(world, Window_Rectangle_2f32, {(float)w, (float)h});
		}

		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			csc_sdlglew_event_loop (window, &event, &main_flags, &cam);
		}

		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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






		if (0)
		{
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp.m);
			glBindTexture (GL_TEXTURE_2D_ARRAY, main_texlist.tex[0]);
			glx_vao_flush (&main_vao, &main_vgraphics);
		}


		if (1)
		{
			gui_profiler_draw (&gprofiler, &main_textcontext);
			//vgraphics_drawtextf (&vg, tctx.c, &tctx.atlas, -1.0f, -0.9f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0, "FPS: %3.3f, %3.3f", (float)SDL_GetPerformanceFrequency() / gprofiler.a[0], (gprofiler.a[0] * 1000.0) / SDL_GetPerformanceFrequency());
			vgraphics_drawrect_border (&main_vgraphics, 0.0f, -2.0f, 1.0f, 1.0f, 2.0f);
			vgraphics_drawrect (&main_vgraphics, -1.0f, -1.0f, 1.0f, 0.2f, 1.0f);
			vgraphics_drawtext (&main_vgraphics, main_textcontext.c, &main_textcontext.atlas, -1.0f, 0.0f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, -0.1f, "A B C\nD-E-F\n\n1_2_3\n!#&");
			int w;
			int h;
			SDL_GetWindowSize (window, &w, &h);
			//gui_draw (&mygui, &vg, w, h);
			m4f32 m;
			m = (m4f32)M4F32_IDENTITY;
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, m.m);
			glx_vao_flush (&main_vao, &main_vgraphics);
		}


		ecs_progress (world, 0);

		//SDL_Delay (10);
		SDL_GL_SwapWindow (window);
		gui_profiler_end (&gprofiler);




	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit ();
	return 0;
}
