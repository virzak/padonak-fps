//*****************************************************************************
// @(#) $Id: app.h,v 1.5 2003/12/08 11:01:31 virzak Exp $ 
// 
//  Description: 
//      Provides Application class and implements module events
//
//  =
//
//*****************************************************************************

#ifndef __APP_H__
#define __APP_H__


#include <stdlib.h>
#include <deque>

using namespace::std;




class CWindowDependent {
  protected:

    // Members
    const bool m_register_callbacks;

  //private:
    // Methods
    virtual void onCreateWindow() = 0;
    virtual void onDestroyWindow() = 0;

  protected:
    CWindowDependent(bool callbacks=true);
    CWindowDependent(const CWindowDependent & wd);
    //CWindowDependent & operator=(const CWindowDependent & wd);
    virtual ~CWindowDependent();
    
  friend class CCommandQueue;
  friend class CApp;
};



class CApp;


class CCommandQueue {
    typedef void (CWindowDependent::*FModule)();

    deque<CWindowDependent *> m_queue;
    FModule m_f;
    
  public:
    void execute() {
      for (deque<CWindowDependent *>::const_iterator i = m_queue.begin(); 
          i != m_queue.end(); ++i) {
        CWindowDependent * wdep = *i;
        (wdep->*(this->m_f))();
      }  
    }
    
    CCommandQueue(FModule f) : m_f(f) {}
  
    CCommandQueue& operator+=(CWindowDependent * mod) {
      m_queue.push_back(mod);
      return *this;        
    }
      

    CCommandQueue& operator-=(CWindowDependent * mod) {
      for (deque<CWindowDependent *>::iterator i = m_queue.begin(); 
          i != m_queue.end(); ++i) {
        if (mod == *i) {
          m_queue.erase(i);
          break;
        }
      }
      return *this;    
    }
    friend class CApp;
};



class CApp {
    CCommandQueue *m_onCreateWindow;
    CCommandQueue *m_onDestroyWindow;

    // Singleton Variable
    static CApp * m_instance;
  
    CApp() : m_done(false) {
      m_onCreateWindow = new CCommandQueue (&CWindowDependent::onCreateWindow);
      m_onDestroyWindow = new CCommandQueue (&CWindowDependent::onDestroyWindow);
    }
    

    void created() {
      m_onCreateWindow->execute(); 
    }

    void destroyed() {
      m_onDestroyWindow->execute(); 
    }


    CCommandQueue & onCreateWindow() {return *m_onCreateWindow;}
    CCommandQueue & onDestroyWindow() {return *m_onDestroyWindow;}
    
  public:
    
    static CApp * instanceOf() {
      if (!m_instance) {
        m_instance = new CApp();
      }    
      return m_instance;
    }

    ~CApp() {
      delete m_onCreateWindow;
      delete m_onDestroyWindow;
      m_instance = NULL;
    }


    bool m_done;
    
    friend class CWindowDependent;
    friend class CWindow;
};

inline CWindowDependent::CWindowDependent(bool callbacks) 
        : m_register_callbacks(callbacks) {
  if (m_register_callbacks) {
    CApp::instanceOf()->onCreateWindow()+=this;
    CApp::instanceOf()->onDestroyWindow()+=this;
  }
}

inline CWindowDependent::CWindowDependent(const CWindowDependent & wd) 
        : m_register_callbacks(wd.m_register_callbacks) {
  if (m_register_callbacks) {
    CApp::instanceOf()->onCreateWindow()+=this;
    CApp::instanceOf()->onDestroyWindow()+=this;
  }
}


/*
inline CWindowDependent & 
    CWindowDependent::operator=(const CWindowDependent & wd) 
{
  if (m_register_callbacks != wd.m_register_callbacks) {
    
    m_register_callbacks = wd.m_register_callbacks;

    if (m_register_callbacks) {
      CApp::instanceOf()->onCreateWindow()+=this;
      CApp::instanceOf()->onDestroyWindow()+=this;
    } else {
      CApp::instanceOf()->onCreateWindow()-=this;
      CApp::instanceOf()->onDestroyWindow()-=this;
    }
  }
  return *this;
}
*/

inline CWindowDependent::~CWindowDependent() {
  if (m_register_callbacks) {
    CApp::instanceOf()->onCreateWindow()-=this;
    CApp::instanceOf()->onDestroyWindow()-=this;
  }
}


#endif /*__APP_H__*/



//
// @(#) $Log: app.h,v $
// @(#) Revision 1.5  2003/12/08 11:01:31  virzak
// @(#) CWindowDependent is now responsible for registering a callback
// @(#)
// @(#) Revision 1.4  2003/12/05 16:55:25  virzak
// @(#) changed from CModule to CWindowDependent
// @(#)
// @(#) Revision 1.3  2003/10/05 23:25:34  virzak
// @(#) CModule is now ABC.
// @(#)
// @(#) Revision 1.2  2003/08/29 08:08:19  virzak
// @(#) Added m_done flag which is used to quit the app
// @(#)
// @(#) Revision 1.1  2003/08/26 18:57:30  virzak
// @(#) First revision
// @(#)
//

