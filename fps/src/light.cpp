//*****************************************************************************
// @(#) $Id: light.cpp,v 1.5 2003/10/18 08:00:10 virzak Exp $ 
// 
//  Description: 
//      Contains lighting code
//
//  =
//
//*****************************************************************************


#include "light.h"
#include "engine.h"
#include "render.h"





void CLight::onUpdate(unsigned int /*msecs*/) {
  m_pos = cp->m_eye_pos;

  
#ifdef __ICC
#pragma warning (disable:810)
#endif

  GLfloat lightPosition[4];
  lightPosition[0] = (float) m_pos.x();
  lightPosition[1] = (float) m_pos.y();
  lightPosition[2] = (float) m_pos.z();
  /* This was left uninitialised. It came out 0 in the fixed-function build,
   * making a directional light along the eye position; keep that look. */
  lightPosition[3] = 0.0f;
  
#ifdef __ICC
#pragma warning (default:810)
#endif

  
  rnd::lightPosition(lightPosition);

}








void CLight::onDraw() const {

  // Is this necessary???
  if (rnd::isEnabled(rnd::LIGHTING)) {
    rnd::pushMatrix();
    rnd::pushAttrib(rnd::ENABLE_BIT | rnd::LIGHTING_BIT | rnd::CURRENT_BIT);
    
      rnd::translate(m_pos.x(), m_pos.y(), m_pos.z());
      rnd::color(1, 1, 1, 0.5);
      rnd::disable(rnd::LIGHTING);
      rnd::enable (GL_BLEND);
      rnd::blendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      rnd::disable(rnd::TEXTURE_2D);
      
      rnd::sphere( 0.2, 20, 20 );
    rnd::popAttrib();
    rnd::popMatrix();
  }

}


//
// @(#) $Log: light.cpp,v $
// @(#) Revision 1.5  2003/10/18 08:00:10  virzak
// @(#) Fixed onUpdate routine
// @(#)
// @(#) Revision 1.4  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.3  2003/09/06 03:50:47  virzak
// @(#) Now the light shines from the eye point
// @(#) Drawing the sphere properly
// @(#)
// @(#) Revision 1.2  2003/08/29 09:53:06  virzak
// @(#) Removed some redundant code
// @(#)
// @(#) Revision 1.1  2003/08/29 08:36:07  virzak
// @(#) Fisrt draft
// @(#)
//



