//*****************************************************************************
// @(#) $Id: object.h,v 1.10 2003/12/08 07:02:12 virzak Exp $ 
// 
//  Description: 
//      ABC of all objects
//
//  =
//
//*****************************************************************************


#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "node.h"
#include "textures.h"
#include "output.h"

class CObject;


class CIResult{
  public:
    CObject *obj;
    double dist;
    GrPoint3D pt;
    CIResult() : obj(NULL), dist(0) {}
};



// Abstract Base Class (ABC) - onDraw must be implemented
class CObject : public CGrDagNode {
  typedef void (CObject::*rec_func)();
  protected:

    GrMaterial* m_mat;
    CTexture * m_tex;
    
    GrPoint3D m_pos;
    GrVector3D m_velocity;
    GrVector3D m_acceleration;

    double m_sphereSize;
  
    void recurse(void (CObject::*f)() const) const 
    {
          (void) (this->*f)();         // Perform a function
     
          if (isParent())
            child()->recurse(f);

          if (!isLastSibling())
            next()->recurse(f);
          
    }


    void recurse(void (CObject::*f)()) 
    {
          (void) (this->*f)();         // Perform a function
     
          if (isParent())
            child()->recurse(f);

          if (!isLastSibling())
            next()->recurse(f);
          
    }

    
  public: 
    CObject() : m_mat(NULL), m_tex(NULL), m_sphereSize(0) {}
    ~CObject() {}


    virtual void draw() const {
      recurse(&CObject::onDraw);
    }
    
    virtual void update(unsigned int msecs=0) {
      (void) onUpdate(msecs);         // Perform a function
     
      if (isParent())
        child()->update(msecs);

      if (!isLastSibling())
        next()->update(msecs);
    }

    virtual void rayIntercection( 
        const CGrRay3D & ray, 
        CIResult* closest_res
    ) const {
      (void) onRayIntercection(ray, closest_res);         // Perform a function
     
      if (isParent())
        child()->rayIntercection(ray, closest_res);

      if (!isLastSibling())
        next()->rayIntercection(ray, closest_res);
    }

    


    CObject * parent() const {return (CObject *) m_parent;}
    CObject * child() const {return (CObject *) m_child;}
    CObject * next() const {return (CObject *) m_next;}
    CObject * prev() const {return (CObject *) m_prev;}

    const double & sphereSize() const {return m_sphereSize;}
    const GrPoint3D & pos() const {return m_pos;}
    const GrVector3D & velocity() const {return m_velocity;}
    const GrVector3D & acceleration() const {return m_acceleration;}

    // perform basic physics on the object
    virtual void onUpdate(unsigned int /*msecs=0*/) {}

    // draw the object given the camera position
    virtual void onDraw() const = 0; 

    // This function contains display list instructions
    // The parameter is used if there are more than 1 list
    virtual void displayListInstructions(unsigned int /*instSet*/) const {}

    // Performs ray intersection with an object
    virtual void onRayIntercection (
        const CGrRay3D & ray, 
        CIResult* closest_res
        ) const;

    virtual void onBulletHit(const GrPoint3D & pos) {}
    //virtual void closestIntersection(const CGrRay3D &, CIResult *) const {}

    // Returns the normal to the object at point pos
    virtual const GrVector3D norm(const GrPoint3D & /*pos*/) const {
      return GrVector3D(0,0,0);
    }

    // Derives internal state from m_pos, m_velocity...
    virtual void onKeepIntegrity() {}

    
    CObject & operator=(const CObject & obj) {
      
      m_pos = obj.m_pos;
      m_velocity = obj.m_velocity;
      m_acceleration = obj.m_acceleration;
      m_mat = obj.m_mat;
      m_tex = obj.m_tex;
      m_sphereSize = obj.m_sphereSize;
      
      return *this;
    }
      
    CObject(const CObject & obj) : 
      m_mat(obj.m_mat), 
      m_tex(obj.m_tex), 
      m_pos(obj.m_pos), 
      m_velocity(obj.m_velocity), 
      m_acceleration(obj.m_acceleration), 
      m_sphereSize(obj.m_sphereSize) {
    }

    virtual void texture(CTexture * tex) {
      m_tex = tex;
    }

    virtual const CTexture * texture() const {
      return m_tex;
    }

    virtual void material(GrMaterial * mat) {
      m_mat = mat;
    }

    virtual const GrMaterial * material() const {
      return m_mat;
    }


    friend ostream & operator << (ostream &os, const CObject & o);
};

inline ostream & operator << (ostream &os, const CObject & o) {
  os << "pos: " << o.m_pos << " vel: " << o.m_velocity << " accel: " 
    << o.m_acceleration << " sohereSize: " << o.m_sphereSize;
  
  return os;
}





#endif /*__OBJECT_H__*/

//
// @(#) $Log: object.h,v $
// @(#) Revision 1.10  2003/12/08 07:02:12  virzak
// @(#) Now has displayListInstructions member
// @(#)
// @(#) Revision 1.9  2003/12/05 17:05:54  virzak
// @(#) Moved CIResult here from polygon.h
// @(#) Added virtual rayIntercection for all the objects
// @(#) Added onBulletHit function
// @(#) Added texture and maretial manipulator functions
// @(#)
// @(#) Revision 1.8  2003/10/21 08:37:13  virzak
// @(#) Added CTexture to the object class
// @(#)
// @(#) Revision 1.7  2003/10/18 07:57:45  virzak
// @(#) Changed icc-warning 869 disabling to removing parameter's name
// @(#)
// @(#) Revision 1.6  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.5  2003/10/18 00:23:01  virzak
// @(#) Added virtual void onKeepIntegrity() {} to CObject to change
// @(#) internal state
// @(#)
// @(#) Revision 1.4  2003/10/09 00:19:27  virzak
// @(#) Made query methods const
// @(#)
// @(#) Revision 1.3  2003/09/21 06:09:22  virzak
// @(#) onDraw and onUpdate are not public functions.
// @(#) Added CC and OP= as well as ostream handling
// @(#)
// @(#) Revision 1.2  2003/09/18 03:54:39  virzak
// @(#) Added interface functions sphereSize, pos, velocity and acceleration
// @(#)
// @(#) Revision 1.1  2003/09/17 06:57:18  virzak
// @(#) Contains CObject class that was in node.h before
// @(#)
//

