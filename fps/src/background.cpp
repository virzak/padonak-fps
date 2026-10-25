//*****************************************************************************
// @(#) $Id: background.cpp,v 1.6 2003/10/18 07:59:21 virzak Exp $ 
// 
//  Description: 
//      Handles Background drawing
//
//  =
//
//*****************************************************************************


#include "background.h"
#include "engine.h"
#include "render.h"

inline GrPoint2D getBgTexCoord(double rotY, double lookup) {
  return GrPoint2D(rotY/360.f+0.5f,lookup/360.f+0.5f); //top-left
  
}



void CBackground::onDraw() const {


  // set up for orthogonal drawing to draw Background 
  rnd::matrixMode(rnd::PROJECTION);
  rnd::loadIdentity();
  glViewport(0, 0, window->gl_win.width, window->gl_win.height);
  rnd::ortho(0.0, (double)window->gl_win.width, 0.0, (double)window->gl_win.height, -0.1, 0.1);

  // change to model view for drawing
  rnd::matrixMode(rnd::MODELVIEW);
  rnd::loadIdentity();
  
  rnd::enable(rnd::TEXTURE_2D);



  rnd::pushAttrib(rnd::ENABLE_BIT);
    rnd::disable(GL_CULL_FACE);
    rnd::disable(GL_DEPTH_TEST);
    rnd::disable(rnd::LIGHTING);
    
    tex_container->glBind("bg_texture");
    
    rnd::begin(rnd::QUADS);
      rnd::texCoord2v(m_tl);
      rnd::vertex(0,window->gl_win.height);
      rnd::texCoord2v(m_bl);
      rnd::vertex(0,0); 
      rnd::texCoord2v(m_br);
      rnd::vertex(window->gl_win.width,0); 
      rnd::texCoord2v(m_tr);
      rnd::vertex(window->gl_win.width,window->gl_win.height); 
    rnd::end();
  
  
  rnd::popAttrib();


}




void CBackground::onUpdate(unsigned int /*msecs*/) {
  double y_fov = gr->getFOV();
  double x_fov = y_fov*(double)window->gl_win.width / (double)window->gl_win.height;

  const double x_speed = 0.00f;
  const double y_speed = 0.01f;
  
  
  m_tl = getBgTexCoord(
      cp->m_rotY+(double)timer.getTime()*x_speed+x_fov/2.f, 
      cp->m_lookup+(double)timer.getTime()*y_speed+y_fov/2.f
  );
  
  m_br = getBgTexCoord(
      cp->m_rotY+(double)timer.getTime()*x_speed-x_fov/2.f, 
      cp->m_lookup+(double)timer.getTime()*y_speed-y_fov/2.f
  );
  
  m_tr = GrPoint2D(m_br.x(),m_tl.y());
  m_bl = GrPoint2D(m_tl.x(),m_br.y());

}






//
// @(#) $Log: background.cpp,v $
// @(#) Revision 1.6  2003/10/18 07:59:21  virzak
// @(#) Got rid of icc warning 869
// @(#)
// @(#) Revision 1.5  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.4  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.3  2003/08/29 08:09:02  virzak
// @(#) Divided the calculations between update and draw evenly
// @(#)
// @(#) Revision 1.2  2003/08/28 09:05:13  virzak
// @(#) Changed texture access
// @(#)
// @(#) Revision 1.1  2003/08/27 22:16:48  virzak
// @(#) First try
// @(#)
//


