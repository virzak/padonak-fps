//****************************************************************************
// @(#) $Id: engine.cpp,v 1.54 2003/12/05 16:56:38 virzak Exp $ 
// 
//  Description: 
//      Contains the initialization of the application routine and storage
//      for global variables
//
//  =
//
//*****************************************************************************


#include "engine.h"
#include "gr.h"
#include "window.h"
#include "ui.h"
#include "camera.h"
#include "textures.h"
#include <string>



static char *msg="Victor's FPS, $Revision: 1.54 $, $Date: 2003/12/05 16:56:38 $ GMT\n";


const char *worldfile = "data/world.txt";   /* datafile for our 3D world */


bool mirror = true;
bool mirror_stencil = false;
bool draw_myself = true;

bool output_toggle = false;


CTextureContainer *tex_container;
CMaterialContainer *mat_container;

GLuint filter;  /* which filter to use */



const float piover180 = 0.0174532925f;


COutput * output;


CGraphics *gr;
CUI *ui;
CPlayer *cp;
CWorld *wld;
CWindow *window;




unsigned int fps;



// This function is called on a user command
static void output_function(const string & s) {
  if (s.length()) {
    ui->parseUserCommand(s);
  }
}




/* Measures Framse per Second rate */
static void measureFPS() {

  static long int last_draw_time = 0;
  static unsigned int cur_fps = 0;


  cur_fps++;
  long int cur_time;
  if ((cur_time = (timer.getTime() % 1000)) < last_draw_time) {
    fps = cur_fps;
    cur_fps=0;
  }
  last_draw_time = cur_time;
  
}



static CApp * app;

/* Initial function. Gets called from main or _tWinMain */

void init_app () {

  /* Initializations */

  app = CApp::instanceOf();
  
  
  output = COutput::instanceOf();
  *output << setprecision(1) << fixed;

  // Initialize containers
  tex_container = new CTextureContainer;
  mat_container = new CMaterialContainer;
  
  wld = CWorld::instanceOf(worldfile);
  gr = CGraphics::instanceOf();
  gr->setRoot(wld);
  ui = CUI::instanceOf();
  output->setOutputFunction(output_function);
   
  output->printf(msg);


  /* default to fullscreen */ 
  if (!(window = CWindow::instanceOf("Victor's FPS", 640, 480, 16, false, ui)))
  {
    app->m_done = true;
  }
  
  // Should this be here?
  ui->initKeys();

  timer.pause();


  /* Event loop */ 
  while (!app->m_done)
  {
    /*Process outstanding events*/ 
    window->processEvents();
    
    // Handle user input
    ui->keyButtonAction();

    // Update timer
    if (!timer.isPaused()) timer.update();
    
    // Update world
    if (!timer.isPaused()) {
      unsigned int left = timer.getChange();
      
      while (left > 40) {
        wld->update(40);
        left -= 40;
      }
      wld->update(left);
    }

    // Draw the scene
    gr->drawGLScene(); 
    window->swapFunction();
    measureFPS();

  }
  
  output->setOutputFunction(NULL);

  delete window;
  delete ui;
  delete gr;
  delete wld;
  delete tex_container;
  delete mat_container;
  delete output;


  delete app;
}




#include <signal.h>
#include <SDL3/SDL_main.h>   /* provides WinMain on Windows */

static void gc_signal_handler(int signal_no)
{
    signal(SIGINT, gc_signal_handler);  /* Make sure we catch another CTRL-C. */
    app->m_done = true;
    signal(SIGINT, SIG_DFL);            /* Restore the interrupt handler. */
    //raise(signal_no);                   /* Wake up default handler. */
}


int main(int argc, char **argv)
{
  signal(SIGINT, gc_signal_handler);
  signal(SIGABRT, gc_signal_handler);
  
  init_app();
  SDL_Quit();
  return 0;
}

 

