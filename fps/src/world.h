//*****************************************************************************
// @(#) $Id: world.h,v 1.41 2003/12/08 11:15:18 virzak Exp $ 
// 
//  Description: 
//      Contains the world information
//      
//
//*****************************************************************************


#ifndef __WORLD_H__
#define __WORLD_H__

#include "wall.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace::std;


/*
class CMaterialContainer : public CNamedContainer<GrMaterial> {
  
};
*/


typedef CNamedContainer<GrMaterial> CMaterialContainer;


class CMirror : public CObject {
  public:
    virtual void onDraw() const;
};


struct CMovement {
  unsigned int axis;
  double dist;
  double speed;
};


class CSector : public CObject {
    CStainContainer * m_stain_container;
    CObjectContainer<CWall> m_walls;
  public:
    ~CSector();
    CSector();
    void onDraw() const;
    void shot(const CGrRay3D & ray);
    virtual void onRayIntercection(const CGrRay3D &, CIResult *) const;
    bool addTriangle(ifstream & , bool );
    bool addPoly(ifstream & file, bool quad_double, unsigned int n,
      CMovement * movement, CTexture * tex);
    void checkCollisions(CKineticObject * kobj) const;

    void onUpdate(unsigned int msecs=0) {
      m_walls.onUpdate(msecs);
    }

    void loadStreet();

    double checkMovementCollisions(CKineticObject * kobj, const GrVector3D & v) const;
};

class CWorld : public CObject {
    static CWorld * m_instance;
    CWorld(const char * worldfile);
    void load(ifstream & filein);
    const double m_gravity;
  public: 
    static CWorld * instanceOf(const char * worldfile) {
      if (!m_instance) {
        m_instance = new CWorld(worldfile);
      }
      return m_instance;
    }
    
    void onDraw() const {}

    double getFrincionCoeff(const GrPoint3D & /*p*/) const { return 15; }
    static bool readStr(ifstream & file, string & type);
    double getGravity() const {return m_gravity;}
    ~CWorld() {
      m_instance = NULL;
    }
};

#endif /*__WORLD_H__*/



