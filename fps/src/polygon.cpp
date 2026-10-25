//*****************************************************************************
// @(#) $Id: polygon.cpp,v 1.21 2003/12/05 17:07:35 virzak Exp $ 
// 
//  Description: 
//      Provides polygon functions such as loading, drawing, intersections
//      and others
//
//  =
//
//*****************************************************************************



#include "polygon.h"
#include "engine.h"
#include <math.h>


#define POLYGON_DROP_COMPONENT_IN_CONVEX 1
#define DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX 0

static bool getVertex(ifstream & file, CVertex & ver) {
  float x, y, z, u, v;
  
  // Read in the vertices
  file >> x;
  if (file.eof()) {return false;}
  file >> y;
  if (file.eof()) {return false;}
  file >> z;
  if (file.eof()) {return false;}

  // Read in the texture coordinates
  file >> u;
  if (file.eof()) {return false;}
  file >> v;
  if (file.eof()) {return false;}
  
  ver.p = GrPoint3D(x, y, z);
  ver.tex = GrPoint2D(u, v);
  return true;
}



bool CPolygon::load(ifstream & file) {
  string type;
  
  for (unsigned int vertLoop = 0; vertLoop < m_n; ++vertLoop)
  { 
    // Read type of info
    if (!CWorld::readStr(file, type)) {
      return false;
    }
    
    // Read info itself according to the type
    if (type[0] == 'v') {
      if (!getVertex(file, m_vertex[vertLoop])) {
        return false;
      }
      
    } else {
      *output << "Scripting error: Unknown option in readTriangle " << type << endl;
    }

  }

  // Generate normal
  m_norm = GrMkNormal (m_vertex[0].p, m_vertex[1].p, m_vertex[2].p);

  // Find D as in AX+BY+CZ+D=0
  m_D = m_vertex[0].p^m_norm;

  // Set position
  m_pos = m_vertex[0].p;
  
  return true;
 
}


void CPolygon::reverse() {

  // Generate new polygon
  CPolygon new_p(*this);
  
  // Reverse vertex order
  new_p.m_vertex[0] = m_vertex[0];
  for (unsigned int i = 1; i<m_n; ++i) {
    new_p.m_vertex[i] = m_vertex[m_n-i];
  }

  // Negate normal and planar distance
  new_p.m_norm = -m_norm;
  new_p.m_D = -m_D;

  *this = new_p;
  
}

  
bool CPolygon::intersectPlane(CIResult * result, const GrPoint3D & pt) const {
  
  result->dist = distToPlane(pt);

  /* Test whether the intersection is behind the eye point */
  if (result->dist<0) {
    return false;
  }
  
  // Find the resulting point on the plane
  result->pt = (pt-(result->dist*m_norm));
  return true;

}



bool CPolygon::intersectPlane(CIResult * result, const CGrRay3D & ray) const {
  
    
  /* Back face culling */
  /* And test whether the polygon and the viewing direction are parallel */  
  
  double ray_dot_normal = (ray.dir()^m_norm);
  if (ray_dot_normal >= 0) {
    return false;
  }
  
  /* Calculate the point of intersection with the plane */
 
  
  result->dist = ( m_D - (ray.pt()^m_norm)) / ray_dot_normal;

  /* Test whether the intersection is behind the eye point */
  if (result->dist<0) {
    return false;
  }
  
  // Find the resulting point on the plane
  result->pt = (ray.pt()+(result->dist*ray.dir()));
  return true;

}

bool CPolygon::inConvex(const GrPoint3D & pt) const {
  
#if POLYGON_DROP_COMPONENT_IN_CONVEX

  // Find dominant component
  unsigned int dominant = m_norm.dominant();

  // Drop it from the intersection point
  GrPoint2D pt2d = pt.drop(dominant);
  
#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
  *output << " m_norm : " << m_norm << "   Dominant : " << dominant << endl;
  
  *output << "pt2d: " << pt2d << endl;
  for (unsigned int i=0; i<m_n; ++i) {
    *output << "m_vertex[" << i << "].p " << m_vertex[i].p << " -> " <<
      m_vertex[i].p.drop(dominant) << endl;
  }
#endif

  
  for (unsigned int i=0; i<m_n; ++i) {
    
    GrPoint2D i_vtx(m_vertex[i].p.drop(dominant)); // Iterating polygon vertex
      
    // Get local normal by rotating the side of the polygon 90 degrees
    // to a side depending on the dominant component
    GrVector2D localnormal = (
       m_vertex[(i+1)%m_n].p.drop(dominant) - i_vtx
      ).rotate90(m_norm.getComponent(dominant)>0);


#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
    *output << i << ") localnormal^vec " << localnormal << "^" 
      << (pt2d - i_vtx) << " = " << 
      (localnormal^(pt2d - i_vtx)) << endl;
#endif
    
    // Test for cutoff
    if ((localnormal^(pt2d - i_vtx))<0) {
#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
      *output << "fail" << endl;
#endif
      return false;
    }
          
  }
  
#else
  
  for (unsigned int i=0; i<m_n; ++i) {

    // Get local normal
    GrVector3D localnormal = (m_norm *
        (
          m_vertex[(i+1)%m_n].p -
          m_vertex[i].p)
    );
    
    // Test for cutoff
    if ((localnormal^(pt - m_vertex[i].p))<0) {
      return false;
    }
  }
#endif

#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
  *output << "Stain!!!" << endl << endl << endl << endl;
#endif
  
  return true;  
}


