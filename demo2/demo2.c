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
#include "csc/csc_gft.h"
#include "csc/csc_pixmap.h"
#include "csc/experiment/gtext1.h"


#include "gui_profiler.h"
#include "glx_vertex_manager.h"
#include "glx_texture_manager.h"
#include "text.h"


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






int main (int argc, char * argv[])
{
	csc_crossos_enable_ansi_color ();
	ASSERT (argc);
	ASSERT (argv);
	srand (1);
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
	GLint uniform_mvp = glGetUniformLocation (program, "mvp");
	ASSERT (uniform_tex >= 0);
	ASSERT (uniform_mvp >= 0);
	glUseProgram (program);
	glUniform1i (uniform_tex, 0);

	struct glx_texlist texlist = {};
	texlist.texarray[0].w = 512;
	texlist.texarray[0].h = 512;
	texlist.texarray[0].l = 3;
	glx_texlist_setup (&texlist);
	glx_texlist_gen_gradient (&texlist, 0, 2);

	struct glx_vao vm = {.capacity = 1000000};
	struct vgraphics vg = {.capacity = 1000000};
	glx_vao_init (&vm);
	vgraphics_init (&vg);



	struct gtext1_context tctx;
	tctx.filename = CSC_SRCDIR"consola.ttf";
	tctx.ft = ft;
	glBindTexture (GL_TEXTURE_2D_ARRAY, texlist.tex[0]);
	gtext1_setup (&tctx, 48, 48);




	struct gui_profiler gprofiler;


	while (main_flags & CSC_SDLGLEW_RUNNING)
	{
		//main_profile_start(0);
		gui_profiler_start (&gprofiler);

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
					vgraphics_drawtextf (&vg, tctx.c, &tctx.atlas, sin(i*0.1f + counter*0.01f) - j, cos(i*0.1f + counter*0.01f), i * -0.1f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "%jx", i*j*counter);
				}
			}


			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 0.1f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "%BCDEF");
			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 0.2f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "ABCDEF");
			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 0.3f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "ABCDEF");
			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 0.4f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "ABCDEF");
			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 0.5f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0.0f, "ABCDEF");
			vgraphics_drawtext (&vg, tctx.c, &tctx.atlas, -1.0f, 1.6f, 0.0f, 0.5f/48.0f, 0.5f/48.0f, 0.0f, "ABCDEF");

		}






		{
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, cam.mvp.m);
			glBindTexture (GL_TEXTURE_2D_ARRAY, texlist.tex[0]);
			glx_vao_flush (&vm, &vg);
		}

		{
			gui_profiler_draw (&gprofiler, &tctx);
			vgraphics_drawtextf (&vg, tctx.c, &tctx.atlas, -1.0f, -0.9f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, 0, "FPS: %3.3f, %3.3f", (float)SDL_GetPerformanceFrequency() / gprofiler.a[0], (gprofiler.a[0] * 1000.0) / SDL_GetPerformanceFrequency());
			vgraphics_drawrect_border (&vg, 0.0f, -2.0f, 1.0f, 1.0f, 2.0f);
			vgraphics_drawrect (&vg, -1.0f, -1.0f, 1.0f, 0.2f, 1.0f);


			struct gui_context gctx = {0};
			gctx.dim[0] = (v2u32){{400, 400}};
			gctx.padding[0] = (v2u32){{10, 10}};
			gctx.flags[0] = GUI_UP;
			gctx.rect_last = 1;
			gui_push (&gctx, 0, 0, 0, 100, 100, GUI_RIGHT);
			gui_push (&gctx, 0, 0, 0, 100, 100, GUI_RIGHT);
			gui_push (&gctx, 0, 0, 0, 100, 100, GUI_RIGHT);
			gui_push (&gctx, 0, 0, 0, 100, 100, GUI_RIGHT);

			gui_push (&gctx, 3, 0, 0, 100, 100, GUI_RIGHT);
			gui_push (&gctx, 3, 0, 0, 100, 100, GUI_RIGHT);
			int w;
			int h;
			SDL_GetWindowSize (window, &w, &h);
			gui_flush (&gctx, &vg, w, h);


			m4f32 m;
			m = (m4f32)M4F32_IDENTITY;
			glUniformMatrix4fv (uniform_mvp, 1, GL_FALSE, m.m);
			glx_vao_flush (&vm, &vg);
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
