//*****************************************************************************
// @(#) $Id: wall.h,v 1.4 2003/12/08 11:15:55 virzak Exp $ 
// 
//  Description: 
//      CWall and CMovingWall classes
//
//  =
//
//*****************************************************************************


#ifndef __WALL_H__
#define __WALL_H__

#include "stain.h"
#include "polygon.h"
#include "mobject.h"

class CWall : public CPolygon, protected CStainContainer, public CFloorObject {
  
  public:
    // Draws the wall
    virtual void onDraw() const {
      CPolygon::onDraw();
      CStainContainer::onDraw();
    }

    // Updates the wall
    virtual void onUpdate(unsigned int /*msecs=0*/) {
      // No need - stationary
      // CPolygon::onUpdate();
      // CStainContainer::onUpdate();
    }
    
    // Constructor
    CWall(const unsigned int n) : CPolygon(n) {}

    // Copy constructor (Do need to copy the container)
    CWall(const CWall& w) : CPolygon(w) {}
    
    // Assignment operator (Do need to copy the container)
    CWall & operator=(const CWall& w) {
      this->CPolygon::operator=(w);
      return *this;
    }


    virtual void onBulletHit(const GrPoint3D & pos) {
      addStain(pos);
    }

    
    // Adds a stain to the wall
    void addStain(const GrPoint3D & pt) {
      CStain *stain;
      if (fabs(m_norm.y())<0.99f) {
        stain = new CStain(pt+0.01*m_norm, m_norm, m_pos);
      } else {
        stain = new CStain(pt+0.01*m_norm, m_norm, m_pos,
            m_vertex[0].p-m_vertex[1].p);
      }
      push_back(stain);
    }
    
    virtual void onKeepIntegrity() {
      CPolygon::onKeepIntegrity();
      CStainContainer::onKeepIntegrity();
    }
    
    virtual void checkCollision(CKineticObject * kobj) const;
    
    //virtual GrVector3D getNorm() const {return norm(GrPoint3D());};
    
    virtual const GrVector3D norm(const GrPoint3D & p) const {
      return CPolygon::norm(p);
    }

};



class CMovingWall : public CWall, public CMovingObject {
  
  public:
    
    // Constructor
    CMovingWall(const unsigned int n, unsigned int axis, double dist,
        double speed) : CWall(n), CMovingObject(axis, dist, speed) {}

    virtual void onUpdate(unsigned int msecs=0) {
      CMovingObject::onUpdate(msecs);
    }
    
    virtual bool load(ifstream & file) {
      bool res = CWall::load(file);
      CMovingObject::load(file);
      return res;
    }

    virtual const GrVector3D norm(const GrPoint3D & p) const {
      return CWall::norm(p);
    }

    virtual void onDraw() const {
      CWall::onDraw();
    }

    virtual void onBulletHit(const GrPoint3D & pos) {
      CWall::onBulletHit(pos);
    }

    virtual void onKeepIntegrity() {
      CWall::onKeepIntegrity();
    }

};



#endif /*__WALL_H__*/

//
// @(#) $Log: wall.h,v $
// @(#) Revision 1.4  2003/12/08 11:15:55  virzak
// @(#) Fixed windows related warnings of MI
// @(#)
// @(#) Revision 1.3  2003/12/08 07:04:31  virzak
// @(#) changes as a result of container bs
// @(#)
// @(#) Revision 1.2  2003/12/05 17:10:09  virzak
// @(#) Added onBulletHit, removed getNorm
// @(#)
// @(#) Revision 1.1  2003/10/21 04:29:12  virzak
// @(#) Moved all Wall related into from polygon.cpp polygon.h and world.h to
// @(#) wall.cpp and wall.h
// @(#)
//

