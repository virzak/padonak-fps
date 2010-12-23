//*****************************************************************************
// @(#) $Id: camera.cpp,v 1.12 2003/10/18 07:59:21 virzak Exp $ 
// 
//  Description: 
//      CCamera class contains the information about the camera position and 
//    direction as well OpenGL commands to put the camera in the world
//
//  =
//
//*****************************************************************************

#include "camera.h"
#include "engine.h"
#include <GL/gl.h>
#include <GL/glu.h>


inline void gluLookAtv (const GrPoint3D & eye, const GrPoint3D & center, const GrVector3D & up) {

  gluLookAt(  eye.x(), eye.y(), eye.z(),
              center.x(), center.y(), center.z(),
               up.x(), up.y(), up.z());

}


inline void gluLookAtv (const CGrRay3D & ray) {
  gluLookAtv(ray.pt(), ray.pt()+ray.dir(), GrVector3D(0,1,0));
}




void CCamera::onDraw() const {
  GrVector3D up = GrVector3D(0,1,0);
  
  /*
  up += 2*cp->m_walkbias*p->m_move_right_dir;
  up.normalize();
  */

  gluLookAtv(
    m_p->m_eye_pos-(!m_eye_view)*(double)m_dist*(m_p->m_look_dir+m_look_dir_diff),
    m_p->m_eye_pos+(double)m_eye_view*m_p->m_look_dir, 
    up
  );
  
  // TODO: Find out why this causes trouble
  // gluLookAtv(m_ray);
}

void CCamera::onUpdate(unsigned int /*msecs*/) {
  if (m_eye_view) {
    m_ray.pt(m_p->m_eye_pos);
    m_ray.dir(m_p->m_look_dir);
  } else {
    m_ray.pt(m_p->m_eye_pos-(double)m_dist*(m_p->m_look_dir+m_look_dir_diff));
    m_ray.dir(m_p->m_eye_pos-m_ray.pt());
  }
}


void CCamera::changeLook(double right_angle, double up_angle, bool force) {
  
  if (right_angle || force) {
    m_rotY -= right_angle;
    m_left_right_rot = GrMatrix4x4(m_rotY, 'y');  
  }
  
  
  if (up_angle || force) {  
    m_lookup += up_angle;
    if (m_lookup>179.5) m_lookup = 179.5;
    if (m_lookup<.5) m_lookup = .5; 
    m_up_down_rot = GrMatrix4x4(m_lookup, 'x');
  }

  m_look_dir = m_left_right_rot*(m_up_down_rot*GrVector3D(0,-1,0));
  m_look_dir_diff = m_look_dir - m_p->m_look_dir;
}


void CCamera::init() {
  m_look_dir=m_p->m_look_dir;
  m_lookup = m_p->m_lookup;
  m_rotY = m_p->m_rotY;
  changeLook(0, 0, true);
}


//
// @(#) $Log: camera.cpp,v $
// @(#) Revision 1.12  2003/10/18 07:59:21  virzak
// @(#) Got rid of icc warning 869
// @(#)
// @(#) Revision 1.11  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.10  2003/10/04 22:26:16  virzak
// @(#) Added m_dist parameter to change the distance when the camera is
// @(#) behind the player
// @(#)
// @(#) Revision 1.9  2003/09/22 01:34:10  virzak
// @(#) Fixed bug
// @(#)
// @(#) Revision 1.8  2003/09/22 00:45:18  virzak
// @(#) Fixed for windows
// @(#)
// @(#) Revision 1.7  2003/09/21 20:25:22  virzak
// @(#) Added m_ray member
// @(#) Added onUpdate function
// @(#) Overloaded gluLookAtv witha CGrRay3D parameter
// @(#)
// @(#) Revision 1.6  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.5  2003/09/04 01:09:40  virzak
// @(#) Removed outpututil.h from includes
// @(#)
// @(#) Revision 1.4  2003/08/27 22:12:04  virzak
// @(#) Camera is now a derived from CObject and implements onDraw
// @(#)
// @(#) Revision 1.3  2003/08/14 01:11:06  virzak
// @(#) Redefined m_lookup from looking down as 0 and up as 180, ranges 0.5-179.5
// @(#)
// @(#) Revision 1.2  2003/08/02 20:46:47  virzak
// @(#) Small windows bug
// @(#)
// @(#) Revision 1.1  2003/08/02 20:10:35  virzak
// @(#) Initial code for camera handling
// @(#)
//


