#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "csc/csc_basic.h"
#include "csc/csc_debug.h"
#include "csc/csc_gl.h"
#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"
#include "csc/csc_pixmap.h"

#include "glx_texture_manager.h"
#include "glx_vertex_manager.h"
#include "text.h"



#define GUI_PROFILER_HCOUNT 512
struct gui_profiler
{
	uint32_t i;
	uint8_t h[GUI_PROFILER_HCOUNT];
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

static void gui_profiler_draw (struct gui_profiler * item, struct glx_vertex_manager * vm, struct gtext1_context * tctx)
{
	item->i &= (GUI_PROFILER_HCOUNT-1);
	double f = (double)SDL_GetPerformanceFrequency();
	int w = 512;
	int h = 512;
	uint32_t image[512*512] = {};
	csc_pixmap_plot_u8 (image, w, h, item->h, GUI_PROFILER_HCOUNT, 0xFFFFFFFF);
	csc_pixmap_drawrect (image, w, h, item->i, 0, 10, h, 0xFF0000FF);
	GLint xoffset = 0;
	GLint yoffset = 0;
	GLint zoffset = 1;
	GLint level = 0;
	GLsizei depth = 1;
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, zoffset, w, h, depth, GL_RGBA, GL_UNSIGNED_BYTE, image);
	ASSERT_GL;
	item->h[item->i] = (item->a[0] * 1000) / f;
	item->i++;
	glx_vertex_manager_drawtextf (vm, tctx->c, &tctx->atlas, -1.0f, -1.0f, 0.1f/48.0f, 0.1f/48.0f, 0, "FPS: %3.5f", f / item->a[0]);
	glx_vertex_manager_drawtextf (vm, tctx->c, &tctx->atlas, -1.0f, -0.9f, 0.1f/48.0f, 0.1f/48.0f, 0, " ms: %3.5f", (item->a[0] * 1000.0) / f);
}
