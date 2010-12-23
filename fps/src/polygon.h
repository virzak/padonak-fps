//*****************************************************************************
// @(#) $Id: polygon.h,v 1.17 2003/12/05 17:08:05 virzak Exp $ 
// 
//  Description: 
//      Provides polygon functions such as loading, drawing, intersections
//      and others
//
//  =
//
//*****************************************************************************



#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "util.h"
#include <fstream>
#include "kobject.h"



class CPolygon : virtual public CObject {
    
  protected:
    
    const unsigned int m_n;

  public:
    CVertex *m_vertex;
    GrVector3D m_norm;
    
    double m_D;
    
  public:

    // Constructor
    CPolygon(const unsigned int n) : m_n(n) {
      m_vertex = new CVertex [m_n];
    }

    virtual ~CPolygon() {
      delete [] m_vertex;
    }
    
    // Copy constructor
    CPolygon(const CPolygon& p) : 
      CObject(p), m_n(p.m_n), m_norm(p.m_norm), m_D(p.m_D) {
      m_vertex = new CVertex [m_n];
      for (unsigned int i = 0; i<m_n; ++i) {
        m_vertex[i] = p.m_vertex[i];
      }
    }

    // Assignment operator
    CPolygon & operator=(const CPolygon & p) {
      this->CObject::operator=(p);
      
      for (unsigned int i = 0; i<m_n; ++i) {
        m_vertex[i] = p.m_vertex[i];
      }
      m_norm = p.m_norm;
      m_D = p.m_D;
      return *this;
    }
    
    virtual const GrVector3D norm(const GrPoint3D & ) const {
      return m_norm;
    }

    virtual void reverse();
    
    virtual void onDraw() const;
    virtual bool load(ifstream & file);

    // Derives internal state from m_pos, m_velocity...
    virtual void onKeepIntegrity();


    void intersect (CIResult * result, const CGrRay3D & ray, 
        const double & min_dist = 0) const;
    void intersect (CIResult * result, const GrPoint3D & pt, 
        const double & min_dist = 0) const;
    bool intersectPlane(CIResult * result, const CGrRay3D & ray) const;
    bool intersectPlane(CIResult * result, const GrPoint3D & pt) const;
    bool inConvex(const GrPoint3D & pt) const;

    
    

    double distToPlane(const GrPoint3D & pt) const {
      return (m_norm^pt) - m_D;
    }
    
    bool onPlane(const GrPoint3D & pt) const {
      return (distToPlane(pt) == 0);
    }
    
    friend ostream & operator << (ostream &os, const CPolygon & p);
};


inline ostream & operator << (ostream &os, const CPolygon & p) {
  os << p.m_n << " Vertices: (";
  for (unsigned int i=0; i<p.m_n; ++i) {
    os << p.m_vertex[i].p;
    if (i != p.m_n-1) os << ',';
  }
  os << ") norm: ";
  os << p.m_norm;
  
  return os;
}




class CTriangle : public CPolygon {
  public:
    CTriangle() : CPolygon(3) {}
    CTriangle(const CPolygon & p) : CPolygon(p) {}
};


class CQuad : public CPolygon {
  public:
    CQuad() : CPolygon(4) {}
    CQuad(const CPolygon & p) : CPolygon(p) {}
};



#endif /*__POLYGON_H__*/


//
// @(#) $Log: polygon.h,v $
// @(#) Revision 1.17  2003/12/05 17:08:05  virzak
// @(#) moved Norm function to object.h
// @(#)
// @(#) Revision 1.16  2003/10/21 04:29:12  virzak
// @(#) Moved all Wall related into from polygon.cpp polygon.h and world.h to
// @(#) wall.cpp and wall.h
// @(#)
// @(#) Revision 1.15  2003/10/18 00:32:59  virzak
// @(#) Added virtual void onKeepIntegrity function that updates the vertices
// @(#) and the distance.
// @(#) Removed anything related to CMovingPolygon
// @(#)
// @(#) Revision 1.14  2003/10/09 20:49:25  virzak
// @(#) Changed the polygons to stop at the specified position
// @(#) as opposed to start reversing there.
// @(#)
// @(#) Revision 1.13  2003/10/05 01:20:33  virzak
// @(#) Fixed OP= and CC for CMovingPolygon
// @(#)
// @(#) Revision 1.12  2003/10/05 01:03:43  virzak
// @(#) Changed onUpdate to make the moving velocity controlled as opposed to
// @(#) time-controlled
// @(#)
// @(#) Revision 1.11  2003/09/30 07:35:35  virzak
// @(#) Changed onUpdate to record the new position
// @(#) Added m_basePos member to help calculate the position
// @(#) Removed all the stain related into into CWall which is
// @(#) m_stain_container member and genStain method
// @(#) Replaced assignements with initialisations in constructors
// @(#)
// @(#) Revision 1.10  2003/09/29 03:42:40  virzak
// @(#) Stain container is now a pointer member
// @(#)
// @(#) Revision 1.9  2003/09/21 06:31:59  virzak
// @(#) Replaced m_dist with m_D
// @(#) Improved OP= and CC
// @(#) Added class CMovingPolygon for moving polygons
// @(#)
// @(#) Revision 1.8  2003/09/18 03:55:25  virzak
// @(#) Added ostream & operator << for CPolygon
// @(#)
// @(#) Revision 1.7  2003/09/17 06:37:52  virzak
// @(#) Added more intersection functions
// @(#)
// @(#) Revision 1.6  2003/09/17 03:52:31  virzak
// @(#) Added CPolygon::intersectPlane and CPolygon::inConvex and changed
// @(#) CPolygon::intersect to work with CGrRay3D
// @(#)
// @(#) Revision 1.5  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.4  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.3  2003/09/09 22:50:05  virzak
// @(#) Methods distToPlane, onPlane and intersect are now const
// @(#) tri member of CIResult have been renamed to poly
// @(#) Added CIResult constructor
// @(#) Added m_dist member to Polygon to specify the plane constant
// @(#)
// @(#) Revision 1.2  2003/08/26 03:24:22  virzak
// @(#) Added #include <fstream>
// @(#)
// @(#) Revision 1.1  2003/08/25 20:10:53  virzak
// @(#) Initial code: Moved all the polygon related code from other files here
// @(#)
//


