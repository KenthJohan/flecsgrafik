#pragma once

#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_primf32.h"
#include "csc/csc_gl.h"
#include "csc/csc_xlog.h"
#include "csc/csc_math.h"
#include "csc/csc_vf32.h"
#include "csc/csc_v2f32.h"
#include "csc/csc_v4f32.h"
#include "csc/csc_vu32.h"
#include "csc/csc_primf32.h"

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





Dependencies:

Inputs:
* Window width & height (SDL)
* Mouse & key inputs (SDL)

Configs:
* Texturelayer
* GUI elements

* Shader (OpenGL)

Outputs:
* Vertices: xyz, uv, texturelayer




*/


































