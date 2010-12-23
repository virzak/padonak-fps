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
#include "GL/glu.h"





void CLight::onUpdate(unsigned int /*msecs*/) {
  m_pos = cp->m_eye_pos;

  
#ifdef __ICC
#pragma warning (disable:810)
#endif

  GLfloat lightPosition[4];
  lightPosition[0] = (float) m_pos.x();
  lightPosition[1] = (float) m_pos.y();
  lightPosition[2] = (float) m_pos.z();
  
#ifdef __ICC
#pragma warning (default:810)
#endif

  
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

}








void CLight::onDraw() const {

  // Is this necessary???
  if (glIsEnabled(GL_LIGHTING)) {
    GLUquadricObj * quad = gluNewQuadric (); 
    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    
      glTranslated(m_pos.x(), m_pos.y(), m_pos.z());
      glColor4f(1, 1, 1, 0.5);
      glDisable(GL_LIGHTING);
      glEnable (GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_TEXTURE_2D);
      
      gluSphere( quad, 0.2, 20, 20 );
    glPopAttrib();
    glPopMatrix();
    gluDeleteQuadric(quad);
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



