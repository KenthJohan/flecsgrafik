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





struct vgraphics
{
	uint32_t capacity; //Number of vertices
	uint32_t last; //Number of vertices
	struct glx_vertex * v;
};


static void vgraphics_init (struct vgraphics * ctx)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ctx->last = 0;
	uint32_t size = ctx->capacity*sizeof(struct glx_vertex);
	ctx->v = malloc (size);
	ASSERT_NOTNULL (ctx->v);
}


static void vgraphics_drawtext
(struct vgraphics * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float z, float sx, float sy, float l, char const * text)
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


static void vgraphics_drawtextf
(struct vgraphics * ctx, struct gft_char c[128], struct gft_atlas * atlas, float x, float y, float z, float sx, float sy, float l, const char* format, ...)
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
	vgraphics_drawtext (ctx, c, atlas, x, y, z, sx, sy, l, buf);
	va_end (va);
}


static void vgraphics_drawrect (struct vgraphics * ctx, float x, float y, float w, float h, float l)
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


static void vgraphics_drawrect1 (struct vgraphics * ctx, float x, float y, float w, float h, float l, float tu, float tv)
{
	ASSERT_PARAM_NOTNULL (ctx);
	ASSERT_NOTNULL (ctx->v);
	struct glx_vertex * v = ctx->v + ctx->last;
	uint32_t stride = sizeof(struct glx_vertex) / sizeof(float);
	primf32_make_rectangle4 (v->xyzw.e, stride, x, y, w, h, 0.0f, 0.0f);
	v[0].uvl.x = tu;
	v[1].uvl.x = tu;
	v[2].uvl.x = tu;
	v[3].uvl.x = tu;
	v[4].uvl.x = tu;
	v[5].uvl.x = tu;

	v[0].uvl.y = tv;
	v[1].uvl.y = tv;
	v[2].uvl.y = tv;
	v[3].uvl.y = tv;
	v[4].uvl.y = tv;
	v[5].uvl.y = tv;

	v[0].uvl.z = l;
	v[1].uvl.z = l;
	v[2].uvl.z = l;
	v[3].uvl.z = l;
	v[4].uvl.z = l;
	v[5].uvl.z = l;

	//v2f32_vertices6_set_rectangle (v->xyzw.e, stride, x, y, w, h); // Set vertex position
	//v2f32_vertices6_set_rectangle (v->uvl.e, stride, 0, 0, 1, 1); // Set vertex texture uv coordinate
	//vf32_set1_strided (v->uvl.e + 2, l, 6, stride); //Set vertex texture layer
	ctx->last += 6;
}



static void vgraphics_drawrect_border (struct vgraphics * ctx, float x, float y, float w, float h, float l)
{
	ASSERT_PARAM_NOTNULL (ctx);
	float t = 0.1f;
	vgraphics_drawrect (ctx, x, y, w, h, l);//Draw rectangle
	vgraphics_drawrect (ctx,   x, y-t, w, t, l);//Draw bottom border
	vgraphics_drawrect (ctx, x-t,   y, t, h, l);//Draw left border
	vgraphics_drawrect (ctx, w+x,   y, t, h, l);//Draw right border
	vgraphics_drawrect (ctx,   x, y+h, w, t, l);//Draw top border
}










enum glx_vattr
{
	GLX_VATTR0_POS_XY = 0,
	GLX_VATTR1_TEX_UVL = 1
};



struct glx_vao
{
	GLuint vao;
	GLuint vbo[1];
	uint32_t capacity; //Number of vertices
	uint32_t last; //Number of vertices
};


static void glx_vao_init (struct glx_vao * ctx)
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




static void glx_vao_flush (struct glx_vao * ctx, struct vgraphics * container)
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
























/*
window (x1,y1,w1,h1)
-window (x2,y2,w2,h2)
--window (x3,y3,w3,h3)
window (x4,y4,w4,h4)
-window (x5,y5,w5,h5)

//Relative position in % and px
//Three rectangles:
*-----------------*
|		1		  |
*-----------------*
|		4		  |
|				  |
*-----------------*

Pair:
(0,1)
(0,4)
(1,2)
(1,3)
(1,5)
(0,7)

Adjacency matrix:

*/

struct gui_rectangle
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};

struct gui_padding
{
	uint16_t l;
	uint16_t r;
	uint16_t t;
	uint16_t b;
};

