#include "csc/csc_crossos.h"
#include "csc/csc_basic.h"
#include "csc/csc_debug.h"
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




struct glx_texture_manager
{
	uint32_t w;
	uint32_t h;
	uint32_t l;
	GLuint tex[1];
};

static void glx_texture_manager_setup (struct glx_texture_manager * item)
{
	ASSERT_PARAM_NOTNULL (item);
	unsigned unit = 0;
	glActiveTexture (GL_TEXTURE0 + unit);
	glGenTextures (1, item->tex);
	glBindTexture (GL_TEXTURE_2D_ARRAY, item->tex[0]);
	//glUniform1i (ctx->uniform_tex, unit);

	GLsizei mipLevelCount = 1;
	//glTexImage2D (GL_TEXTURE_2D_ARRAY, 0, GL_ALPHA, item->w, item->h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	glTexStorage3D (GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_R8, item->w, item->h, item->l);
	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/* Linear filtering usually looks best for text */
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	uint8_t * a = malloc(item->w * item->h);
	for (uint32_t i = 0; i < item->w * item->h; ++i) {a[i] = i;}
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, item->w, item->h, 1,  GL_RED, GL_UNSIGNED_BYTE, a);
	free(a);
}
















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
	if (FT_Init_FreeType(&ft))
	{
		XLOG(XLOG_ERR, XLOG_FT, "Could not init FreeType Library");
		exit(1);
	}

	struct glx_texture_manager texlist = {.w = 512, .h = 512, .l = 2};
	glx_texture_manager_setup (&texlist);


	struct glx_vertex_manager vm = {.maxchars = 1000, .program = csc_gl_program_from_files1 (CSC_SRCDIR"shader.glfs;"CSC_SRCDIR"shader.glvs")};
	glx_vertex_manager_setup (&vm);

	struct gtext1_context tctx;
	tctx.filename = CSC_SRCDIR"consola.ttf";
	tctx.ft = ft;
	glBindTexture (GL_TEXTURE_2D_ARRAY, texlist.tex[0]);
	gtext1_setup (&tctx);

	GLint mvp = glGetUniformLocation (vm.program, "mvp");
	ASSERT (mvp >= 0);


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
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, "123456");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.1f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.2f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.3f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.4f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.5f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawtext (&vm, tctx.c, &tctx.atlas, -1.0f, 0.6f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			glx_vertex_manager_drawrect (&vm, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			gui_profiler_draw (&gprofiler, &vm, &tctx);
		}

		{
			m4f32 m;

			glUseProgram (vm.program);
			m = (m4f32)M4F32_IDENTITY;
			//glUniformMatrix4fv (mvp, 1, GL_FALSE, m.m);
			glUniformMatrix4fv (mvp, 1, GL_FALSE, cam.mvp.m);
			glBindTexture (GL_TEXTURE_2D_ARRAY, texlist.tex[0]);
			glx_vertex_manager_flush (&vm);
		}





		SDL_Delay (10);
		SDL_GL_SwapWindow (window);
		gui_profiler_end (&gprofiler);
	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit ();
	return 0;
}
