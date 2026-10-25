//*****************************************************************************
// @(#) $Id: gr.cpp,v 1.55 2003/10/01 00:06:22 virzak Exp $ 
// 
//  Description: 
//      This file is responsible for graphics
//
//  =
//
//*****************************************************************************

#ifdef _WIN32
#include "stdafx.h"
#endif


#include "gr.h"
#include "render.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

CGraphics * CGraphics::m_instance = NULL;


/* ambient 0.5 and diffuse 1.0 are built into the renderer's shader */
static GLfloat lightPosition[] = { 0.0f, 3.0f, 2.0f, 1.0f };

/* general OpenGL deinitialization function */
void CGraphics::deinitGL() {
}

/* general OpenGL initialization function */
void CGraphics::initGL()
{
  rnd::enable (GL_CULL_FACE);
  glDisable (GL_DITHER); /*WTF?*/
  rnd::enable(rnd::TEXTURE_2D);     /* enable texture mapping */
  rnd::blendFunc(GL_SRC_ALPHA, GL_ONE);  /* blending function */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  rnd::enable(GL_DEPTH_TEST);
  rnd::depthFunc(GL_LESS);
  /* set up our lighting */
  rnd::lightPosition(lightPosition);
  
  if (m_light)
    rnd::enable(rnd::LIGHTING);
  /* we use resizeGLScene once to set up our initial perspective */
  //resizeGLScene(window->gl_win.width, window->gl_win.height);
  glFlush();
}



/* function called when our window is resized (should only happen in window mode) */
void CGraphics::resizeGLScene(unsigned int width, unsigned int height)
{
  if (height == 0)  /* Prevent A Divide By Zero If The Window Is Too Small */
    height = 1;
  glViewport(0, 0, width, height);  /* Reset The Current Viewport And Perspective Transformation */
}






/* Here goes our drawing code */
void CGraphics::drawGLScene()
{
  COutput * output = COutput::instanceOf();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_root->draw();
  


  int err = glGetError();
  if (err != GL_NO_ERROR)
    output->printf ("Error %x\007\007\007\n", err);
  

}





