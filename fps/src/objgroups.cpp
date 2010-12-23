//*****************************************************************************
// @(#) $Id: objgroups.cpp,v 1.2 2003/10/21 02:54:18 virzak Exp $ 
// 
//  Description: 
//      Initialisations for 3d or 2d drawing
//
//  =
//
//*****************************************************************************



#include "objgroups.h"

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <GL/gl.h>
#include "engine.h"

void C3DObjects::onDraw() const {
  // set up for projective drawing to draw 3D objects
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(gr->getFOV(), (GLdouble)window->gl_win.width / (GLdouble)window->gl_win.height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


static void drawCircle() {
  double chf = 0.05; /* crosshair factor */
  double radius = window->gl_win.width < window->gl_win.height ? (double)window->gl_win.width * chf : (double)window->gl_win.height * chf;
  glPushMatrix();
  glTranslated((double)window->gl_win.width * 0.5, (double)window->gl_win.height * 0.5, 0);
  glBegin(GL_LINE_LOOP);
  for (int i=0; i<40; ++i) {
    double cosine = radius * cos(i*2*M_PI/40);
    double sine = radius * sin(i*2*M_PI/40);
    glVertex2d(cosine, sine);
  }
  glEnd();
  glPopMatrix();
}



void CGUI2D::drawHelpBar() const {

  glColor4f(0, 0, 0, 0.5);
  glRecti(0, 20, window->gl_win.width, 0);  

  glColor3f(1, 0, 0);
  
  double speed = sqrt(cp->m_velocity.sqmag());
  *output <<
    COutput::Pos(window->gl_win.width - 80, 4) << "FPS=" << fps <<
    COutput::Pos(window->gl_win.width - 180, 4) << "RotY=" << cp->m_rotY <<
    COutput::Pos(0, 4) << /*"look_dir=" << cp->m_look_dir <<*/
      (cp->m_floor ? "floor" : "air") <<
    COutput::Pos(180, 4) << "vel=" << cp->m_velocity << "=" << 
      speed << "m/s=" << speed*3600/1000 << "km/h" << ends;
  
}




void CGUI2D::onDraw() const {
  // set up for orthogonal drawing to draw 2D objects on screen 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, window->gl_win.width, window->gl_win.height);
  glOrtho(0.0, (double)window->gl_win.width, 0.0, (double)window->gl_win.height, -0.1, 0.1);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  
  glColor4d (1, 1, 1, 1);

  glPushAttrib (GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_CULL_FACE);

    /* Cross hair */
    drawCircle();
    
    /* Help Bar */
    drawHelpBar();

    if (output_toggle) {
      glColor4f(0, 0, 0, 0.5);
      glRecti(0, window->gl_win.height, window->gl_win.width, window->gl_win.height/2);
    
      glColor3f(1, 0, 1);
      output->draw(10, window->gl_win.height - 20, 20);
    }


  glPopAttrib();


}





//
// @(#) $Log: objgroups.cpp,v $
// @(#) Revision 1.2  2003/10/21 02:54:18  virzak
// @(#) Changed  cp->m_look_dir inticator to cp->m_floor, which is more useful
// @(#)
// @(#) Revision 1.1  2003/09/17 07:35:38  virzak
// @(#) Moved classes C3DObjects and CGUI2D from world.cpp world.h to
// @(#) objgroups.cpp objgroups.h
// @(#)
//
