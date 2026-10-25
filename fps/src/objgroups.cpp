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

#include "render.h"
#include "engine.h"

void C3DObjects::onDraw() const {
  // set up for projective drawing to draw 3D objects
  rnd::matrixMode(rnd::PROJECTION);
  rnd::loadIdentity();
  rnd::perspective(gr->getFOV(), (double)window->gl_win.width / (double)window->gl_win.height, 0.1f, 100.0f);
  rnd::matrixMode(rnd::MODELVIEW);
  rnd::loadIdentity();
}


static void drawCircle() {
  double chf = 0.05; /* crosshair factor */
  double radius = window->gl_win.width < window->gl_win.height ? (double)window->gl_win.width * chf : (double)window->gl_win.height * chf;
  rnd::pushMatrix();
  rnd::translate((double)window->gl_win.width * 0.5, (double)window->gl_win.height * 0.5, 0);
  rnd::begin(rnd::LINE_LOOP);
  for (int i=0; i<40; ++i) {
    double cosine = radius * cos(i*2*M_PI/40);
    double sine = radius * sin(i*2*M_PI/40);
    rnd::vertex(cosine, sine);
  }
  rnd::end();
  rnd::popMatrix();
}



void CGUI2D::drawHelpBar() const {

  rnd::color(0, 0, 0, 0.5);
  rnd::rect(0, 20, window->gl_win.width, 0);  

  rnd::color(1, 0, 0);
  
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
  rnd::matrixMode(rnd::PROJECTION);
  rnd::loadIdentity();
  glViewport(0, 0, window->gl_win.width, window->gl_win.height);
  rnd::ortho(0.0, (double)window->gl_win.width, 0.0, (double)window->gl_win.height, -0.1, 0.1);

  // change to model view for drawing
  rnd::matrixMode(rnd::MODELVIEW);
  rnd::loadIdentity();
  
  
  rnd::color (1, 1, 1, 1);

  rnd::pushAttrib (rnd::ENABLE_BIT | rnd::CURRENT_BIT);
    rnd::disable(rnd::LIGHTING);
    rnd::disable(GL_DEPTH_TEST);

    rnd::enable (GL_BLEND);
    rnd::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    rnd::disable(rnd::TEXTURE_2D);
    rnd::disable (GL_CULL_FACE);

    /* Cross hair */
    drawCircle();
    
    /* Help Bar */
    drawHelpBar();

    if (output_toggle) {
      rnd::color(0, 0, 0, 0.5);
      rnd::rect(0, window->gl_win.height, window->gl_win.width, window->gl_win.height/2);
    
      rnd::color(1, 0, 1);
      output->draw(10, window->gl_win.height - 20, 20);
    }


  rnd::popAttrib();


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
