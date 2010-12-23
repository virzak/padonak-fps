//*****************************************************************************
// @(#) $Id: wall.cpp,v 1.2 2003/12/05 17:09:36 virzak Exp $ 
// 
//  Description: 
//      Wall routines
//
//  =
//
//*****************************************************************************


#include "wall.h"

void CWall::checkCollision(CKineticObject * kobj) const {
  CIResult cur_coll;
  
  // Straight to the wall plane
  intersect(&cur_coll, kobj->pos() + 
            GrVector3D(0, kobj->sphereSize(), 0), kobj->sphereSize());
  
  if (cur_coll.obj) {
    kobj->onCollision(m_norm, cur_coll.dist, this);  
  } else {
    
    // Check velocity intersection only if the distance to the plane is 
    // less than the size of bounding sphere
    if (0 < cur_coll.dist && cur_coll.dist <= kobj->sphereSize()) {

    
      // Ray direction is the velocity normalized
      GrVector3D tmp_vec(kobj->velocity());
      tmp_vec.normalize();

      
      intersect(&cur_coll, CGrRay3D(kobj->pos() + 
            GrVector3D(0, kobj->sphereSize(), 0), tmp_vec), 
                  kobj->sphereSize());
      
      if (cur_coll.obj) {    
        kobj->onCollision(m_norm, -tmp_vec, cur_coll.dist);
      }
    }
  }
}


//
// @(#) $Log: wall.cpp,v $
// @(#) Revision 1.2  2003/12/05 17:09:36  virzak
// @(#) small changes
// @(#)
// @(#) Revision 1.1  2003/10/21 04:29:12  virzak
// @(#) Moved all Wall related into from polygon.cpp polygon.h and world.h to
// @(#) wall.cpp and wall.h
// @(#)
//

