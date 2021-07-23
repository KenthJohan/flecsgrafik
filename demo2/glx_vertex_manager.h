#pragma once

#include <GL/glew.h>

#include "csc/csc_gl.h"
#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"
#include "csc/csc_vf32.h"
#include "csc/csc_v2f32.h"
#include "csc/csc_v4f32.h"
#include "csc/csc_vu32.h"
#include "csc/csc_primf32.h"



struct glx_vertex
{
	v4f32 xyzw;
	v4f32 uvl;
};





struct vcontainer
{
	uint32_t capacity; //Number of vertices
	uint32_t last; //Number of vertices
	struct glx_vertex * v;
};


static void vcontainer_init (struct vcontainer * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ctx->last = 0;
	uint32_t size = ctx->capacity*sizeof(struct glx_vertex);
	ctx->v = malloc (size);
	ASSERT_NOTNULL (ctx->v);
}


static void vcontainer_drawtext
(struct vcontainer * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float z, float sx, float sy, float l, char const * text)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_PARAM_NOTNULL (c);
	ASSERT_PARAM_NOTNULL (atlas);
	ASSERT_PARAM_NOTNULL (text);
	ASSERT_NOTNULL (ctx->v);
	struct glx_vertex * v = ctx->v + ctx->last;
	uint32_t stride = sizeof(struct glx_vertex) / sizeof(float);
	uint32_t n;
	n = gft_gen_pos (v->xyzw.e, ctx->capacity, stride, text, c, x, y, sx, sy);
	n = gft_gen_uv  (v->uvl.e, ctx->capacity, stride, text, c, atlas->w, atlas->h);
	vf32_set1_strided (v->xyzw.e + 2, z, n, stride); //Set z
	vf32_set1_strided (v->uvl.e + 2, l, n, stride); //Set vertex texture layer
	ctx->last += n;
}


static void vcontainer_drawtextf
(struct vcontainer * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float z, float sx, float sy, float l, const char* format, ...)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_PARAM_NOTNULL (format);
	ASSERT_PARAM_NOTNULL (atlas);
	char buf[1024];
	memset (buf, 0, 1024);
	va_list va;
	va_start (va, format);
	vsnprintf (buf, 1024, format, va);
	//printf ("%s", buf);
	vcontainer_drawtext (ctx, c, atlas, x, y, z, sx, sy, l, buf);
	va_end (va);
}


static void vcontainer_drawrect (struct vcontainer * ctx, float x, float y, float w, float h, float l)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	struct glx_vertex * v = ctx->v + ctx->last;
	uint32_t stride = sizeof(struct glx_vertex) / sizeof(float);
	primf32_make_rectangle4 (v->xyzw.e, stride, x, y, w, h, 0.0f, 0.0f);
	primf32_make_rectangle4 (v->uvl.e, stride, 0.0f, 0.0f, 1.0f, 1.0f, l, 0.0f);

	//v2f32_vertices6_set_rectangle (v->xyzw.e, stride, x, y, w, h); // Set vertex position
	//v2f32_vertices6_set_rectangle (v->uvl.e, stride, 0, 0, 1, 1); // Set vertex texture uv coordinate
	//vf32_set1_strided (v->uvl.e + 2, l, 6, stride); //Set vertex texture layer
	ctx->last += 6;
}


static void vcontainer_drawrect_border (struct vcontainer * ctx, float x, float y, float w, float h, float l)
{
	ASSERT_PARAM_NOTNULL (ctx);
	float t = 0.1f;
	vcontainer_drawrect (ctx, x, y, w, h, l);//Draw rectangle
	vcontainer_drawrect (ctx,   x, y-t, w, t, l);//Draw bottom border
	vcontainer_drawrect (ctx, x-t,   y, t, h, l);//Draw left border
	vcontainer_drawrect (ctx, w+x,   y, t, h, l);//Draw right border
	vcontainer_drawrect (ctx,   x, y+h, w, t, l);//Draw top border
}










enum glx_vattr
{
	GLX_VATTR0_POS_XY = 0,
	GLX_VATTR1_TEX_UVL = 1
};



struct glx_vertex_manager
{
	GLuint vao;
	GLuint vbo[1];
	uint32_t capacity; //Number of vertices
	uint32_t last; //Number of vertices
};


static void glx_vertex_manager_setup (struct glx_vertex_manager * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	glGenBuffers (1, ctx->vbo);
	glGenVertexArrays (1, &ctx->vao);
	glBindVertexArray (ctx->vao);
	glEnableVertexAttribArray (GLX_VATTR0_POS_XY);
	glEnableVertexAttribArray (GLX_VATTR1_TEX_UVL);
	{
		glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo[0]);
		glBufferData (GL_ARRAY_BUFFER, ctx->capacity*sizeof(struct glx_vertex), NULL, GL_DYNAMIC_DRAW);
		GLsizei const stride = sizeof(struct glx_vertex);
		GLintptr const offset0 = offsetof(struct glx_vertex, xyzw);
		GLintptr const offset1 = offsetof(struct glx_vertex, uvl);
		glVertexAttribPointer (GLX_VATTR0_POS_XY, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset0);
		glVertexAttribPointer (GLX_VATTR1_TEX_UVL, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset1);
	}
	ASSERT_GL;
}




static void glx_vertex_manager_flush (struct glx_vertex_manager * ctx, struct vcontainer * container)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (container->v);
	ASSERT (glIsVertexArray(ctx->vao));
	ASSERT (glIsBuffer(ctx->vbo[0]));
	glBindVertexArray (ctx->vao);
	//glUseProgram (ctx->program);
	//glBindTexture (GL_TEXTURE_2D, ctx->tex);
	//glUniform1i (ctx->uniform_tex, 0);


	//Number of triangles:
	GLsizei const count = container->last;
	for (int i = 0; i < count; ++i)
	{
		//printf ("Vertex%i: %2.5f %2.5f %2.5f %2.5f L%.1f\n", i, container->v[i].xyzw.x, container->v[i].xyzw.y, container->v[i].uvl.x, container->v[i].uvl.y, container->v[i].uvl.z);
	}
	//printf("\n");
	{
		glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo[0]);
		// Triangles vertices size in bytes:
		GLsizeiptr const size = sizeof(struct glx_vertex)*count;
		glBufferSubData (GL_ARRAY_BUFFER, 0, size, container->v);
		ASSERT_GL;
		//TODO: Why do we need this?:
		//memset (container->v, 0, size);
	}

	glDrawArrays (GL_TRIANGLES, 0, count);
	ASSERT_GL;
	container->last = 0;
}







