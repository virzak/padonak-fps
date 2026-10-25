//****************************************************************************
// @(#) $Id: displist.h,v 1.3 2003/12/10 09:32:14 virzak Exp $ 
// 
//  Description: 
//      Contains display list class
//      
//
//****************************************************************************

#ifndef __DISPLIST_H__
#define __DISPLIST_H__


#include "app.h"
#include "object.h"
#include "container.h"
#include "render.h"


class CDisplayList : public CWindowDependent {

  protected:
    const CObject * m_obj;
    const unsigned int m_instructionSet;
    rnd::Mesh * m_mesh;
    

  public:
    
    CDisplayList(
        const CObject * obj, 
        unsigned int instSet=0, 
        bool callbacks = true) : 
      CWindowDependent(callbacks), m_obj(obj), m_instructionSet(instSet),
      m_mesh(NULL) {
    }
  
    virtual void callDisplayListInstructions() const {
      m_obj->displayListInstructions(m_instructionSet);
    }

    virtual void onCreateWindow() {

      m_mesh = rnd::beginMesh();
        callDisplayListInstructions();
      rnd::endMesh();

    }

    virtual void onDestroyWindow() {
      rnd::deleteMesh(m_mesh);
      m_mesh = NULL;
    }

    virtual void call() const {
      rnd::drawMesh(m_mesh);
    }
    
};

class COneDisplayList : public CDisplayList, virtual public CObject {
  public:
    COneDisplayList() : CDisplayList( NULL ) {}

    virtual void callDisplayListInstructions() const {
      displayListInstructions(m_instructionSet);
    }

};


class CDisplayListContainer : 
  public CWindowDependent,
  protected CContainer<CDisplayList>
{
  public:
    

    virtual void onCreateWindow() {
      for (deque<CDisplayList *>::iterator i = begin(); i < end(); ++i) {
        (*i)->onCreateWindow(); 
      }
    }

    virtual void onDestroyWindow() {
      for (deque<CDisplayList *>::iterator i = begin(); i < end(); ++i) {
        (*i)->onDestroyWindow(); 
      }
    }
    
    virtual void onDraw() const {
      call();
    }

    virtual void call() const {
      for (deque<CDisplayList *>::const_iterator i = begin(); i < end(); ++i) {
        (*i)->call();
      }      
    }
    

    virtual void addDisplayList(unsigned int instSet=0) {
      CDisplayList * displayList = 
        new CDisplayList((const CObject *) this, instSet, false);      
      push_back(displayList);
      
    }
  
};



#endif



//
// @(#) $Log: displist.h,v $
// @(#) Revision 1.3  2003/12/10 09:32:14  virzak
// @(#) Got rid of g++ warning
// @(#)
// @(#) Revision 1.2  2003/12/08 11:05:44  virzak
// @(#) Added COneDisplayList class that is extended by classes that have
// @(#)     1 display list.
// @(#) Removed the registration
// @(#)
// @(#) Revision 1.1  2003/12/08 07:07:11  virzak
// @(#) New file: contains siaplay list info
// @(#)
//

