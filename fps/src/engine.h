//*****************************************************************************
// @(#) $Id: engine.h,v 1.34 2003/12/05 16:56:38 virzak Exp $
//
//  Description:
//      Provides an interface to the global variables and initialization
//      routines
//
//  =
//
//*****************************************************************************


#ifndef __ENGINE_H__
#define __ENGINE_H__

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "util.h"
#include "timer.h"
#include "output.h"
#include "gr.h"
#include "ui.h"
#include "player.h"
#include "world.h"
#include "textures.h"
#include "window.h"

extern const char *worldfile;   /* datafile for our 3D world */

extern bool mirror;
extern bool mirror_stencil;
extern bool draw_myself;


extern CGraphics *gr;
extern CUI *ui;
extern CPlayer *cp;
extern CWorld *wld;
extern CCamera *cam;
extern CWindow *window;


extern bool output_toggle;

extern CTextureContainer *tex_container;
extern CMaterialContainer *mat_container;


extern unsigned int fps;

extern GLuint filter;  /* which filter to use */

extern const float piover180;


extern COutput * output;

extern void draw();
extern void init_app();

#endif





// @(#)
// @(#) $Log: engine.h,v $
// @(#) Revision 1.34  2003/12/05 16:56:38  virzak
// @(#) added material container
// @(#)
// @(#) Revision 1.33  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.32  2003/09/04 01:16:04  virzak
// @(#) Removed extern bool destroyed_by_user;
// @(#)
// @(#) Revision 1.31  2003/08/29 08:12:20  virzak
// @(#) Removed many global variables and put them in apropriate classes
// @(#) Added texture container that contains all textures
// @(#)
// @(#) Revision 1.30  2003/08/28 09:17:18  virzak
// @(#) Added enum for required texture and an array of pointers.
// @(#) Removed many unnecessary global variables
// @(#)
// @(#) Revision 1.29  2003/08/27 22:10:14  virzak
// @(#) Moved GLWindow structure to window.h
// @(#)
// @(#) Revision 1.28  2003/08/26 05:34:14  virzak
// @(#) gui.h >> window.h
// @(#)
// @(#) Revision 1.27  2003/08/26 04:09:01  virzak
// @(#) Changes resulting from movement of windowing functions into CWindow class
// @(#)
// @(#) Revision 1.26  2003/08/24 06:15:27  virzak
// @(#) Removed some comments
// @(#)
// @(#) Revision 1.25  2003/08/09 01:05:01  virzak
// @(#) Added stencil mirror.
// @(#)
// @(#) Revision 1.24  2003/08/05 18:18:25  virzak
// @(#) Replaced quad by s_quad, since it is a reserved vaeiable or a keyword
// @(#) on some compilers
// @(#)
// @(#) Revision 1.23  2003/08/04 08:05:34  virzak
// @(#) Changed all texture handling to use CTexture class
// @(#)
// @(#) Revision 1.22  2003/08/03 21:04:35  virzak
// @(#) Added comment
// @(#)
// @(#) Revision 1.21  2003/08/02 20:13:04  virzak
// @(#) Added CCamera instance
// @(#)
// @(#) Revision 1.20  2003/07/30 08:14:06  virzak
// @(#) Provided interface for textures. This is a temporary solution
// @(#)
// @(#) Revision 1.19  2003/07/10 18:38:30  virzak
// @(#) fixed comments
// @(#)
// @(#) Revision 1.18  2003/07/03 22:16:47  virzak
// @(#) Moved speeds to player.h/cpp
// @(#)
// @(#) Revision 1.17  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
// @(#) Revision 1.16  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
// @(#) Revision 1.15  2003/07/02 19:27:51  virzak
// @(#) Removed a line from log that contained a comment, since the compiler was throwing errors about having nested comments
// @(#)
// @(#)








