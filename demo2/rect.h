#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <GL/glew.h>
#include "csc/csc_debug.h"
#include "csc/csc_gft.h"

struct rect_context
{
	GLuint vao;
	GLuint vbo;
	GLint program;
	float * v; //Triangles (xyuv,xyuv,xyuv) ...
	uint32_t last; //Number of triangles
	uint32_t count; //Number of triangles
};



static void rect_setup (struct rect_context * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT (glIsProgram(ctx->program));
	ctx->last = 0;
	ctx->v = malloc (ctx->count * 3 * sizeof(float));
	ASSERT_NOTNULL (ctx->v);
	glUseProgram (ctx->program);
	glGenBuffers (1, &ctx->vbo);
	glGenVertexArrays (1, &ctx->vao);
	glBindVertexArray (ctx->vao);
	glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, 0);
}



static void rect_draw_rect (struct rect_context * ctx, float x, float y, float w, float h)
{
	uint32_t const stride = 4;
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT (glIsProgram(ctx->program));
	ASSERT (glIsVertexArray(ctx->vao));
	ASSERT (glIsBuffer(ctx->vbo));
	ASSERT_NOTNULL (ctx->v);
	float * pos = ctx->v + 0;
	float * uv = ctx->v + 2;
	gft_trianglemesh2 (&pos, stride, x, y, w, h);
	//gft_trianglemesh2 (&uv, stride, x, y, w, h);
	ctx->last += 6;
}



static void rect_glflush (struct rect_context * ctx)
{
	uint32_t const stride = 4;
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	glBindVertexArray (ctx->vao);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
	GLsizeiptr size = sizeof(float) * stride * 3 * ctx->last; // Triangles vertices size in bytes
	GLsizei count = ctx->last * 3; //Number of triangles
	glBufferData (GL_ARRAY_BUFFER, size, ctx->v, GL_DYNAMIC_DRAW);
	glDrawArrays (GL_TRIANGLES, 0, count);
	ctx->last = 0;
}

