//*****************************************************************************
// @(#) $Id: mobject.cpp,v 1.2 2003/10/18 05:07:28 virzak Exp $ 
// 
//  Description: 
//      Contains ABC CMovingObject. This is an interface to any object 
//    that moves from side to side in an elevator style.
//
//  =
//
//*****************************************************************************


#include "mobject.h"
#include "timer.h"


void CMovingObject::onUpdate(unsigned int msecs) {


  const int dir = (m_speed > 0)? 1 : -1;
  
  if (m_changing) {
    // Changing directions
    if (dir * m_velocity.getComponent(m_axis) > dir * m_speed) {
      m_velocity.setComponent(m_axis, m_speed);
      m_acceleration.zero();
      m_changing = false;
    } else if (dir * m_velocity.getComponent(m_axis) < -dir * m_speed) {
      m_velocity.setComponent(m_axis, -m_speed);
      m_acceleration.zero();
      m_changing = false;
    }

  } else {
    // Setting constant velocity
    if (dir * m_pos.getComponent(m_axis) > 
        dir * (m_basePos.getComponent(m_axis)+m_dist - m_speed/2.f)) {
      m_acceleration.setComponent(m_axis, -m_speed);
      m_changing = true;
    } else if (dir * m_pos.getComponent(m_axis) < 
        dir * (m_basePos.getComponent(m_axis) + m_speed/2.f)) {
      m_acceleration.setComponent(m_axis, m_speed);
      m_changing = true;
    }
  }
  

  
  if (msecs==0) msecs = timer.getChange();
  
  // Find new position
  m_velocity += (double)msecs/1000.f*m_acceleration;
  m_pos += (double)msecs/1000.f*m_velocity;
  
  onKeepIntegrity();
}





//
// @(#) $Log: mobject.cpp,v $
// @(#) Revision 1.2  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.1  2003/10/18 00:25:10  virzak
// @(#) Contains class CMovingObject
// @(#)
//


