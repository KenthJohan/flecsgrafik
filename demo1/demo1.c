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

//#include "api.h"

#include <flecs.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <cglm/cam.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ecs.h"
#include "text.h"

#define WIN_X SDL_WINDOWPOS_UNDEFINED
#define WIN_Y SDL_WINDOWPOS_UNDEFINED
#define WIN_W 640
#define WIN_H 480
#define WIN_TITLE "Demo1"


static struct gtext_context gtext_ctx;



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

	ecs_world_t * world = ecs_init();
	main_ecs_register (world);

	struct csc_gcam cam;





	while (main_flags & CSC_SDLGLEW_RUNNING)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			csc_sdlglew_event_loop (window, &event, &main_flags, &cam);
		}
		ecs_progress (world, 0);
		SDL_Delay (10);
		SDL_GL_SwapWindow (window);
	}

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit ();
	ecs_fini (world);
	return 0;
}
