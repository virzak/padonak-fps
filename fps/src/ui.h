//*****************************************************************************
// @(#) $Id: ui.h,v 1.23 2003/12/08 11:07:25 virzak Exp $ 
// 
//  Description: 
//      This file provides to user I/O changes by calling appropriate methods
//
//  =
//
//*****************************************************************************


#ifndef __UI_H__
#define __UI_H__

#include <stdlib.h>
#include "app.h"
#include "window.h"
#include "output.h"

class CUI : public CWindowDependent, public CCallbacks {
    
    // Singleston variable
    static CUI * m_instance;
    
    
    virtual void onCreateWindow() {
      reset();
    }
    virtual void onDestroyWindow() {}
    
    bool m_centering;
    CUI() : m_centering(true) {}

    bool m_keys[256];
    bool m_buttons[10];
    unsigned int  m_keyCodes[40];   /* array to hold our fetched keycodes */
     
  public:
    
    static CUI * instanceOf() {
      if (!m_instance) {
        m_instance = new CUI();
      }
      return m_instance;
    }

    ~CUI() {
      m_instance = NULL;
    }


    
    void reset();
    inline void center() {m_centering = true;}
    void keyButtonAction();
    void buttonAction();
    void keyAction();
    void motionAction(unsigned int x, unsigned int y);
    void initKeys();
    
    virtual void parseUserCommand(const string &);
    virtual void parseUserSource(istream & in);
    
    virtual void enter() {
      center();
    };
    
    virtual void motion(unsigned int x, unsigned int y) {
      motionAction(x,y);
    }
    
    virtual void rollUp();
    virtual void rollDown();
      
    
    virtual void buttonPress(unsigned int b) {
      if (b==4) {
        rollUp();
      } else  if (b==5) {
        rollDown();
      }
      m_buttons[b] = true;
    }
    
    virtual void buttonRelease(unsigned int b) {
      m_buttons[b] = false;
    }
    
    virtual void keyPress(unsigned int k) {
      m_keys[k] = true;
    }
    
    virtual void sendChar(const unsigned char c) {
      *(COutput::instanceOf()) << c;
    }
    
    
    virtual void keyRelease(unsigned int k) {
      m_keys[k] = false;
    }

};


#endif /*__UI_H__*/


//
// @(#) $Log: ui.h,v $
// @(#) Revision 1.23  2003/12/08 11:07:25  virzak
// @(#) Removed callback registration
// @(#)
// @(#) Revision 1.22  2003/12/05 16:58:42  virzak
// @(#) Changed from CModule to CWindowDependent
// @(#)
// @(#) Revision 1.21  2003/10/05 23:34:15  virzak
// @(#) Small changes
// @(#)
// @(#) Revision 1.20  2003/10/04 23:04:12  virzak
// @(#) Added mouse wheel functions
// @(#) Added input parsing
// @(#)
// @(#) Revision 1.19  2003/09/24 06:49:32  virzak
// @(#) Added user command parsing and executing
// @(#)
// @(#) Revision 1.18  2003/09/23 01:02:51  virzak
// @(#) changed m_keyCodes to unsigned int []
// @(#)
// @(#) Revision 1.17  2003/09/12 03:30:45  virzak
// @(#) removed unnecessary output
// @(#)
// @(#) Revision 1.16  2003/09/04 01:38:23  virzak
// @(#) Changed keyPress and added sendChar
// @(#)
// @(#) Revision 1.15  2003/09/02 20:45:59  virzak
// @(#) changes
// @(#)
// @(#) Revision 1.14  2003/08/29 08:24:05  virzak
// @(#) Class CUI now also extends CCallbacks and overloads functions
// @(#)
// @(#) Revision 1.13  2003/08/27 21:59:48  virzak
// @(#) Added initKeys function
// @(#)
// @(#) Revision 1.12  2003/08/26 18:58:44  virzak
// @(#) Added event filing on creation of window
// @(#)
// @(#) Revision 1.11  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.10  2003/08/03 21:10:41  virzak
// @(#) Renamed go to m_centering with opposite effects
// @(#)
// @(#) Revision 1.9  2003/08/02 20:22:54  virzak
// @(#) Increased the m_keyCodes array from 20 to 40
// @(#)
// @(#) Revision 1.8  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.7  2003/07/03 22:22:45  virzak
// @(#) Added capabilities for acceleration, velocity.
// @(#) Tied the positioning to them in order to have more realistic movement
// @(#)
//



