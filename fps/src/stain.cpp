//*****************************************************************************
// @(#) $Id: stain.cpp,v 1.23 2003/12/08 11:08:19 virzak Exp $ 
// 
//  Description: 
//      
//
//  =
//
//*****************************************************************************


#ifdef _WIN32
#include "stdafx.h"
#endif

#include "stain.h"
#include <stdlib.h>
#include "render.h"

// Always last
#include "globals.h"
#include "engine.h"




CStain::CStain(const GrPoint3D & pos, const GrVector3D & normal, 
    const GrPoint3D & base_point, GrVector3D up) {

  m_norm = normal;

  const double half_side = 0.05;
  
  // Determine and normalise the up vector
  up.planeProject(m_norm);
  up.normalize();

  // Find the last vector
  GrVector3D right = up*m_norm;

  // Find the points and texture coordinates
  GrPoint2D center(0.5, 0.5);
  double angle = 45+rand();
  double mag = 0.5 ;
  m_vertex[0].p = pos+half_side*(up+right);
  m_vertex[0].tex = center + mag*GrVector2D(0+angle);
  m_vertex[1].p = pos+half_side*(up-right);
  m_vertex[1].tex = center + mag*GrVector2D(90+angle);
  m_vertex[2].p = pos+half_side*(-up-right);
  m_vertex[2].tex = center + mag*GrVector2D(180+angle);
  m_vertex[3].p = pos+half_side*(-up+right);
  m_vertex[3].tex = center + mag*GrVector2D(270+angle);
  m_pos = m_vertex[0].p;
  m_D = m_pos^m_norm;
  m_fromBase = m_pos - base_point;
}



void CStainContainer::onDraw() const {
  if (!empty()) {

    // Save the state on stack
    rnd::pushAttrib (rnd::DEPTH_BUFFER_BIT | rnd::COLOR_BUFFER_BIT | rnd::TEXTURE_BIT);
      
      tex_container->glBind("bullet_mrk");
      
      rnd::depthMask(false);  // Don't write stains to depth buffer
                              // to prevent z-buffer fighting
      rnd::depthFunc(GL_LEQUAL); // When stain is on the same depth as the wall
                              // draw the stain
      //
      // Blend the stain with the original color
      rnd::enable(GL_BLEND);     
      rnd::blendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

      CObjectContainer<CStain>::onDraw();

    rnd::popAttrib(); // Restore the previous values
  }


}


void CStainContainer::onKeepIntegrity() {

  // traverse deque nodes
  for (deque<CStain *>::iterator i = begin(); i < end(); ++i) {
    (*i)->m_pos = m_pos + (*i)->m_fromBase;
    (*i)->onKeepIntegrity(); 
  }  
}



//
// @(#) $Log: stain.cpp,v $
// @(#) Revision 1.23  2003/12/08 11:08:19  virzak
// @(#) Changed CContainer to CObjectContainer as base class
// @(#)
// @(#) Revision 1.22  2003/12/08 07:04:31  virzak
// @(#) changes as a result of container bs
// @(#)
// @(#) Revision 1.21  2003/10/21 05:51:25  virzak
// @(#) Replaced to use planeProject
// @(#)
// @(#) Revision 1.20  2003/10/18 04:37:51  virzak
// @(#) small bug
// @(#)
// @(#) Revision 1.19  2003/10/18 04:27:00  virzak
// @(#) CStain is now derived from CQuad
// @(#)
// @(#) Revision 1.18  2003/10/18 03:00:52  virzak
// @(#) Changed onUpdate to onKeepIntegrity
// @(#)
// @(#) Revision 1.17  2003/09/30 06:58:51  virzak
// @(#) Reverted CStainContainer::onUpdate to previous version to preserve
// @(#) virtual behaviour
// @(#) Added CStain::m_fromBase member that is used to calculate vertices
// @(#) from the position of the container and changed CStainContainer::onUpdate
// @(#) to have that.
// @(#) Removed m_owner because of inheritance
// @(#)
// @(#) Revision 1.16  2003/09/29 03:45:09  virzak
// @(#) Implemented onUpdate with CStain and CStainContainer
// @(#)
// @(#) Revision 1.15  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.14  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.13  2003/08/29 08:33:58  virzak
// @(#) Changed texture handling
// @(#)
// @(#) Revision 1.12  2003/08/28 09:10:30  virzak
// @(#) Changed texture access
// @(#)
// @(#) Revision 1.11  2003/08/25 20:08:01  virzak
// @(#) Removed a comment
// @(#)
// @(#) Revision 1.10  2003/08/25 20:00:19  virzak
// @(#) Stain container keeps track of stain textures
// @(#) Stain keeps texture coordinates
// @(#)
// @(#) Revision 1.9  2003/08/24 22:05:43  virzak
// @(#) Fixed g++ bug
// @(#)
// @(#) Revision 1.8  2003/08/24 20:02:19  virzak
// @(#) Changed CStainContainer to be a derived class of CContainer
// @(#) and changed all functions accordingly
// @(#)
// @(#) Revision 1.7  2003/08/24 06:19:49  virzak
// @(#) Added CNode interface to stains. Added drawing capabilities.
// @(#) Added class CStainContainer which is responsible for collecting stains
// @(#) and seting drawing state on stain drawing.
// @(#)
// @(#) Revision 1.6  2003/08/20 02:40:50  virzak
// @(#) g++ compatible
// @(#)
// @(#) Revision 1.5  2003/08/16 05:51:44  virzak
// @(#) up vector is random
// @(#)
// @(#) Revision 1.4  2003/08/02 20:21:09  virzak
// @(#) Stain now also stores the normal
// @(#)
// @(#) Revision 1.3  2003/07/11 04:03:23  virzak
// @(#) Got rid of getComponent where is made mode sence to put x(), y(), or z()
// @(#)
// @(#) Revision 1.2  2003/07/04 18:57:02  virzak
// @(#) Make the stain more realistic
// @(#)
// @(#) Revision 1.1  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
//

