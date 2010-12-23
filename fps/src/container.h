//*****************************************************************************
// @(#) $Id: container.h,v 1.8 2003/12/08 11:02:47 virzak Exp $ 
// 
//  Description: 
//      Container class provides a deque based container that contains 
//      pointers to instances of a desired class
//
//  =
//
//*****************************************************************************

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "node.h"
#include <deque>

template<class T>
class CContainer : /*virtual public CObject,*/ public deque<T *> {
    // Members
    // deque<T *> m_elems;
    
  public:

    // Methods
    
    // Constructor
    CContainer() {}

    // Destructor
    ~CContainer() {
      while (!empty()) {
        delete front();
        pop_front();
      }
    }
  
    // Assignment operator
    CContainer & operator=(const CContainer & c) {

      // Copy all the elements one by one
      for (typename deque<T *>::const_iterator i = c.begin(); 
          i != c.end(); ++i ) {
        T *elem = new T(**i);
        push_back(elem);
      } // for  
      return *this;
    }
  
    // Copy constructor
    CContainer(const CContainer & c) {
      *this = c;
    }
};


template<class T>
class CObjectContainer : virtual public CObject, public CContainer<T> {

  public:

    // Methods
    
    // Draws the container
    virtual void onDraw() const {
      // traverse deque nodes
      for (typename deque<T *>::const_iterator i = begin(); 
          i < end(); ++i)
      {
        (*i)->onDraw(); 
      }  
    }
    
    virtual void onUpdate(unsigned int msecs=0) {
      // traverse deque nodes
      for (typename deque<T *>::iterator i = begin(); 
          i < end(); ++i)
      {
        (*i)->onUpdate(msecs); 
      }  
    }

};


#endif /*__CONTAINER_H__*/


//
// @(#) $Log: container.h,v $
// @(#) Revision 1.8  2003/12/08 11:02:47  virzak
// @(#) There are two containers now CContainer and CContainer.
// @(#) One is an object and contains objects, the other is not
// @(#)
// @(#) Revision 1.7  2003/12/08 06:59:32  virzak
// @(#) Container now extends deque
// @(#)
// @(#) Revision 1.6  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.5  2003/09/30 06:49:14  virzak
// @(#) Make CObject virtual base class
// @(#)
// @(#) Revision 1.4  2003/09/21 06:05:36  virzak
// @(#) changed onDraw to virtual
// @(#) added onUpdate
// @(#)
// @(#) Revision 1.3  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.2  2003/09/04 01:12:18  virzak
// @(#) Added typename to deque iterator... apparently required by new g++-v3
// @(#)
// @(#) Revision 1.1  2003/08/24 20:08:57  virzak
// @(#) Initial version of CContainer class
// @(#)
//