//
// @(#) $Log: gr.cpp,v $
// @(#) Revision 1.55  2003/10/01 00:06:22  virzak
// @(#) Removed unnecessary includes
// @(#)
// @(#) Revision 1.54  2003/09/10 03:28:50  virzak
// @(#) When using output doing it thru COutput::instanceOf() instead of global
// @(#) variable, since test programs may not define any globals
// @(#)
// @(#) Revision 1.53  2003/09/09 22:41:59  virzak
// @(#) Added glEnable(GL_COLOR_MATERIAL);
// @(#)
// @(#) Revision 1.52  2003/09/04 01:09:40  virzak
// @(#) Removed outpututil.h from includes
// @(#)
// @(#) Revision 1.51  2003/08/29 08:13:17  virzak
// @(#) Moved all lighting from this file into light.h and light.cpp
// @(#)
// @(#) Revision 1.50  2003/08/28 09:14:25  virzak
// @(#) Moved all 2D functionality to CGUI2D.
// @(#) Removed all global Texture names. Loading textures using tex_free
// @(#) Removed all blending
// @(#)
// @(#) Revision 1.49  2003/08/27 22:09:10  virzak
// @(#) Moved CSector::onDrawi and CMirror::onDraw to world.cpp
// @(#) Moved background related functions to background.h and .cpp
// @(#)
// @(#) Revision 1.48  2003/08/26 08:39:56  virzak
// @(#) Fixed windows bug
// @(#)
// @(#) Revision 1.47  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.46  2003/08/26 03:30:43  virzak
// @(#) Moved gprintf to COutput
// @(#)
// @(#) Revision 1.45  2003/08/25 19:59:07  virzak
// @(#) Moved CPolygon::onDraw to polygon file
// @(#)
// @(#) Revision 1.44  2003/08/24 20:00:38  virzak
// @(#) Moved CTriangle::onDraw to CPolygon::onDraw
// @(#) Using CContainer to draw triangles
// @(#)
// @(#) Revision 1.43  2003/08/24 06:16:38  virzak
// @(#) Moved stain drawing into stain.cpp
// @(#) Changed CTriangle::onDraw();
// @(#)
// @(#) Revision 1.42  2003/08/20 02:36:54  virzak
// @(#) Changed drawing of the children of CMirror as a result of changes
// @(#) in node.h
// @(#)
// @(#) Revision 1.41  2003/08/16 05:51:03  virzak
// @(#) Fixed stains drawing to make glDepthMask(GL_FALSE);
// @(#)
// @(#) Revision 1.40  2003/08/14 03:02:29  virzak
// @(#) Added texture to stains. Need to fix flickering
// @(#)
// @(#) Revision 1.39  2003/08/14 01:14:18  virzak
// @(#) added background movement
// @(#)
// @(#) Revision 1.38  2003/08/09 01:05:01  virzak
// @(#) Added stencil mirror.
// @(#)
// @(#) Revision 1.37  2003/08/05 18:18:25  virzak
// @(#) Replaced quad by s_quad, since it is a reserved vaeiable or a keyword
// @(#) on some compilers
// @(#)
// @(#) Revision 1.36  2003/08/05 08:01:30  virzak
// @(#) Reused some code from CTriangle::onDraw in CSector::onDraw
// @(#)
// @(#) Revision 1.35  2003/08/04 08:05:34  virzak
// @(#) Changed all texture handling to use CTexture class
// @(#)
// @(#) Revision 1.34  2003/08/03 21:06:32  virzak
// @(#) Added background to the scene.
// @(#)
// @(#) Revision 1.33  2003/08/02 20:16:45  virzak
// @(#) Moved COutput util to outpututil.h
// @(#) Moved stains from CTriangle to CSector
// @(#) Moved gluLookAt and related to CCamera class
// @(#)
// @(#) Revision 1.32  2003/07/30 08:19:46  virzak
// @(#) Moved drawing code chunks to the onDraw methods of their
// @(#) respective classes
// @(#)
// @(#) Revision 1.31  2003/07/30 04:28:53  virzak
// @(#) more node changes
// @(#)
// @(#) Revision 1.30  2003/07/27 02:39:44  virzak
// @(#) Added output interface for vector3d
// @(#)
// @(#) Revision 1.29  2003/07/20 00:43:13  virzak
// @(#) Added tagra textures
// @(#)
// @(#) Revision 1.28  2003/07/12 22:26:31  virzak
// @(#) Small fixes
// @(#)
// @(#) Revision 1.27  2003/07/12 01:09:58  virzak
// @(#) put texture into into GrPoint2D
// @(#)
// @(#) Revision 1.26  2003/07/11 23:20:59  virzak
// @(#) Added double * conversion to GrTuple and used it
// @(#)
// @(#) Revision 1.25  2003/07/11 04:03:23  virzak
// @(#) Got rid of getComponent where is made mode sence to put x(), y(), or z()
// @(#)
// @(#) Revision 1.24  2003/07/10 19:23:08  virzak
// @(#) changed triangle array into deque
// @(#)
// @(#) Revision 1.23  2003/07/10 17:40:32  virzak
// @(#) Changed stains to be drawn with LEQUAL depth function to make sure
// @(#) it is always seen
// @(#)
// @(#) Revision 1.22  2003/07/09 05:27:06  virzak
// @(#) Fixed two windows warnings
// @(#)
// @(#) Revision 1.21  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.20  2003/07/04 19:31:10  virzak
// @(#) Moved stains from world to triangle in order to be able to use
// @(#) GL_ALWAYS as depth function and not have these stains going to
// @(#) other polygons. GL_EQUAL doesn't work well.
// @(#)
// @(#) Revision 1.19  2003/07/04 18:57:02  virzak
// @(#) Make the stain more realistic
// @(#)
// @(#) Revision 1.18  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.17  2003/07/04 06:17:52  virzak
// @(#) Fixed flying mode and got rid of useless variables and routines
// @(#) that were used in the simple kinetics mode
// @(#)
// @(#) Revision 1.16  2003/07/03 22:22:45  virzak
// @(#) Added capabilities for acceleration, velocity.
// @(#) Tied the positioning to them in order to have more realistic movement
// @(#)
// @(#) Revision 1.15  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
//




