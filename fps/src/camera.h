//*****************************************************************************
// @(#) $Id: camera.h,v 1.6 2003/10/18 05:07:28 virzak Exp $ 
// 
//  Description: 
//      CCamera class contains the information about the camera position and 
//    direction as well OpenGL commands to put the camera in the world
//
//  =
//
//*****************************************************************************

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "player.h"

class CCamera : public CObject {
    CPlayer * m_p;
    GrVector3D m_look_dir;
    GrVector3D m_look_dir_diff;
    double m_lookup;
    double m_rotY;

    GrMatrix4x4 m_left_right_rot;
    GrMatrix4x4 m_up_down_rot;
    bool m_eye_view;
    
    double m_dist;
    
    CGrRay3D m_ray;

  public:
    
    void onDraw() const;
    void onUpdate(unsigned int msecs=0);
    
    void init();

    CCamera(CPlayer * p) : 
      m_p(p), m_eye_view(true), m_dist(0.8f) {
        init();
      }
    
    
    void switchView() {
      m_eye_view = !m_eye_view;
      init();
    }
    

    void changeLook(double lr, double ud, bool force=false);

    friend class CUI;
};



#endif


//
// @(#) $Log: camera.h,v $
// @(#) Revision 1.6  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.5  2003/10/04 22:26:16  virzak
// @(#) Added m_dist parameter to change the distance when the camera is
// @(#) behind the player
// @(#)
// @(#) Revision 1.4  2003/09/21 20:24:31  virzak
// @(#) Added m_ray member
// @(#) Added onUpdate function
// @(#)
// @(#) Revision 1.3  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.2  2003/08/27 22:12:04  virzak
// @(#) Camera is now a derived from CObject and implements onDraw
// @(#)
// @(#) Revision 1.1  2003/08/02 20:10:35  virzak
// @(#) Initial code for camera handling
// @(#)
//


