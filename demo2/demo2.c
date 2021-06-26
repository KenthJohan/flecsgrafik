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

//#include "api.h"


#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <cglm/cam.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "text.h"

#define WIN_X SDL_WINDOWPOS_UNDEFINED
#define WIN_Y SDL_WINDOWPOS_UNDEFINED
#define WIN_W 640
#define WIN_H 480
#define WIN_TITLE "Demo2"



Uint64 main_profile0[10];
Uint64 main_profile1[10];
void main_profile_start(int i)
{
	main_profile0[i] = SDL_GetPerformanceCounter();
}

double main_profile_stop_s(int i)
{
	Uint64 end = SDL_GetPerformanceCounter();
	Uint64 d = (end - main_profile0[i]);
	//Moving average:
	Uint64 denominator = 100;
	uint64_t numerator = 90;
	Uint64 d1 = numerator * main_profile1[i] / denominator;
	Uint64 d2 = (denominator - numerator) * d / denominator;
	main_profile1[i] = d1 + d2;
	return  main_profile1[i] / (double)SDL_GetPerformanceFrequency();
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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

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

	struct text_context tctx;
	tctx.maxchars = 1000;
	tctx.filename = CSC_SRCDIR"consola.ttf";
	tctx.program = csc_gl_program_from_files1 (CSC_SRCDIR"shader_text.glfs;"CSC_SRCDIR"shader_text.glvs");
	tctx.ft = ft;
	text_setup (&tctx);
	GLint mvp = glGetUniformLocation(tctx.program, "mvp");
	ASSERT (mvp >= 0);



	double s;
	while (main_flags & CSC_SDLGLEW_RUNNING)
	{
		main_profile_start(0);

		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			csc_sdlglew_event_loop (window, &event, &main_flags, &cam);
		}

		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if(0)
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
			m4f32 m = M4F32_IDENTITY;
			glUniformMatrix4fv (mvp, 1, GL_FALSE, m.m);
			//glUniformMatrix4fv (mvp, 1, GL_FALSE, cam.mvp.m);
			text_draw (&tctx, -1.0f, 0.0f, 0.1f/48.0f, 0.1f/48.0f, "123456");
			text_draw (&tctx, -1.0f, 0.1f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw (&tctx, -1.0f, 0.2f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw (&tctx, -1.0f, 0.3f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw (&tctx, -1.0f, 0.4f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw (&tctx, -1.0f, 0.5f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw (&tctx, -1.0f, 0.6f, 0.1f/48.0f, 0.1f/48.0f, "ABCDEF");
			text_draw_format (&tctx, -1.0f, -1.0f, 0.1f/48.0f, 0.1f/48.0f, "FPS: %f", 1.0/s);
			text_draw_format (&tctx, -1.0f, -0.9f, 0.1f/48.0f, 0.1f/48.0f, " ms: %f", s*1000.0);
		}


		text_glflush (&tctx);


		SDL_Delay (10);
		SDL_GL_SwapWindow (window);
		s = main_profile_stop_s(0);
	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit ();
	return 0;
}