void CPolygon::intersect(CIResult * result, const CGrRay3D & ray, 
    const double & min_dist) const {

  result->obj = (intersectPlane(result, ray) 
      && (min_dist==0 || result->dist <= min_dist)
#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
      && (&(*output << "dist: " << result->dist) != NULL)
#endif
      && inConvex(result->pt)) ?
    (CPolygon *) this : NULL ;
}


void CPolygon::intersect(CIResult * result, const GrPoint3D & pt, 
    const double & min_dist) const {

  result->obj = (intersectPlane(result, pt) 
      && (min_dist==0 || result->dist <= min_dist)
#if DEBUG_POLYGON_DROP_COMPONENT_IN_CONVEX
      && (&(*output << "dist: " << result->dist) != NULL)
#endif
      && inConvex(result->pt)) ?
    (CPolygon *) this : NULL ;
}



void CPolygon::onDraw() const {

  if (m_tex) {
    m_tex->glBind();
  }
  
  rnd::begin(m_n==3 ? rnd::TRIANGLES : rnd::QUADS);
    rnd::normal3v(m_norm);
    for (unsigned int i=0; i<m_n; ++i) {
      rnd::texCoord2v(m_vertex[i].tex);
      rnd::vertex3v(m_vertex[i].p);
    }

  rnd::end();
}



// Derives internal state from m_pos, m_velocity...
void CPolygon::onKeepIntegrity() {

  // Find displacement for all points
  GrVector3D local_disp = m_pos - m_vertex[0].p;
  
  
  // Change actual vertex positions
  for (unsigned int vertLoop = 0; vertLoop < m_n; ++vertLoop)
    m_vertex[vertLoop].p += local_disp;
  

  // Find planar distance again
  m_D = m_vertex[0].p^m_norm;
}



//
// @(#) $Log: polygon.cpp,v $
// @(#) Revision 1.21  2003/12/05 17:07:35  virzak
// @(#) small changes
// @(#)
// @(#) Revision 1.20  2003/10/21 08:37:42  virzak
// @(#) Added texture binding for a polygon
// @(#)
// @(#) Revision 1.19  2003/10/21 04:29:12  virzak
// @(#) Moved all Wall related into from polygon.cpp polygon.h and world.h to
// @(#) wall.cpp and wall.h
// @(#)
// @(#) Revision 1.18  2003/10/18 00:32:53  virzak
// @(#) Added virtual void onKeepIntegrity function that updates the vertices
// @(#) and the distance.
// @(#) Removed anything related to CMovingPolygon
// @(#)
// @(#) Revision 1.17  2003/10/09 20:49:25  virzak
// @(#) Changed the polygons to stop at the specified position
// @(#) as opposed to start reversing there.
// @(#)
// @(#) Revision 1.16  2003/10/09 00:22:38  virzak
// @(#) Fixed for negative movement
// @(#)
// @(#) Revision 1.15  2003/10/05 01:22:50  virzak
// @(#) Removed unnecessary code
// @(#)
// @(#) Revision 1.14  2003/10/05 01:03:43  virzak
// @(#) Changed onUpdate to make the moving velocity controlled as opposed to
// @(#) time-controlled
// @(#)
// @(#) Revision 1.13  2003/09/30 22:46:01  virzak
// @(#) In CMovingPolygon::onUpdate handling the case where timer change is 0.
// @(#)
// @(#) Revision 1.12  2003/09/30 07:35:35  virzak
// @(#) Changed onUpdate to record the new position
// @(#) Added m_basePos member to help calculate the position
// @(#) Removed all the stain related into into CWall which is
// @(#) m_stain_container member and genStain method
// @(#) Replaced assignements with initialisations in constructors
// @(#)
// @(#) Revision 1.11  2003/09/29 03:42:40  virzak
// @(#) Stain container is now a pointer member
// @(#)
// @(#) Revision 1.10  2003/09/22 00:45:18  virzak
// @(#) Fixed for windows
// @(#)
// @(#) Revision 1.9  2003/09/22 00:28:33  virzak
// @(#) Collisions are now continious, meaning after a responce, collision
// @(#) still occurs until a position is changed
// @(#)
// @(#) Revision 1.8  2003/09/21 07:58:18  virzak
// @(#) Rewrote onUpdate
// @(#)
// @(#) Revision 1.7  2003/09/21 06:38:31  virzak
// @(#) Reverse now updtes itself instead of returning an object.
// @(#) This is done in order to implement hierarchy.
// @(#) Added CPolygon::checkCollision that determines if there is a collision
// @(#) and calls handler if there is
// @(#)
// @(#) Revision 1.6  2003/09/17 06:37:52  virzak
// @(#) Added more intersection functions
// @(#)
// @(#) Revision 1.5  2003/09/17 03:58:27  virzak
// @(#) Broke donwn intersect to intersectPlane, inConvex and added a
// @(#) distance comparison test.
// @(#) Reimplemented inConvex with dropping a component and performing
// @(#) the test in 2D
// @(#)
// @(#) Revision 1.4  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.3  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.2  2003/09/09 22:50:05  virzak
// @(#) Methods distToPlane, onPlane and intersect are now const
// @(#) tri member of CIResult have been renamed to poly
// @(#) Added CIResult constructor
// @(#) Added m_dist member to Polygon to specify the plane constant
// @(#)
// @(#) Revision 1.1  2003/08/25 20:10:53  virzak
// @(#) Initial code: Moved all the polygon related code from other files here
// @(#)
//


