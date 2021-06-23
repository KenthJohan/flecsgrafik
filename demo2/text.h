#pragma once

#include <GL/glew.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"


struct text_context
{
	FT_Library ft;
	FT_Face face;
	char const * filename;
	struct gchar c[128];
	struct gatlas atlas;
	GLuint vao;
	GLuint vbo;
	GLint program;
	GLint uniform_tex;
	GLuint tex;
};





static void text_setup (struct text_context * ctx)
{
	ASSERT (glIsProgram(ctx->program));
	if (FT_New_Face(ctx->ft, ctx->filename, 0, &ctx->face))
	{
		XLOG(XLOG_ERR, XLOG_FT, "Failed to load font %s", ctx->filename);
		exit(1);
	}
	glUseProgram (ctx->program);
	ctx->uniform_tex = glGetUniformLocation (ctx->program, "tex0");
	ASSERT (ctx->uniform_tex >= 0);
	unsigned unit = 0;
	glActiveTexture (GL_TEXTURE0 + unit);
	glGenTextures (1, &ctx->tex);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, unit);
	FT_Set_Pixel_Sizes (ctx->face, 0, 48);
	gtext_init (ctx->face, ctx->c, &ctx->atlas);
	glGenBuffers (1, &ctx->vbo);
	glGenVertexArrays (1, &ctx->vao);
	glBindVertexArray (ctx->vao);
	glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, 0);
}



static void text_draw(struct text_context * ctx, char const * text, float x, float y, float sx, float sy)
{
	ASSERT (glIsProgram(ctx->program));
	ASSERT (glIsTexture(ctx->tex));
	ASSERT (glIsVertexArray(ctx->vao));
	ASSERT (glIsBuffer(ctx->vbo));
	ASSERT (strnlen(text, 100) < 100);
	struct
	{
		float x;
		float y;
		float u;
		float v;
	} v[6*20];
	memset (v, 0, sizeof (v));
	uint32_t i = 0;
	i = gtext_gen1 (&v->x, 6*20, 4, text, ctx->c, x, y, sx, sy);
	i = gtext_gen2 (&v->u, 6*20, 4, text, ctx->c, ctx->atlas.w, ctx->atlas.h);

	glBindVertexArray (ctx->vao);
	glUseProgram (ctx->program);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, 0);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(float)*4*i, v, GL_DYNAMIC_DRAW);
	glDrawArrays (GL_TRIANGLES, 0, i);
}
