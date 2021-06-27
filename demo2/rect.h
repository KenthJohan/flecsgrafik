#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <GL/glew.h>
#include "csc/csc_debug.h"
#include "csc/csc_gft.h"
#include "plot.h"




struct rect_context
{
	GLuint vao;
	GLuint vbo;
	GLint program;
	float * v; //Triangles (xyuv,xyuv,xyuv) ...
	uint32_t last; //Number of triangles
	uint32_t count; //Number of triangles
	GLint uniform_tex;
	GLuint tex;
};



static void rect_setup (struct rect_context * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT (glIsProgram(ctx->program));
	ctx->last = 0;
	ctx->v = malloc (ctx->count * 3 * sizeof(float));
	ASSERT_NOTNULL (ctx->v);
	glUseProgram (ctx->program);

	{
		ctx->uniform_tex = glGetUniformLocation (ctx->program, "tex0");
		ASSERT (ctx->uniform_tex >= 0);
		unsigned unit = 0;
		glActiveTexture (GL_TEXTURE0 + unit);
		glGenTextures (1, &ctx->tex);
		glBindTexture (GL_TEXTURE_2D, ctx->tex);
		glUniform1i (ctx->uniform_tex, unit);

		int w = 256;
		int h = 256;
		/*
		GLint xoffset = 0;
		GLint yoffset = 0;
		uint8_t buffer[50*50] = {0x11, 0x22, 0x33, 0x44};
		plot_draw_rect (buffer, w, h, 3, 2, 2, 10);
		plot_i32 (buffer, w, h, (int32_t[]){-3,3,7,2,2,-3,7,7,0,2}, 10);
		*/

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
		/* We require 1 byte alignment when uploading texture data */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		/* Clamping to edges is important to prevent artifacts when scaling */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/* Linear filtering usually looks best for text */
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//glTexSubImage2D (GL_TEXTURE_2D, 0, xoffset, yoffset, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, buffer);
	}

	{
		glGenBuffers (1, &ctx->vbo);
		glGenVertexArrays (1, &ctx->vao);
		glBindVertexArray (ctx->vao);
		glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo);
		glEnableVertexAttribArray (0);
		glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}
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
	gft_trianglemesh2 (&uv, stride, 0, 0, 1, 1);
	ctx->last += 2;
}



static void rect_glflush (struct rect_context * ctx)
{
	uint32_t const stride = 4;
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	glBindVertexArray (ctx->vao);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
	GLsizeiptr size = sizeof(float) * stride * ctx->last * 3; // Triangles vertices size in bytes
	GLsizei count = ctx->last * 3; //Number of triangles
	glBufferData (GL_ARRAY_BUFFER, size, ctx->v, GL_DYNAMIC_DRAW);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, 0);
	glDrawArrays (GL_TRIANGLES, 0, count);
	ctx->last = 0;
}

