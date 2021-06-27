#pragma once

#include <GL/glew.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"


static void plot_draw_rect (uint8_t * image, int32_t iw, int32_t ih, int32_t rx, int32_t ry, int32_t rw, int32_t rh)
{
	rx += MIN (0, rw);
	ry += MIN (0, rh);
	rw = abs (rw);
	rh = abs (rh);
	rw = MIN (rw + rx, iw);
	rh = MIN (rh + ry, ih);
	for (; rx < rw; ++rx)
	{
		for (; ry < rh; ++ry)
		{
			image[rx + ry*ih] = 0xFF;
		}
	}
}


static void plot_i32 (uint8_t * image, uint32_t w, uint32_t h, int32_t data[], uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		plot_draw_rect (image, w, h, i, h/2, 1, data[i]);
	}
}

static void plot_u8 (uint8_t * image, uint32_t w, uint32_t h, uint8_t data[], uint32_t n)
{
	for (uint32_t i = 0; i < n; ++i)
	{
		plot_draw_rect (image, w, h, i, h/2, 1, data[i]);
	}
}



/*
A B C D E F
G A B C D E
F G H A B C



*/



