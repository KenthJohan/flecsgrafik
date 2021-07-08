#pragma once

#include <GL/glew.h>

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_gft.h"






struct glx_texarray
{
	uint32_t w;
	uint32_t h;
	uint32_t l;
};





static void glx_texarray (struct glx_texarray * item)
{
	GLsizei mipLevelCount = 1;
	//glTexImage2D (GL_TEXTURE_2D_ARRAY, 0, GL_ALPHA, item->w, item->h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	glTexStorage3D (GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_R8, item->w, item->h, item->l);
	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/* Linear filtering usually looks best for text */
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


static void glx_texarray_gen_gradient (struct glx_texarray * item, float l)
{
	uint8_t * a = malloc(item->w * item->h);
	for (uint32_t i = 0; i < item->w * item->h; ++i) {a[i] = i;}
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 0, 0, 0, l, item->w, item->h, 1, GL_RED, GL_UNSIGNED_BYTE, a);
	free (a);
}



#define GLX_TEXTLIST_CAPACITY 4
struct glx_texlist
{
	struct glx_texarray texarray[GLX_TEXTLIST_CAPACITY];
	GLuint tex[GLX_TEXTLIST_CAPACITY];
};

static void glx_texlist_setup (struct glx_texlist * item)
{
	ASSERT_PARAM_NOTNULL (item);
	unsigned unit = 0;
	glActiveTexture (GL_TEXTURE0 + unit);
	glGenTextures (GLX_TEXTLIST_CAPACITY, item->tex);
	//glUniform1i (ctx->uniform_tex, unit);

	glBindTexture (GL_TEXTURE_2D_ARRAY, item->tex[0]);
	glx_texarray (item->texarray + 0);

	//glBindTexture (GL_TEXTURE_2D_ARRAY, item->tex[1]);
	//glx_texarray (item->texarray + 1);

}


static void glx_texlist_gen_gradient (struct glx_texlist * item, int index, float l)
{
	glBindTexture (GL_TEXTURE_2D_ARRAY, item->tex[index]);
	glx_texarray_gen_gradient (item->texarray + index, l);
}