//
// @(#) $Log: world.h,v $
// @(#) Revision 1.41  2003/12/08 11:15:18  virzak
// @(#) Sector now has CObjectContainer<CWall> m_walls
// @(#)
// @(#) Revision 1.40  2003/12/05 17:12:17  virzak
// @(#) Came up with classes for patches and subdivisions
// @(#) CSubdivBox, CBasicSubdivBoxes, CSubdivBoxesAnim, CSubdivBoxesShot
// @(#) Instanciated their objects
// @(#) other small changes
// @(#)
// @(#) Revision 1.39  2003/10/21 08:42:53  virzak
// @(#) Now allowing for a polygon to specify its texture
// @(#)
// @(#) Revision 1.38  2003/10/21 04:29:12  virzak
// @(#) Moved all Wall related into from polygon.cpp polygon.h and world.h to
// @(#) wall.cpp and wall.h
// @(#)
// @(#) Revision 1.37  2003/10/18 11:19:34  virzak
// @(#) Added another type of collision detection for movement
// @(#) Divided shot routine into shot and closestIntersection
// @(#)
// @(#) Revision 1.36  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.35  2003/10/18 03:01:46  virzak
// @(#) Changed calls to CStainContainer::onUpdate to
// @(#) CStainContainer::onKeepIntegrity.
// @(#)
// @(#) Revision 1.34  2003/10/18 00:27:57  virzak
// @(#) CWall now extends CPolygon (not virtual) and CStainContainer
// @(#) CMovingWall now extends CWall and CMovingObject
// @(#)
// @(#) Revision 1.33  2003/09/30 07:51:49  virzak
// @(#) Added CMovingWall::load
// @(#)
// @(#) Revision 1.32  2003/09/30 07:22:12  virzak
// @(#) Added class CWall and class CMovingWall that inherit
// @(#) CPolygon and CMovingPolygon respectively and CStainContainer
// @(#)
// @(#) Revision 1.31  2003/09/29 03:43:39  virzak
// @(#) Stain container is now a pointer member
// @(#) Keeping all polygons in the same container
// @(#)
// @(#) Revision 1.30  2003/09/21 06:42:08  virzak
// @(#) Added moving polygons container
// @(#)
// @(#) Revision 1.29  2003/09/17 07:35:38  virzak
// @(#) Moved classes C3DObjects and CGUI2D from world.cpp world.h to
// @(#) objgroups.cpp objgroups.h
// @(#)
// @(#) Revision 1.28  2003/09/17 06:36:22  virzak
// @(#) Added CSector::checkCollisions signature
// @(#)
// @(#) Revision 1.27  2003/09/17 03:44:22  virzak
// @(#) CSector::shot now takes CGrRay3D instead of a point and a Vector
// @(#)
// @(#) Revision 1.26  2003/09/10 03:32:21  virzak
// @(#) Functions CWorld::ground and CWorld::getFrincionCoeff are now constant
// @(#) Added CWorld::getGravity()
// @(#) Defined CWorld::onDraw() since it is now a pure virtual function
// @(#)
// @(#) Revision 1.25  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.24  2003/08/29 08:33:31  virzak
// @(#) Added CWorld::load
// @(#)
// @(#) Revision 1.23  2003/08/28 09:06:09  virzak
// @(#) dded class CGUI2D to draw 2D UI stuff
// @(#)
// @(#) Revision 1.22  2003/08/27 21:33:58  virzak
// @(#) Added class C3DObjects that is used to draw 3D configuration
// @(#)
// @(#) Revision 1.21  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.20  2003/08/26 03:23:36  virzak
// @(#) Added required #include <string>
// @(#)
// @(#) Revision 1.19  2003/08/25 20:07:04  virzak
// @(#) Renamed struct Vertex to class CVertex and moved it to util.h and .cpp
// @(#) Moved class CIResult to the header
// @(#) Moved all polygon related functions to polygon.h and .cpp
// @(#) Made CSector::addPoly be generic for all polygons
// @(#) Moved getVertex function to polygon.h
// @(#) readStr is now a static function of world class
// @(#)
// @(#) Revision 1.18  2003/08/24 22:10:13  virzak
// @(#) Added CPolygon::load function to load polygon info
// @(#) Added CPolygon::reverse that returns polygon looking back
// @(#) Added CPolygon::intersect that checks for intersection
// @(#) Moved definition of class CIResult from world.cpp to world.h
// @(#) Added CPolygon::genStain that retuns a new stain
// @(#)
// @(#) Revision 1.17  2003/08/24 20:08:23  virzak
// @(#) Added CPolygon class which is the base class for triangle, quads
// @(#) and other polygons.
// @(#) Changed deque<T *> to CContainer<T>
// @(#)
// @(#) Revision 1.16  2003/08/24 06:20:42  virzak
// @(#) Changed deque<CStain *> to CStainContainer
// @(#)
// @(#) Revision 1.15  2003/08/02 20:31:35  virzak
// @(#) CSector is not a member of CWorld any longer, it is now an independant
// @(#) instance in the tree and world's child. It gets initialised in CWorld
// @(#) constructor and destroyed with the tree.
// @(#) CTriangle is also a derived class from CObject, however it is rendered
// @(#) as a part of CSector.
// @(#)
// @(#) Revision 1.14  2003/07/30 08:16:41  virzak
// @(#) Added CMirror, a mirror class
// @(#)
// @(#) Revision 1.13  2003/07/30 04:28:53  virzak
// @(#) more node changes
// @(#)
// @(#) Revision 1.12  2003/07/12 22:26:31  virzak
// @(#) Small fixes
// @(#)
// @(#) Revision 1.11  2003/07/12 01:09:58  virzak
// @(#) put texture into into GrPoint2D
// @(#)
// @(#) Revision 1.10  2003/07/10 19:23:09  virzak
// @(#) changed triangle array into deque
// @(#)
// @(#) Revision 1.9  2003/07/10 18:38:30  virzak
// @(#) fixed comments
// @(#)
// @(#) Revision 1.8  2003/07/09 05:01:46  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.7  2003/07/04 19:31:10  virzak
// @(#) Moved stains from world to triangle in order to be able to use
// @(#) GL_ALWAYS as depth function and not have these stains going to
// @(#) other polygons. GL_EQUAL doesn't work well.
// @(#)
// @(#) Revision 1.6  2003/07/04 18:57:02  virzak
// @(#) Make the stain more realistic
// @(#)
// @(#) Revision 1.5  2003/07/03 22:21:14  virzak
// @(#) Added a routine to get the ground height. To be moved to sector
// @(#)
// @(#) Revision 1.4  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
// @(#) Revision 1.3  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
//




