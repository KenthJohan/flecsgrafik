#pragma once

#include <GL/glew.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"





struct gtext1_context
{
	//Inputs:
	FT_Library ft;
	char const * filename;
	FT_UInt pixel_width;
	FT_UInt pixel_height;
	unsigned int maxwidth;

	//Generated:
	FT_Face face;
	struct gft_char c[128];
	struct gft_atlas atlas;
};





static void gtext1_setup (struct gtext1_context * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);

	if (FT_New_Face(ctx->ft, ctx->filename, 0, &ctx->face))
	{
		XLOG (XLOG_ERR, XLOG_FT, "Failed to load font %s", ctx->filename);
		exit (1);
	}


	/*
	unsigned unit = 0;
	glActiveTexture (GL_TEXTURE0 + unit);
	glGenTextures (1, &ctx->tex);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, unit);
	*/

	gft_init (ctx->face, ctx->c, &ctx->atlas, ctx->maxwidth, ctx->pixel_width, ctx->pixel_height);

}


