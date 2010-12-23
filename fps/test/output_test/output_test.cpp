//*****************************************************************************
// @(#) $Id: output_test.cpp,v 1.11 2003/09/23 01:00:24 virzak Exp $
//
//  Description:
//        Tests the output class
//
//  =
//
//*****************************************************************************


#include "output.h"
#include "window.h"
#include "gr.h"




// Testing routine

void COutput::selfTest() {
  
  if (!m_instance) {
    cout << "Cannot test output, initialize first.\007" << endl;
    return;
  }

  string s = "Ebat' vas";
  *m_instance << "Testing int 9:  " << (int) 9 << "\n";
  *m_instance << "Testing hex 100 = :  " << hex << "0x" << (int) 100 << "\n";
  *m_instance << "Testing double 9.957385827364:  " << (double) 9.957385827364 << "\n";
  *m_instance << "Testing double 9.957385827364 pre(5):  " << setprecision(5) << (double) 9.957385827364 << "\n";
  *m_instance << "Testing char 'f':  " << (char) 'f' << "\n";
  *m_instance << "Testing const char * 'Zarazite\\nMenya\\nSpidom\\n':  " << (const char *) "Zarazite\nMenya\nSpidom\n";
  *m_instance << "Testing string  'Ebat' vas':  " << s << endl;
}





static COutput * output;
static CApp * app;

class CSimpleUI : public CCallbacks {
  
    virtual void sendChar(const unsigned char c) {
      *output << c;
      if (c == 'q' || c == 'Q') app->m_done = true;
    }
  public:
    CSimpleUI() {m_send_letter = true;};

};

class CSimpleObject : public CObject {
  virtual void onDraw() const {
    CWindow *window = CWindow::instanceOf(); 
      
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor4f(1, 1, 1, 0);
    glRecti(0, window->gl_win.height, window->gl_win.width, window->gl_win.height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, window->gl_win.width, window->gl_win.height);
    glOrtho(0.0, (double)window->gl_win.width, 0.0, (double)window->gl_win.height, -0.1, 0.1);
  
    // change to model view for drawing
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  
      glColor4f(0, 0, 0, 0.5);
      glRecti(0, window->gl_win.height, window->gl_win.width, window->gl_win.height/2);
    
      glColor3f(1, 0, 1);
      output->draw(10, window->gl_win.height - 20, 20);
  }
};


static CSimpleObject obj;
static CSimpleUI ui;

static void init_app() {
  app = CApp::instanceOf();
  CWindow *window;
  output = COutput::instanceOf();
  output -> selfTest();
  window = CWindow::instanceOf("Output Test", 640, 480, 16, false, &ui);


  /* Event loop */ 
  while (!app->m_done)
  {
    /*Process outstanding events*/ 
    window->processEvents();
    
    
    
    obj.update();
    obj.draw();
     
    window->swapFunction();
  }


  delete window;
  delete output;
  delete app;
}


int main(
#ifdef linux
		 int argc, char **argv
#endif
#ifdef WIN32
		 HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow
#endif
		 )
{
  init_app();
  return 0;
}



//
// @(#) $Log: output_test.cpp,v $
// @(#) Revision 1.11  2003/09/23 01:00:24  virzak
// @(#) Got rid of global variables
// @(#)
// @(#) Revision 1.10  2003/09/10 03:35:34  virzak
// @(#) Moved COutput::selfTest here from output.cpp
// @(#) static COutput * output is now static
// @(#)
// @(#) Revision 1.9  2003/09/09 23:40:58  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.8  2003/09/04 03:05:12  virzak
// @(#) Rewrote using sendChar
// @(#)
// @(#) Revision 1.7  2003/09/04 02:42:51  virzak
// @(#) Cleaned up a little
// @(#)
// @(#) Revision 1.6  2003/09/02 23:11:41  virzak
// @(#) Expanded the test case
// @(#)
// @(#) Revision 1.5  2003/09/02 20:45:59  virzak
// @(#) changes
// @(#)
// @(#) Revision 1.4  2003/08/30 07:38:24  virzak
// @(#) Changed to be a console application under WIN32
// @(#)
//

