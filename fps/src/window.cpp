//*****************************************************************************
// @(#) $Id: window.cpp,v 1.41 2003/10/05 02:02:43 virzak Exp $
//
//  Description:
//      Implements the window, OpenGL context and input on top of SDL3
//
//  =
//
//*****************************************************************************



#include "window.h"
#include "gr.h"
#include "output.h"
#include "render.h"
#include "resource.h"
#include <math.h>


CApp * CApp::m_instance = NULL;


bool CWindow::m_success = false;
CWindow * CWindow::m_instance = NULL;


#define FPS_STR2(x) #x
#define FPS_STR(x) FPS_STR2(x)


/* this function creates our window and sets it up properly */
bool CWindow::createGLWindow(const char* title, int width, int height,
          bool fullscreenflag)
{
  COutput * output = COutput::instanceOf();

  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    output->printf("SDL video init failed: %s\n", SDL_GetError());
    return false;
  }

  /* the window and taskbar icon come from fps.rc on Windows */
  SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, FPS_STR(IDI_FPS));
  SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, FPS_STR(IDI_SMALL));

#ifdef __EMSCRIPTEN__
  /* OpenGL ES 3.0, which the browser provides as WebGL 2 */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
  /* OpenGL 3.3 core: the old fixed-function calls do not exist here */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);   /* stencil mirror (T) */

  gl_win.fs = fullscreenflag;

  /* Fullscreen is borderless at the desktop resolution; the windowed size is
   * scaled with the display so it looks the same at 100% and 200%. */
  SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN;
  if (fullscreenflag) flags |= SDL_WINDOW_FULLSCREEN;

  m_window = SDL_CreateWindow(title, width, height, flags);
  if (!m_window) {
    output->printf("Window creation failed: %s\n", SDL_GetError());
    return false;
  }

  if (!fullscreenflag) {
    float scale = SDL_GetWindowDisplayScale(m_window) /
        SDL_GetWindowPixelDensity(m_window);
    if (scale > 0) {
      SDL_SetWindowSize(m_window, (int) lroundf(width * scale),
          (int) lroundf(height * scale));
    }
  }

  m_context = SDL_GL_CreateContext(m_window);
  if (!m_context) {
    output->printf("OpenGL context creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(m_window);
    m_window = NULL;
    return false;
  }

#ifndef __EMSCRIPTEN__
  if (!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress)) {
    output->printf("Could not load the OpenGL functions\n");
    killGLWindow();
    return false;
  }
#endif
  output->printf("OpenGL %s, %s\n", glGetString(GL_VERSION),
      glGetString(GL_RENDERER));
  if (!rnd::init()) {
    killGLWindow();
    return false;
  }

  SDL_ShowWindow(m_window);
  SDL_SyncWindow(m_window);

  /* the mouse looks around; capture it like the original pointer warping */
  SDL_SetWindowRelativeMouseMode(m_window, true);

  int pw, ph;
  SDL_GetWindowSizeInPixels(m_window, &pw, &ph);
  gl_win.width = pw;
  gl_win.height = ph;
  CGraphics::instanceOf()->resizeGLScene(gl_win.width, gl_win.height);

  m_app->onCreateWindow().execute();
  m_created = true;
  return true;// Success

} // createGLWindow




/* function to release/destroy our resources and restoring the old desktop */
void CWindow::killGLWindow()
{
  if (m_created) {
    m_app->onDestroyWindow().execute();
    m_created = false;
  }
  if (m_context) {
    rnd::shutdown();
    SDL_GL_DestroyContext(m_context);
    m_context = NULL;
  }
  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = NULL;
  }
}



void CWindow::swapFunction() {
  SDL_GL_SwapWindow(m_window);
}



void CWindow::setFullscreen(bool fullscreen) {
  if (SDL_SetWindowFullscreen(m_window, fullscreen)) gl_win.fs = fullscreen;
}




void CWindow::processEvents() {

  COutput * output = COutput::instanceOf();
  SDL_Event event;

  /* only produce text events while the user is typing a command */
  if (m_callbacks->isSendingLetter() != SDL_TextInputActive(m_window)) {
    if (m_callbacks->isSendingLetter())
      SDL_StartTextInput(m_window);
    else
      SDL_StopTextInput(m_window);
  }

  /* handle the events in the queue */
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_EVENT_QUIT:
        output->printf("Exiting sanely...\n");
        m_app->m_done = true;
        break;

      case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        gl_win.width = event.window.data1;
        gl_win.height = event.window.data2;
        CGraphics::instanceOf()->resizeGLScene(gl_win.width, gl_win.height);
        break;

      case SDL_EVENT_MOUSE_MOTION:
        {
          /* keep the sub-pixel remainder so slow movement is not lost */
          static float rest_x = 0, rest_y = 0;
          rest_x += event.motion.xrel;
          rest_y += event.motion.yrel;
          int dx = (int) rest_x, dy = (int) rest_y;
          rest_x -= dx;
          rest_y -= dy;
          if (dx || dy) m_callbacks->motion(dx, dy);
        }
        break;

      /* SDL numbers buttons like X11: 1 left, 2 middle, 3 right. Its side
       * buttons are 4 and 5, which X11 and this game use for the wheel, so
       * they move up to 6 and 7. */
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
        {
          unsigned int b = event.button.button;
          if (b > 3) b += 2;
          if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            m_callbacks->buttonPress(b);
          else
            m_callbacks->buttonRelease(b);
        }
        break;

      /* the wheel is buttons 4 (up) and 5 (down), as on X11 */
      case SDL_EVENT_MOUSE_WHEEL:
        if (event.wheel.y != 0) {
          unsigned int b = event.wheel.y > 0 ? 4 : 5;
          m_callbacks->buttonPress(b);
          m_callbacks->buttonRelease(b);
        }
        break;

      case SDL_EVENT_KEY_DOWN:
        m_callbacks->keyPress(event.key.scancode);
        if (m_callbacks->isSendingLetter()) {
          /* control characters do not arrive as text input */
          switch (event.key.key) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER:  m_callbacks->sendChar(13); break;
            case SDLK_BACKSPACE: m_callbacks->sendChar(8); break;
            case SDLK_TAB:       m_callbacks->sendChar(9); break;
            default: break;
          }
        }
        break;

      case SDL_EVENT_KEY_UP:
        m_callbacks->keyRelease(event.key.scancode);
        break;

      case SDL_EVENT_TEXT_INPUT:
        if (m_callbacks->isSendingLetter()) {
          for (const char * c = event.text.text; *c; ++c) {
            if ((unsigned char) *c < 0x80) m_callbacks->sendChar(*c);
          }
        }
        break;

      default:
        break;
    }
  }

}


