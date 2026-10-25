//*****************************************************************************
//
//  Description:
//      The OpenGL API: desktop OpenGL 3.3 core through the glad loader, or
//      OpenGL ES 3.0 (WebGL 2) in the browser
//
//*****************************************************************************

#ifndef __GLAPI_H__
#define __GLAPI_H__

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#endif /*__GLAPI_H__*/
