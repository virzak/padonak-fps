//*****************************************************************************
// @(#) $Id: gr.h,v 1.15 2003/12/08 11:07:25 virzak Exp $ 
// 
//  Description: 
//      Responsible for OpenGL initializations
//
//  =
//
//*****************************************************************************


#ifndef __GR_H__
#define __GR_H__

#include <stdlib.h>
#include "object.h"
#include "app.h"


class CGraphics : public CWindowDependent {
  
    // Singleston variable
    static CGraphics * m_instance;
    

    double m_fov;
    void measureFPS();
    
    CGraphics() : m_fov(60.0f), m_root(NULL), m_light(false) {}

    void onCreateWindow() {initGL();}
    void onDestroyWindow() {deinitGL();}
    
    CObject * m_root;
    
  public:

    void setRoot(CObject * root) {
      m_root = root;
    }
    
    ~CGraphics() {
      m_instance = NULL;
    }
    double getFOV() {
      return m_fov;
    }

    void drawGLScene();
    
    static CGraphics * instanceOf() {
      if (!m_instance) {
        m_instance = new CGraphics();
      }
      return m_instance;
    }
    
    bool m_light;
    
    
    void initGL();
    void deinitGL();
    void resizeGLScene(unsigned int width, unsigned int height);

    friend class CUI;
};

  
#endif


//
// @(#) $Log: gr.h,v $
// @(#) Revision 1.15  2003/12/08 11:07:25  virzak
// @(#) Removed callback registration
// @(#)
// @(#) Revision 1.14  2003/12/05 16:58:42  virzak
// @(#) Changed from CModule to CWindowDependent
// @(#)
// @(#) Revision 1.13  2003/09/17 06:58:40  virzak
// @(#) changed an include from node.h to object.h
// @(#)
//