//
// @(#) $Log: window.cpp,v $
// @(#) Revision 1.41  2003/10/05 02:02:43  virzak
// @(#) Added Mousewheel for windows
// @(#)
// @(#) Revision 1.40  2003/10/01 00:04:03  virzak
// @(#) Removed unnecessary includes
// @(#)
// @(#) Revision 1.39  2003/09/23 01:01:08  virzak
// @(#) Got rif of unnecessary commands
// @(#)
// @(#) Revision 1.38  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.37  2003/09/10 03:28:50  virzak
// @(#) When using output doing it thru COutput::instanceOf() instead of global
// @(#) variable, since test programs may not define any globals
// @(#)
// @(#) Revision 1.36  2003/09/04 02:04:00  virzak
// @(#) Changed keyPress and added sendChar calls as well as CCallbacks accesses
// @(#) Changed all tabs to double spaces
// @(#)
// @(#) Revision 1.35  2003/09/02 22:50:28  virzak
// @(#) Fixed masking code in KeyPress
// @(#)
// @(#) Revision 1.34  2003/09/02 20:45:59  virzak
// @(#) changes
// @(#)
// @(#) Revision 1.33  2003/08/30 02:17:41  virzak
// @(#) Removed all window pointers from global functions
// @(#)
// @(#) Revision 1.32  2003/08/29 09:54:39  virzak
// @(#) Added CWindow::proc that uses callbacks in CWindow class
// @(#)
// @(#) Revision 1.31  2003/08/29 08:27:14  virzak
// @(#) Replaced ui-> calls by CCallback function that are in turn overloaded
// @(#) by CUI
// @(#)
// @(#) Revision 1.30  2003/08/28 18:36:28  virzak
// @(#) Cleaned up a bit
// @(#)
// @(#) Revision 1.29  2003/08/28 01:58:06  virzak
// @(#) Fixed windows based bugs
// @(#)
// @(#) Revision 1.28  2003/08/27 21:58:34  virzak
// @(#) Moved void CWindow::initKeys to CUI::initKeys
// @(#)
// @(#) Revision 1.27  2003/08/26 20:26:59  virzak
// @(#) More reshuffling
// @(#)
// @(#) Revision 1.26  2003/08/26 19:47:55  virzak
// @(#) Reaganged for windows
// @(#)
// @(#) Revision 1.25  2003/08/26 19:00:58  virzak
// @(#) Merged CWindow::createGLWindow for Windows and Linux to with use of
// @(#) #ifdef WIN32, linux to implement similar functions once
// @(#)
// @(#) Revision 1.24  2003/08/26 08:41:24  virzak
// @(#) Moved windows _tWinMain from window to engine
// @(#)
// @(#) Revision 1.23  2003/08/26 08:14:31  virzak
// @(#) Moved main from window.cpp to engine.cpp
// @(#)
// @(#) Revision 1.22  2003/08/26 05:34:14  virzak
// @(#) gui.h >> window.h
// @(#)
// @(#) Revision 1.21  2003/08/26 05:06:45  virzak
// @(#) Changed top comments
// @(#)
// @(#) Revision 1.20  2003/08/26 04:09:01  virzak
// @(#) Changes resulting from movement of windowing functions into CWindow class
// @(#)
// @(#) Revision 1.19  2003/08/26 03:27:53  virzak
// @(#) Added output->setBase after it has been retrieved
// @(#)
// @(#) Revision 1.18  2003/08/14 01:53:38  virzak
// @(#) Added Shift-CapsLock typing capabilities
// @(#)
// @(#) Revision 1.17  2003/08/09 01:05:01  virzak
// @(#) Added stencil mirror.
// @(#)
// @(#) Revision 1.16  2003/08/04 07:56:16  virzak
// @(#) Added gr->deinitGL() call to kill command
// @(#)
// @(#) Revision 1.15  2003/08/03 21:08:37  virzak
// @(#) Make sure that the angle didn't change drastically on mouse cursor
// @(#) entry from a side
// @(#)
// @(#) Revision 1.14  2003/08/02 20:18:03  virzak
// @(#) Hid the mouse cursor (used the mask)
// @(#)
// @(#) Revision 1.13  2003/07/20 00:43:13  virzak
// @(#) Added tagra textures
// @(#)
// @(#) Revision 1.12  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
//