//
// @(#) $Log: engine.cpp,v $
// @(#) Revision 1.54  2003/12/05 16:56:38  virzak
// @(#) added material container
// @(#)
// @(#) Revision 1.53  2003/11/15 06:06:57  virzak
// @(#) Safe Ctrl-C
// @(#)
// @(#) Revision 1.52  2003/10/18 11:30:55  virzak
// @(#) More fixes for slower cards
// @(#)
// @(#) Revision 1.51  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.50  2003/10/04 22:25:16  virzak
// @(#) unpause the timer
// @(#)
// @(#) Revision 1.49  2003/09/30 23:13:58  virzak
// @(#) Added return 0; at entry function (_tWinMain)
// @(#)
// @(#) Revision 1.48  2003/09/30 22:43:20  virzak
// @(#) Handling user input first and then updating the timer
// @(#)
// @(#) Revision 1.47  2003/09/30 07:52:25  virzak
// @(#) Added static to void output_function
// @(#)
// @(#) Revision 1.46  2003/09/24 06:48:08  virzak
// @(#) Added input command handling functions
// @(#)
// @(#) Revision 1.45  2003/09/22 20:23:02  virzak
// @(#) When paused don't update the timer or the tree
// @(#)
// @(#) Revision 1.44  2003/09/04 01:15:41  virzak
// @(#) Removed the self test (testing is in the test program)
// @(#) destroyed_by_user was moved to UI
// @(#)
// @(#) Revision 1.43  2003/08/29 08:12:13  virzak
// @(#) Added measureFPS here.
// @(#) Removed many global variables and put them in apropriate classes
// @(#) Added texture container that contains all textures
// @(#)
// @(#) Revision 1.42  2003/08/28 09:17:18  virzak
// @(#) Added enum for required texture and an array of pointers.
// @(#) Removed many unnecessary global variables
// @(#)
// @(#) Revision 1.41  2003/08/27 22:10:47  virzak
// @(#) Camera instantiation is now handles by world class
// @(#)
// @(#) Revision 1.40  2003/08/26 18:54:51  virzak
// @(#) Added App instance
// @(#)
// @(#) Revision 1.39  2003/08/26 08:41:24  virzak
// @(#) Moved windows _tWinMain from window to engine
// @(#)
// @(#) Revision 1.38  2003/08/26 08:14:31  virzak
// @(#) Moved main from window.cpp to engine.cpp
// @(#)
// @(#) Revision 1.37  2003/08/26 05:34:14  virzak
// @(#) gui.h >> window.h
// @(#)
// @(#) Revision 1.36  2003/08/26 04:09:01  virzak
// @(#) Changes resulting from movement of windowing functions into CWindow class
// @(#)
// @(#) Revision 1.35  2003/08/14 01:13:21  virzak
// @(#) Added output selfTest in the beginning.
// @(#)
// @(#) Revision 1.34  2003/08/09 01:05:00  virzak
// @(#) Added stencil mirror.
// @(#)
// @(#) Revision 1.33  2003/08/05 18:18:25  virzak
// @(#) Replaced quad by s_quad, since it is a reserved vaeiable or a keyword
// @(#) on some compilers
// @(#)
// @(#) Revision 1.32  2003/08/05 08:08:15  virzak
// @(#) Changed update call without argument (as now needed)
// @(#)
// @(#) Revision 1.31  2003/08/03 21:04:52  virzak
// @(#) Added texture
// @(#)
// @(#) Revision 1.30  2003/08/02 20:13:04  virzak
// @(#) Added CCamera instance
// @(#)
// @(#) Revision 1.29  2003/07/30 08:13:27  virzak
// @(#) Got rid of players array, will be stored in DAG
// @(#)
// @(#) Revision 1.28  2003/07/28 03:46:57  virzak
// @(#) Added update and onupdate method in object ans overrode it in
// @(#) player. Update calculates position, velocity, accel for an object
// @(#) every iteration
// @(#)
// @(#) Revision 1.27  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.26  2003/07/10 18:38:29  virzak
// @(#) fixed comments
// @(#)
// @(#) Revision 1.25  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.24  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.23  2003/07/03 22:16:47  virzak
// @(#) Moved speeds to player.h/cpp
// @(#)
// @(#) Revision 1.22  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
// @(#) Revision 1.21  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
// @(#) Revision 1.20  2003/07/02 19:27:51  virzak
// @(#) Removed a line from log that contained a comment, since the compiler was throwing errors about having nested comments
// @(#)
// @(#) Revision 1.18  2003/07/02 19:12:40  virzak
// @(#) added description
// @(#)
// @(#) Revision 1.17  2003/07/02 18:25:02  virzak
// @(#) Eliminated the problem of the full screen in windows
// @(#)
// @(#) Revision 1.16  2003/07/02 16:17:58  virzak
// @(#) added GMT to the time, so that there is no confusion
// @(#)
// @(#) Revision 1.15  2003/07/02 16:14:22  virzak
// @(#) who cares?
// @(#)
// @(#) Revision 1.14  2003/07/02 16:13:20  virzak
// @(#) I hope it will not be expanded
// @(#)
// @(#) Revision 1.13  2003/07/02 16:12:23  virzak
// @(#) moved the cvs $Id: engine.cpp,v 1.54 2003/12/05 16:56:38 virzak Exp $ statement to the top and commented it out
// @(#)
// @(#) Revision 1.12  2003/07/02 16:04:12  virzak
// @(#) Added cvs info into the source and the output
// @(#)
//
