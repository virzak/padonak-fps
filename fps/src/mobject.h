//*****************************************************************************
// @(#) $Id: mobject.h,v 1.4 2003/10/18 07:57:45 virzak Exp $ 
// 
//  Description: 
//      Contains ABC CMovingObject. This is an interface to any object 
//    that moves from side to side in an elevator style.
//
//  =
//
//*****************************************************************************


#ifndef __MOBJECT_H__
#define __MOBJECT_H__

#include "object.h"

class CMovingObject : virtual public CObject {
  protected:
    GrPoint3D m_basePos;
    unsigned int m_axis;
    double m_dist;
    double m_speed;
    bool m_changing;
  public:
    CMovingObject(unsigned int axis, double dist, double speed) 
      : m_axis(axis), m_dist(dist), m_speed(speed), m_changing(true) {
        m_acceleration.setComponent(m_axis, m_speed);
      }

    virtual bool load(ifstream & /*file*/) {
      m_basePos = m_pos;
      return true;
    }
    
    
    virtual void onUpdate(unsigned int msecs=0);

};


#endif /*__MOBJECT_H__*/

//
// @(#) $Log: mobject.h,v $
// @(#) Revision 1.4  2003/10/18 07:57:45  virzak
// @(#) Changed icc-warning 869 disabling to removing parameter's name
// @(#)
// @(#) Revision 1.3  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.2  2003/10/18 01:22:49  virzak
// @(#) Eliminated icc warning
// @(#)
// @(#) Revision 1.1  2003/10/18 00:25:10  virzak
// @(#) Contains class CMovingObject
// @(#)
//


