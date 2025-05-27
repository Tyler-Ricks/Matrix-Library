#ifndef GRAPHICS_TOOLS_H
#define GRAPHICS_TOOLS_H

#include "matrix.h"
#include <math.h>
#define PERSP_PROJ_DIMENSION 4
// I have a lot of macro plans for here due to the sheet variance across different graphics APIs and the 
// way they handle different matrices (ex perspective projection)
// openGL uses left hand rule and a different canonical view volume to Vulkan, which uses right hand rule.
// for now, implement for vulkan, but use macros for more flexibility

fmatrix ortho_proj_plane  (float l, float r, float b, float t, float n, float f, pool* frame);
fmatrix ortho_proj_plane_c(float r, float b, float n, float f, pool* frame);

fmatrix persp_proj_plane  (float l, float r, float b, float t, float n, float f, pool* frame);
fmatrix persp_proj_plane_c(float r, float b, float n, float f, pool* frame);
fmatrix persp_proj_fov(float w, float h, float n, float f, float theta, pool* frame);

#endif GRAPHICS_TOOLS_H