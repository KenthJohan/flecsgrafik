#pragma once

#include <GL/glew.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"
#include "csc/csc_vf32.h"
#include "csc/csc_v2f32.h"
#include "csc/csc_vu32.h"

enum glx_vattr
{
	GLX_VATTR0_POS_XY = 0,
	GLX_VATTR1_TEX_UVL = 1
};


#define GLX_VERTEX_MANAGER_STRIDE 5
struct glx_vertex_manager
{
	GLuint vao;
	GLuint vbo[1];
	uint32_t capacity; //Capacity quad
	uint32_t last; // Last quad
	float * v;
};


static void glx_vertex_manager_setup (struct glx_vertex_manager * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ctx->last = 0;

	uint32_t v_size = ctx->capacity*GLX_VERTEX_MANAGER_STRIDE*6*sizeof(float);

	ctx->v = malloc (v_size);
	ASSERT_NOTNULL (ctx->v);

	glGenBuffers (1, ctx->vbo);
	glGenVertexArrays (1, &ctx->vao);
	glBindVertexArray (ctx->vao);


	glEnableVertexAttribArray (GLX_VATTR0_POS_XY);
	glEnableVertexAttribArray (GLX_VATTR1_TEX_UVL);

	{
		glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo[0]);
		glBufferData (GL_ARRAY_BUFFER, v_size, ctx->v, GL_DYNAMIC_DRAW);
		// (xy,uvl), (xy,uvl), (xy,uvl), etc...
		GLsizei stride = 2 * sizeof (float) + 3 * sizeof (float);
		GLintptr offset0 = 0; //Position xy
		GLintptr offset1 = 2 * sizeof (float); //Texture coordinate uvl
		glVertexAttribPointer (GLX_VATTR0_POS_XY, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset0);
		glVertexAttribPointer (GLX_VATTR1_TEX_UVL, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset1);
	}

}



static void glx_vertex_manager_drawtext
(struct glx_vertex_manager * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float sx, float sy, float l, char const * text)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	float * v = ctx->v;
	v += (ctx->last * GLX_VERTEX_MANAGER_STRIDE * 6);
	uint32_t n;
	n = gft_gen_pos (v+0, ctx->capacity, GLX_VERTEX_MANAGER_STRIDE, text, c, x, y, sx, sy);
	n = gft_gen_uv  (v+2, ctx->capacity, GLX_VERTEX_MANAGER_STRIDE, text, c, atlas->w, atlas->h);
	vf32_set1_strided (v+4, l, 6, GLX_VERTEX_MANAGER_STRIDE); //Set vertex texture layer
	ctx->last += n;
}


static void glx_vertex_manager_drawtextf
(struct glx_vertex_manager * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float sx, float sy, float l, const char* format, ...)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_PARAM_NOTNULL (format);
	char buf[1024];
	memset (buf, 0, 1024);
	va_list va;
	va_start (va, format);
	vsnprintf (buf, 1024, format, va);
	//printf ("%s", buf);
	glx_vertex_manager_drawtext (ctx, c, atlas, x, y, sx, sy, l, buf);
	va_end (va);
}


static void glx_vertex_manager_drawrect (struct glx_vertex_manager * ctx, float x, float y, float w, float h, float l)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	float * v = ctx->v;
	v += (ctx->last * GLX_VERTEX_MANAGER_STRIDE * 6);
	v2f32_vertices6_set_rectangle (v+0, GLX_VERTEX_MANAGER_STRIDE, x, y, w, h); // Set vertex position
	v2f32_vertices6_set_rectangle (v+2, GLX_VERTEX_MANAGER_STRIDE, 0, 0, 1, 1); // Set vertex texture uv coordinate
	vf32_set1_strided (v+4, l, 6, GLX_VERTEX_MANAGER_STRIDE); //Set vertex texture layer
	ctx->last += 1;
}


static void glx_vertex_manager_drawrect_border (struct glx_vertex_manager * ctx, float x, float y, float w, float h, float l)
{
	float t = 0.1f;
	glx_vertex_manager_drawrect (ctx, x, y, w, h, l);//Draw rectangle
	glx_vertex_manager_drawrect (ctx,   x, y-t, w, t, l);//Draw bottom border
	glx_vertex_manager_drawrect (ctx, x-t,   y, t, h, l);//Draw left border
	glx_vertex_manager_drawrect (ctx, w+x,   y, t, h, l);//Draw right border
	glx_vertex_manager_drawrect (ctx,   x, y+h, w, t, l);//Draw top border
}





static void glx_vertex_manager_flush (struct glx_vertex_manager * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	ASSERT (glIsVertexArray(ctx->vao));
	ASSERT (glIsBuffer(ctx->vbo[0]));
	glBindVertexArray (ctx->vao);
	//glUseProgram (ctx->program);
	//glBindTexture (GL_TEXTURE_2D, ctx->tex);
	//glUniform1i (ctx->uniform_tex, 0);


	GLsizei count = ctx->last * 6; //Number of triangles

	{
		glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo[0]);
		GLsizeiptr size = sizeof(float) * GLX_VERTEX_MANAGER_STRIDE * count; // Triangles vertices size in bytes
		glBufferSubData (GL_ARRAY_BUFFER, 0, size, ctx->v);
		memset (ctx->v, 0, size);//TODO: Why do we need this?
	}

	glDrawArrays (GL_TRIANGLES, 0, count);
	ctx->last = 0;
}
