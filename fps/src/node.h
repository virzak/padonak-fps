//*****************************************************************************
// @(#) $Id: node.h,v 1.18 2003/09/17 06:56:40 virzak Exp $ 
// 
//  Description: 
//      Provides tree node structure and functions
//
//  =
//
//*****************************************************************************



#ifndef __NODE_H__
#define __NODE_H__

#define __THIS_NODE_H_IS_REAL__ 1

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <GL/gl.h>
#include "util.h"


class CObject;


/* Scene graph node.
 * Note that a parent only points to its first child, the rest are 
 * found by sibling pointers.   To represent non-tree links (links to
 * shared nodes) an INSTANCE node type should be used.
 */
class CGrDagNode {
  private:
    CGrDagNode* m_parent;
    CGrDagNode* m_next;
    CGrDagNode* m_prev;
    CGrDagNode* m_child;
    bool m_firstSibling;
    bool m_lastSibling;
  public:

    unsigned int id;
    char* name;

    bool isChild() const {return (m_parent!= NULL);}
    bool isParent() const {return (m_child != NULL);}
    bool isFirstSibling() const {return m_firstSibling;}
    bool isLastSibling() const {return m_lastSibling;}
    bool hasSibling() const {return m_next!=this;}

    bool isFirstChild() const {
      return (isChild() && isFirstSibling());
    }
    bool isLastChild() const {
      return (isChild() && isLastSibling());
    }



    
    void attachTo(CGrDagNode * new_parent) {

      if (isChild()) detach();

      m_parent = new_parent;
      
      if (m_parent->m_child) {
      
        m_prev = m_parent->m_child->m_prev;
        m_prev->m_next = this;
      
        m_next = m_parent->m_child;
        m_next->m_prev = this;

        m_prev->m_lastSibling = false;
        m_firstSibling = false;
      
      } else {
        m_parent->m_child = this;
      }

    }
    
    void attach(CGrDagNode * new_child) {new_child->attachTo(this);}

    void detach() {

      if (isFirstChild())
        m_parent->m_child = hasSibling() ? m_next : NULL;

      m_prev->m_next = m_next;
      m_next->m_prev = m_prev;

      if (m_lastSibling) 
        m_prev->m_lastSibling=true;
      
      if (m_firstSibling) 
        m_next->m_firstSibling=true;
      

      m_next = m_prev = this;
      m_parent = NULL;
      m_firstSibling = m_lastSibling = true;
    }
    
    // Constructors
    CGrDagNode() {
      m_parent = m_child = NULL;
      m_next = m_prev = this;
      m_firstSibling = m_lastSibling = true;
    }
    
    CGrDagNode(CGrDagNode * node) {
      m_parent = m_child = NULL;
      m_next = m_prev = this;
      m_firstSibling = m_lastSibling = true;
      attachTo(node);
    }
    

    // Destructor
    virtual ~CGrDagNode() {
      detach();

      while (m_child) {
        delete m_child;
      }
    }

    friend class CObject;
};




#endif



//
// @(#) $Log: node.h,v $
// @(#) Revision 1.18  2003/09/17 06:56:40  virzak
// @(#) Removed all CObject stuff and put it into object.h
// @(#)
// @(#) Revision 1.17  2003/09/17 06:34:08  virzak
// @(#) Set up defaults for mat and m_sphereSize
// @(#)
// @(#) Revision 1.16  2003/09/10 07:21:39  virzak
// @(#) Another bug
// @(#)
// @(#) Revision 1.15  2003/09/10 06:45:41  virzak
// @(#) Fixed a horrible bug
// @(#)
// @(#) Revision 1.14  2003/09/10 03:40:04  virzak
// @(#) Removed all unnecessary inline keywords.
// @(#) Added m_firstSibling and m_lastSibling members to speed up traversals
// @(#) End nodes now point to the opposite ends instead of NULL. This was done
// @(#) to speed up addition (Don't have to go to the end).
// @(#)
// @(#) Revision 1.13  2003/09/09 23:40:03  virzak
// @(#) Made onDraw be a constant function.
// @(#) Now there is also a const_recurse function. It calls a constant function
// @(#) recursively
// @(#)
// @(#) Revision 1.12  2003/09/09 22:42:20  virzak
// @(#) Added double m_sphereSize; to every object
// @(#)
// @(#) Revision 1.11  2003/09/06 03:52:04  virzak
// @(#) Put position, velocity and acceletation as protected variables
// @(#)
// @(#) Revision 1.10  2003/08/28 09:11:52  virzak
// @(#) Fixed linked link bug
// @(#)
// @(#) Revision 1.9  2003/08/27 22:01:46  virzak
// @(#) Changed attach() to attach the node at the end
// @(#)
// @(#) Revision 1.8  2003/08/20 02:39:30  virzak
// @(#) Removed timer and emptied update, to make it compatible with other
// @(#) programs that are not aware of timer
// @(#) Timing issues moved to kobject.h
// @(#)
// @(#) Revision 1.7  2003/08/05 08:03:09  virzak
// @(#) Added recurse function that takes a function variable argument and
// @(#) calls it recursively
// @(#)
// @(#) Revision 1.6  2003/07/30 05:54:16  virzak
// @(#) Made draw() virtual. It should be changed for class like mirror
// @(#)
// @(#) Revision 1.5  2003/07/30 04:36:58  virzak
// @(#) Fixed windows bug
// @(#)
// @(#) Revision 1.4  2003/07/30 04:28:53  virzak
// @(#) more node changes
// @(#)
// @(#) Revision 1.3  2003/07/28 03:46:57  virzak
// @(#) Added update and onupdate method in object ans overrode it in
// @(#) player. Update calculates position, velocity, accel for an object
// @(#) every iteration
// @(#)
// @(#) Revision 1.2  2003/07/27 03:02:22  virzak
// @(#) First step to nodes. Player class extends node. Still no hierarchy
// @(#)
// @(#) Revision 1.1  2003/07/20 00:42:43  virzak
// @(#) Initial structure
// @(#)
//



