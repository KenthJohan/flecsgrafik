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
	uint32_t maxchars;
	uint32_t char_last;
	float * v;
	char * strbuf;
};





static void text_setup (struct text_context * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ctx->char_last = 0;
	ctx->v = malloc (ctx->maxchars*4*6);
	ctx->strbuf = malloc (ctx->maxchars);
	ASSERT_NOTNULL (ctx->v);
	ASSERT_NOTNULL (ctx->strbuf);
	ASSERT (glIsProgram(ctx->program));
	if (FT_New_Face(ctx->ft, ctx->filename, 0, &ctx->face))
	{
		XLOG (XLOG_ERR, XLOG_FT, "Failed to load font %s", ctx->filename);
		exit (1);
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



static void text_draw (struct text_context * ctx, float x, float y, float sx, float sy, char const * text)
{
	uint32_t const stride = 4;
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT (glIsProgram(ctx->program));
	ASSERT (glIsTexture(ctx->tex));
	ASSERT (glIsVertexArray(ctx->vao));
	ASSERT (glIsBuffer(ctx->vbo));
	//ASSERT (strnlen (text, ctx->maxchars) <= ctx->maxchars);
	//memset (ctx->v, 0, sizeof (v));
	float * v = ctx->v;
	v += (ctx->char_last * stride * 6);
	uint32_t n;
	n = gtext_gen1 (v+0, ctx->maxchars, stride, text, ctx->c, x, y, sx, sy);
	n = gtext_gen2 (v+2, ctx->maxchars, stride, text, ctx->c, ctx->atlas.w, ctx->atlas.h);
	ctx->char_last += n;

}


static void text_draw_format (struct text_context * ctx, float x, float y, float sx, float sy, const char* format, ...)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_PARAM_NOTNULL (format);
	ASSERT_NOTNULL (ctx->strbuf);
	va_list arglist;
	va_start (arglist, format);
	vsnprintf (ctx->strbuf, ctx->maxchars, format, arglist);
	//printf ("%s", buf);
	text_draw (ctx, x, y, sx, sy, ctx->strbuf);
	va_end (arglist);
}


static void text_glflush(struct text_context * ctx)
{
	uint32_t const stride = 4;
	ASSERT_PARAM_NOTNULL (ctx);
	glBindVertexArray (ctx->vao);
	glUseProgram (ctx->program);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, 0);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
	GLsizeiptr size = sizeof(float) * stride * 6 * ctx->char_last; // Triangles vertices size in bytes
	GLsizei count = ctx->char_last * 6; //Number of triangles
	glBufferData (GL_ARRAY_BUFFER, size, ctx->v, GL_DYNAMIC_DRAW);
	glDrawArrays (GL_TRIANGLES, 0, count);
	ctx->char_last = 0;
}



