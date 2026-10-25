//*****************************************************************************
// @(#) $Id: window.h,v 1.12 2003/10/18 07:57:45 virzak Exp $
//
//  Description:
//      Contains the CWindow class
//
//  =
//
//*****************************************************************************



#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdio.h>
#include <SDL3/SDL.h>
#include "app.h"


typedef struct {
  bool fs;
  unsigned int width;   /* drawable size in pixels */
  unsigned int height;
} GLWindow;




class CCallbacks {
  protected:
    bool m_destroyed_by_user;
    bool m_send_letter;
  public:
    bool isDestroyedByUser() {return m_destroyed_by_user;}
    bool isSendingLetter() {return m_send_letter;}
    /* relative mouse movement in pixels; the pointer is captured */
    virtual void motion(int /*dx*/, int /*dy*/) {}
    virtual void buttonPress(unsigned int /*b*/) {}
    virtual void buttonRelease(unsigned int /*b*/) {}
    /* k is an SDL_Scancode */
    virtual void keyPress(unsigned int /*k*/) {}
    virtual void sendChar(const unsigned char /*c*/) {}
    virtual void keyRelease(unsigned int /*k*/) {}
    CCallbacks() : m_destroyed_by_user(true), m_send_letter(false) {}
    virtual ~CCallbacks() {}
};




class CWindow {
    static bool m_success;
    bool createGLWindow (const char* title, int width, int height,
        bool fullscreenflag);
    void killGLWindow();

    CApp * m_app;

    bool m_default_callbacks;

    static CWindow * m_instance;
    CWindow(const char* title, int width, int height, int /*bits*/,
        bool fullscreenflag, CCallbacks * callbacks)
        : m_window(NULL), m_context(NULL), m_created(false) {
      m_app = CApp::instanceOf();
      gl_win.fs=fullscreenflag;
      gl_win.width = width;
      gl_win.height = height;

      if (callbacks) {
        m_callbacks = callbacks;
        m_default_callbacks = false;
      } else {
        m_callbacks = new CCallbacks();
        m_default_callbacks = true;
      }

      m_success = createGLWindow(title, width, height, fullscreenflag);
    }


    CCallbacks * m_callbacks;

    SDL_Window * m_window;
    SDL_GLContext m_context;
    bool m_created;   /* the window dependents have been told */

  public:
    GLWindow gl_win;

    ~CWindow() {
      if (m_default_callbacks) {
        delete m_callbacks;
      }

      killGLWindow();
      m_instance = NULL;
    }

    /* bits is ignored; SDL picks the desktop colour depth */
    static CWindow * instanceOf(const char* title, int width, int height,
        int bits, bool fullscreenflag, CCallbacks * callbacks) {
      if (!m_instance) {
        m_instance = new CWindow(title, width, height, bits, fullscreenflag,
            callbacks);
        if (!m_success) m_instance = NULL;
      }
      return m_instance;
    }

    static CWindow * instanceOf() {
      return m_instance;
    }


    void swapFunction();
    void processEvents();
};

#endif /*__WINDOW_H__*/


//
// @(#) $Log: window.h,v $
// @(#) Revision 1.12  2003/10/18 07:57:45  virzak
// @(#) Changed icc-warning 869 disabling to removing parameter's name
// @(#)
// @(#) Revision 1.11  2003/09/04 02:03:11  virzak
// @(#) Changed keyPress and added sendChar
// @(#) Hid CCallbacks members and provided access functions
// @(#)
// @(#) Revision 1.10  2003/08/29 09:54:39  virzak
// @(#) Added CWindow::proc that uses callbacks in CWindow class
// @(#)
// @(#) Revision 1.9  2003/08/29 08:31:15  virzak
// @(#) Added class CCallbacks.
// @(#) Changed instanceOf and constructor in order to get CCallbacks *
// @(#)
// @(#) Revision 1.8  2003/08/28 01:58:06  virzak
// @(#) Fixed windows based bugs
// @(#)
// @(#) Revision 1.7  2003/08/27 21:53:11  virzak
// @(#) Added XGLWindow for linux and GLWindow. Added GLuint m_base member for
// @(#) printing on the screen
// @(#) Added makeRasterFont() to the class
// @(#)
// @(#) Revision 1.6  2003/08/26 05:30:45  virzak
// @(#) Rewrote the Macros
// @(#) Corrected the horrible m_instance bug.
// @(#)
// @(#) Revision 1.5  2003/08/26 05:06:31  virzak
// @(#) Added comments
// @(#)
//


