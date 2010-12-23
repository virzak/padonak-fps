//*****************************************************************************
// @(#) $Id: object.cpp,v 1.1 2003/12/05 17:06:16 virzak Exp $ 
// 
//  Description: 
//      ABC of all objects. This file contains ray to sphere intersection
//      The sphere is taken to be the bounding sphere 
//      and the position of the object is the center
//
//  =
//
//*****************************************************************************

#include "object.h" 
// Performs ray intersection with an object
void CObject::onRayIntercection (
    const CGrRay3D & ray, 
    CIResult * result
) const {

  if (m_sphereSize == 0)
    return;

  double A,B,C,t[2];
  double dist;
  double disc;
  
  A = ray.dir().x()*ray.dir().x() + 
      ray.dir().y()*ray.dir().y() + 
      ray.dir().z()*ray.dir().z();
  
  B = 2*(ray.dir()^(ray.pt() - m_pos));

  C = (ray.pt().x()-m_pos.x()) * (ray.pt().x()-m_pos.x()) +
      (ray.pt().y()-m_pos.y()) * (ray.pt().y()-m_pos.y()) +
      (ray.pt().z()-m_pos.z()) * (ray.pt().z()-m_pos.z()) -
      m_sphereSize * m_sphereSize;

  disc = B*B-4*A*C;
  if (disc < 0) {
    return;
  }
    
  t[0] = (-B-sqrt(disc))/2;
  t[1] = (-B+sqrt(disc))/2;

  if (t[0] < 0 && t[1] < 0) {
    return;
  }

  if (t[0] >= 0 && t[1] >= 0) {
    dist = t[0]<t[1]?t[0]:t[1];
  } else {
    dist = t[0]>=0?t[0]:t[1];
  }

  if (result->obj && result->dist <= dist)
    return;
  
  result->pt = (ray.pt() + (dist * ray.dir()));
  //result->normal = svMpy(1/sphere.radius, ppSub(result->point, sphere.center));
  result->dist = dist;
  result->obj = (CObject *) this;

  //return dist;
}



//
// @(#) $Log: object.cpp,v $
// @(#) Revision 1.1  2003/12/05 17:06:16  virzak
// @(#) New file: contains ray-to-sphere intersection
// @(#)
//



