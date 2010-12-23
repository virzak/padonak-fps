//*****************************************************************************
// @(#) $Id: kobject.h,v 1.15 2003/10/21 05:52:36 virzak Exp $ 
// 
//  Description: 
//      This file implements objects that move
//
//  =
//
//*****************************************************************************

#ifndef __KOBJECT_H__
#define __KOBJECT_H__

#include "fobject.h"
#include "timer.h"

class CKineticObject : public CObject {
  protected:

    const CFloorObject * m_floor;
    
  public:

    CKineticObject() : m_floor(NULL) {}
    
    // perform basic physics on the object
    virtual void onUpdate(unsigned int msecs=0) {
      
      if (msecs==0) msecs = timer.getChange();
        
      m_velocity += (double)msecs/1000.f*m_acceleration;
      m_pos += (double)msecs/1000.f*m_velocity;
    }
    
    virtual void onCollision(const GrVector3D & norm, const double & dist,
        const CFloorObject * other) {
      m_pos += (m_sphereSize - dist) * norm;

      if (norm.y() > 0.8f) {

        // Set the floor object
        m_floor = other;
        
        // Recalculate the velocity from the velocity of the other object.
        GrVector3D relative_vel = m_velocity - other->velocity();
        m_velocity -= (relative_vel^norm)*norm;
      }
    }
    
    virtual void onCollision(const GrVector3D & norm, const GrVector3D & dir, 
        const double & dist) {
      m_pos += (((m_sphereSize - dist) * dir)^norm)*norm;
    }


    // High speed collisions
    virtual void onCollision(const GrPoint3D & pt, const GrVector3D & norm) {

      // Determine the position
      GrVector3D norm_vel(m_velocity);
      norm_vel.normalize();
      m_pos = pt-(m_sphereSize*norm_vel) - GrVector3D(0, m_sphereSize, 0);

      // Determine the new velocity
      m_velocity.planeProject(norm);
    }
};

#endif /*__KOBJECT_H__*/

//
// @(#) $Log: kobject.h,v $
// @(#) Revision 1.15  2003/10/21 05:52:36  virzak
// @(#) Using planeProject function
// @(#)
// @(#) Revision 1.14  2003/10/21 04:27:02  virzak
// @(#) Rewrote the onCollision
// @(#)
// @(#) Revision 1.13  2003/10/18 11:13:56  virzak
// @(#) Added another type of collision handling
// @(#)
// @(#) Revision 1.12  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.11  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.10  2003/10/09 00:20:13  virzak
// @(#) Instead of m_inTheAir now storing a pointer to the floor object
// @(#)
// @(#) Revision 1.9  2003/10/05 01:02:59  virzak
// @(#) Changed generic onUpdate
// @(#)
// @(#) Revision 1.8  2003/09/30 06:50:41  virzak
// @(#) Reverted onUpdate to previous version to preserve virtual behaviour
// @(#)
// @(#) Revision 1.7  2003/09/29 03:45:50  virzak
// @(#) Added optional change parameter to onUpdate
// @(#)
// @(#) Revision 1.6  2003/09/22 00:29:44  virzak
// @(#) onCollision with a horizontal plane, the object assumes a vertical
// @(#) velocity of the other object
// @(#)
// @(#) Revision 1.5  2003/09/21 06:07:34  virzak
// @(#) Added member m_inTheAir that tells us if the object is in the air
// @(#) It is set to true on a collision with a polygon with high y-comp
// @(#) of the normal
// @(#)
// @(#) Revision 1.4  2003/09/18 03:53:44  virzak
// @(#) Added another collision detection function to work when the normal
// @(#) is not equal to the push.
// @(#)
// @(#) Revision 1.3  2003/09/17 06:58:40  virzak
// @(#) changed an include from node.h to object.h
// @(#)
// @(#) Revision 1.2  2003/09/17 06:38:25  virzak
// @(#) Implemented simple collision detection
// @(#)
//