#define GUI_PAD(x) (struct gui_padding){(x),(x),(x),(x)}
#define GUI_DEFAULT  UINT32_C(0x00000001)
#define GUI_LEFT     UINT32_C(0x00000010)
#define GUI_RIGHT    UINT32_C(0x00000020)
#define GUI_UP       UINT32_C(0x00000040)
#define GUI_DOWN     UINT32_C(0x00000080)
#define GUI_LAST     UINT32_C(0x00000100)
#define GUI_FIRST    UINT32_C(0x00000200)

#define GUI_MAX_RECTS 100
struct gui_context
{
	//Generated:
	struct gui_rectangle rectangle[GUI_MAX_RECTS];
	struct gui_rectangle drawarea[GUI_MAX_RECTS];
	uint16_t last;

	//Source:
	struct gui_padding padding[GUI_MAX_RECTS];
	uint16_t size[GUI_MAX_RECTS];
	uint32_t flags[GUI_MAX_RECTS];
	uint16_t parent[GUI_MAX_RECTS];
};



static void gui_push (struct gui_context * ctx, uint16_t parent, uint16_t size, struct gui_padding * padding, uint32_t flags)
{
	//TODO: Find a free index
	uint32_t index = ctx->last;
	XLOG (XLOG_INF, XLOG_GENERAL, "Adding rectangle %i to %i with size: %i\n", index, parent, size);
	ctx->size[index] = size;
	ctx->flags[index] = flags;
	ctx->padding[index] = padding ? *padding : (struct gui_padding){0};
	ctx->last++;

	if (ctx->flags[parent] & GUI_UP)
	{
		size = MIN(size, ctx->drawarea[parent].h);
		ctx->rectangle[index].x = ctx->drawarea[parent].x;
		ctx->rectangle[index].y = ctx->drawarea[parent].y;
		ctx->rectangle[index].w = ctx->drawarea[parent].w;
		ctx->rectangle[index].h = size;
		ctx->drawarea[parent].y += size;
		ctx->drawarea[parent].h -= size;
	}
	else if (ctx->flags[parent] & GUI_DOWN)
	{
		size = MIN(size, ctx->drawarea[parent].h);
		ctx->rectangle[index].x = ctx->drawarea[parent].x;
		ctx->rectangle[index].y = ctx->drawarea[parent].y + ctx->drawarea[parent].h - size;
		ctx->rectangle[index].w = ctx->drawarea[parent].w;
		ctx->rectangle[index].h = size;
		ctx->drawarea[parent].h -= size;
	}
	else if (ctx->flags[parent] & GUI_RIGHT)
	{
		size = MIN(size, ctx->drawarea[parent].h);
		ctx->rectangle[index].x = ctx->drawarea[parent].x;
		ctx->rectangle[index].y = ctx->drawarea[parent].y;
		ctx->rectangle[index].w = size;
		ctx->rectangle[index].h = ctx->drawarea[parent].h;
		ctx->drawarea[parent].x += size;
		ctx->drawarea[parent].w -= size;
	}
	else if (ctx->flags[parent] & GUI_LEFT)
	{
		size = MIN(size, ctx->drawarea[parent].h);
		ctx->rectangle[index].x = ctx->drawarea[parent].x + ctx->drawarea[parent].w - size;
		ctx->rectangle[index].y = ctx->drawarea[parent].y;
		ctx->rectangle[index].w = size;
		ctx->rectangle[index].h = ctx->drawarea[parent].h;
		ctx->drawarea[parent].w -= size;
	}


	{
		uint16_t x = ctx->rectangle[index].x + ctx->padding[index].b;
		uint16_t y = ctx->rectangle[index].y + ctx->padding[index].l;
		uint16_t w = ctx->rectangle[index].w - (ctx->padding[index].l + ctx->padding[index].r);
		uint16_t h = ctx->rectangle[index].h - (ctx->padding[index].b + ctx->padding[index].t);
		ctx->drawarea[index].x = x;
		ctx->drawarea[index].y = y;
		ctx->drawarea[index].w = w;
		ctx->drawarea[index].h = h;
	}
}



static void gui_flush (struct gui_context * ctx, struct vgraphics * graphics, int sw, int sh)
{
	srand (1);
	for (uint32_t i = 0; i < ctx->last; ++i)
	{
		float x = ctx->rectangle[i].x / (float)sw;
		float y = ctx->rectangle[i].y / (float)sh;
		float w = ctx->rectangle[i].w / (float)sw;
		float h = ctx->rectangle[i].h / (float)sh;
		vgraphics_drawrect1 (graphics, x, y, w, h, 2, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
	}
	//ctx->last = 0;
}








