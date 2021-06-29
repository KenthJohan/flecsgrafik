#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"
#include "csc/csc_pixmap.h"
#include "csc/experiment/gtext1.h"


#include "rect.h"






#define GUI_PROFILER_HCOUNT 256
struct gui_profiler
{
	uint8_t i;
	uint8_t h[256];
	uint64_t t0[10];
	uint64_t a[10];
};

static void gui_profiler_start (struct gui_profiler * item)
{
	item->t0[0] = SDL_GetPerformanceCounter();
}

static void gui_profiler_end (struct gui_profiler * item)
{
	uint64_t end = SDL_GetPerformanceCounter();
	uint64_t d = (end - item->t0[0]);
	//Moving average:
	uint64_t denominator = 100;
	uint64_t numerator = 70;
	uint64_t d1 = numerator * item->a[0] / denominator;
	uint64_t d2 = (denominator - numerator) * d / denominator;
	item->a[0] = d1 + d2;
	//return  main_profile1[i] / (double)SDL_GetPerformanceFrequency();
}

static void gui_profiler_draw (struct gui_profiler * item, struct rect_context * rctx, struct gtext1_context * tctx)
{
	double f = (double)SDL_GetPerformanceFrequency();
	glBindTexture (GL_TEXTURE_2D, rctx->tex);
	int w = 256;
	int h = 256;
	GLint xoffset = 0;
	GLint yoffset = 0;
	uint8_t image[256*256] = {0x11, 0x22, 0x33, 0x44};
	csc_pixmap_plot_u8 (image, w, h, item->h, GUI_PROFILER_HCOUNT);
	csc_pixmap_drawrect (image, w, h, item->i, 0, 1, h, 0x44);
	glTexSubImage2D (GL_TEXTURE_2D, 0, xoffset, yoffset, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, image);
	item->h[item->i] = (item->a[0] * 1000) / f;
	item->i++;
	gtext1_draw_format (tctx, -1.0f, -1.0f, 0.1f/48.0f, 0.1f/48.0f, "FPS: %3.5f", f / item->a[0]);
	gtext1_draw_format (tctx, -1.0f, -0.9f, 0.1f/48.0f, 0.1f/48.0f, " ms: %3.5f", (item->a[0] * 1000.0) / f);
}
