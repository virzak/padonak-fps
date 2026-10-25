//*****************************************************************************
// @(#) $Id: window.cpp,v 1.41 2003/10/05 02:02:43 virzak Exp $ 
// 
//  Description: 
//      Implements interface to X Window and Win32 Systems
//
//  =
//
//*****************************************************************************



#include "window.h"
#include "gr.h"


CApp * CApp::m_instance = NULL;


bool CWindow::m_success = false;
CWindow * CWindow::m_instance = NULL;




#ifdef WIN32

// Global Variables:
HDC      hDC=NULL;    // Private GDI Device Context
HGLRC    hRC=NULL;    // Permanent Rendering Context
HWND    hWnd=NULL;    // Holds Our Window Handle
HINSTANCE hInstance;                // current instance

// Forward declarations of functions included in this code module:
ATOM        MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);




#endif


#ifdef linux
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include "X11/Intrinsic.h"

#if 0
/* stuff about our window grouped together */
typedef struct {
  Display *dpy;
  int screen;
  Window win;
  GLXContext ctx;
  XSetWindowAttributes attr;
  XF86VidModeModeInfo deskMode;
  int x, y;
  unsigned int depth;  
} XGLWindow;

static XGLWindow GLWin;
#endif

static Pixmap bitmap;

static int dpyWidth, dpyHeight;


/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4, 
  GLX_GREEN_SIZE, 4, 
  GLX_BLUE_SIZE, 4, 
  GLX_DEPTH_SIZE, 16,
  None};

/* attributes for a double buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
  GLX_RED_SIZE, 4, 
  GLX_GREEN_SIZE, 4, 
  GLX_BLUE_SIZE, 4, 
  GLX_DEPTH_SIZE, 16,
  None };



void CWindow::centerMousePointer() {
  XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, gl_win.width/2, gl_win.height/2);
}

#endif // linux

void CWindow::makeRasterFont(void)
{

  COutput * output = COutput::instanceOf();

#ifdef linux
    XFontStruct *fontInfo;
    Font id;
    unsigned int first, last;

    fontInfo = XLoadQueryFont(GLWin.dpy,
        "-adobe-helvetica-medium-r-normal--17-120-100-100-p-88-iso8859-1");
    if (fontInfo == NULL) /* e.g. WSLg, which only has the built-in font */
        fontInfo = XLoadQueryFont(GLWin.dpy, "fixed");
    if (fontInfo == NULL) {
        output->printf ("no font found\n");
        exit (0);
    }

    id = fontInfo->fid;
    first = fontInfo->min_char_or_byte2;
    last = fontInfo->max_char_or_byte2;

    m_base = glGenLists((GLuint) last+1);
    if (m_base == 0) {
        output->printf ("out of display lists\n");
        exit (0);
    }
    glXUseXFont(id, first, last-first+1, m_base+first);
#endif // linux

#ifdef WIN32


    SelectObject (hDC, GetStockObject (SYSTEM_FONT)); 
    m_base = 1000;
    wglUseFontBitmaps (hDC, 0, 255, m_base); 

#endif //WIN32
    output->setBase(m_base);

}


/* this function creates our window and sets it up properly */
/* FIXME: bits is currently unused */
#ifdef __ICC
#pragma warning (disable:869)
#endif

bool CWindow::createGLWindow(char* title, int width, int height, int bits,
          bool fullscreenflag)
{

  COutput * output = COutput::instanceOf();
#ifdef linux
  XVisualInfo *vi;
  Colormap cmap;
  int i;
  int glxMajorVersion, glxMinorVersion;
  int vidModeMajorVersion, vidModeMinorVersion;
  XF86VidModeModeInfo **modes;
  int modeNum;
  int bestMode;
  Atom wmDelete;
  Window winDummy;
  unsigned int borderDummy;
  Cursor cursor;
  XColor fg, bg;
  

  gl_win.fs = fullscreenflag;
  /* set best mode to current */
  bestMode = 0;
  /* get a connection */
  GLWin.dpy = XOpenDisplay(0);
  GLWin.screen = DefaultScreen(GLWin.dpy);
  XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
    &vidModeMinorVersion);
#if 0
  bitmap = XCreateBitmapFromData(
    GLWin.dpy, 
    RootWindowOfScreen(DefaultScreenOfDisplay(GLWin.dpy)), 
    "\000\000\001\000\000\001\000\000\001\377\377\377", 
    /*"\000\000\000\000\000\000\000\000\000\000\000\000",*/
    /*"\377\377\377\377\377\377\377\377\377\377\377\377",*/
    4, 
    3
  ); 
#endif
  
  bitmap = XCreateBitmapFromData(
    GLWin.dpy, 
    RootWindowOfScreen(DefaultScreenOfDisplay(GLWin.dpy)), 
    "\000", 
    1, 
    1
  );
  
  /*
  bitmap = XCreatePixmapFromBitmapData(GLWin.dpy, 
      RootWindowOfScreen(DefaultScreenOfDisplay(GLWin.dpy)), 
      "\000", 1, 1, 0, 0, 1);
  */
    
  /*
  XLookupColor(GLWin.dpy, colormap, "DarkCyan", &fg, &screen);
  XLookupColor(GLWin.dpy, colormap, "Blue", &fg, &screen);
  */
  
  fg.red = 0;
  fg.green = 0;
  fg.blue = 0;

  bg.red = 0;
  bg.green = 65535;
  bg.blue = 0;
  
  cursor = XCreatePixmapCursor(GLWin.dpy, bitmap, bitmap, &fg, &bg, 0, 0);
  /*
  bitmap = XCreatePixmap(GLWin.dpy, RootWindowOfScreen(GLWin.screen), 100, 200, DefaultDepthOfScreen(GLWin.screen));
  */
  
      
       
  output->printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
    vidModeMinorVersion);
  XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
  /* save desktop-resolution before switching modes */
  GLWin.deskMode = *modes[0];
  /* look for mode with requested resolution */
  for (i = 0; i < modeNum; ++i)
  {
    if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
    {
      bestMode = i;
    }
  }
  /* get an appropriate visual */
  vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
  if (vi == NULL)
  {
    vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
    output->printf("Only Singlebuffered Visual!\n");
  }
  else
  {
    output->printf("Got Doublebuffered Visual!\n");
  }
  glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
  output->printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
  /* create a GLX context */
  GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
  /* create a color map */
  cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
    vi->visual, AllocNone);
  GLWin.attr.colormap = cmap;
  GLWin.attr.border_pixel = 0;

  if (gl_win.fs)
  {
    XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
    XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
    dpyWidth = modes[bestMode]->hdisplay;
    dpyHeight = modes[bestMode]->vdisplay;
    output->printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
    XFree(modes);
  
    /* create a fullscreen window */
    GLWin.attr.override_redirect = True;
    GLWin.attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask | EnterWindowMask;
    
    //Doesn't work, bug in X
    //GLWin.attr.cursor = None;
    GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
      0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
      CWBorderPixel | CWColormap | CWEventMask | /*CWCursor |*/ CWOverrideRedirect,
      &GLWin.attr);
  XMapRaised(GLWin.dpy, GLWin.win);
    XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
      GrabModeAsync, CurrentTime);
    XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
      GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
  }
  else
  {
    /* create a window in window mode*/
    GLWin.attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask | EnterWindowMask;
    GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
      0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
      CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
    /* only set window title and handle wm_delete_events if in windowed mode */
    wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
    XSetStandardProperties(GLWin.dpy, GLWin.win, title,
      title, None, NULL, 0, NULL);
    XMapRaised(GLWin.dpy, GLWin.win);
  }     
  /* connect the glx-context to the window */
  glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
  /*
  XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
    &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
  */

  XDefineCursor(GLWin.dpy, GLWin.win, cursor);
  centerMousePointer();
  XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
    &gl_win.width, &gl_win.height, &borderDummy, &GLWin.depth);
  
  output->printf("Depth %d\n", GLWin.depth);
  if (glXIsDirect(GLWin.dpy, GLWin.ctx)) 
    output->printf("Congrats, you have Direct Rendering!\n");
  else
    output->printf("Sorry, no Direct Rendering possible!\n");
#endif // linux












  
#ifdef WIN32

/*  This Code Creates Our OpenGL Window.  Parameters Are:          *
 *  title      - Title To Appear At The Top Of The Window        *
 *  width      - Width Of The GL Window Or Fullscreen Mode        *
 *  height      - Height Of The GL Window Or Fullscreen Mode      *
 *  bits      - Number Of Bits To Use For Color (8/16/24/32)      *
 *  fullscreenflag  - Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)  */
 

  GLuint    PixelFormat;      // Holds The Results After Searching For A Match
  WNDCLASS  wc;            // Windows Class Structure
  DWORD    dwExStyle;        // Window Extended Style
  DWORD    dwStyle;        // Window Style
  RECT    WindowRect;        // Grabs Rectangle Upper Left / Lower Right Values


  if (gl_win.fs) {
    DEVMODE dmScreenSettings;
    EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings );
    gl_win.width = dmScreenSettings.dmPelsWidth;
    gl_win.height = dmScreenSettings.dmPelsHeight;
  }

  // The manifest makes us per-monitor DPI aware, so scale the windowed
  // size ourselves instead of letting Windows stretch a blurry bitmap.
  UINT dpi = GetDpiForSystem();
  if (!gl_win.fs) {
    gl_win.width = MulDiv(gl_win.width, dpi, USER_DEFAULT_SCREEN_DPI);
    gl_win.height = MulDiv(gl_win.height, dpi, USER_DEFAULT_SCREEN_DPI);
  }



  WindowRect.left=(long)0;      // Set Left Value To 0
  WindowRect.right=(long)gl_win.width;    // Set Right Value To Requested Width
  WindowRect.top=(long)0;        // Set Top Value To 0
  WindowRect.bottom=(long)gl_win.height;    // Set Bottom Value To Requested Height


  hInstance      = GetModuleHandle(NULL);        // Grab An Instance For Our Window
  wc.style      = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc    = (WNDPROC) WndProc;          // WndProc Handles Messages
  wc.cbClsExtra    = 0;                  // No Extra Window Data
  wc.cbWndExtra    = 0;                  // No Extra Window Data
  wc.hInstance    = hInstance;              // Set The Instance
  wc.hIcon      = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FPS));
  wc.hCursor      = LoadCursor(NULL, IDC_ARROW);      // Load The Arrow Pointer
  wc.hbrBackground  = NULL;                  // No Background Required For GL
  wc.lpszMenuName    = NULL;                  // We Don't Want A Menu
  wc.lpszClassName  = "OpenGL";                // Set The Class Name

  if (!RegisterClass(&wc))                  // Attempt To Register The Window Class
  {
    MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                      // Return FALSE
  }
  
  if (gl_win.fs)                        // Attempt Fullscreen Mode?
  {
    DEVMODE dmScreenSettings;                // Device Mode
    memset(&dmScreenSettings,0,sizeof(dmScreenSettings));  // Makes Sure Memory's Cleared
    dmScreenSettings.dmSize=sizeof(dmScreenSettings);    // Size Of The Devmode Structure
    dmScreenSettings.dmPelsWidth  = gl_win.width;        // Selected Screen Width
    dmScreenSettings.dmPelsHeight  = gl_win.height;        // Selected Screen Height
    //dmScreenSettings.dmBitsPerPel  = bits;          // Selected Bits Per Pixel
    //dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
    dmScreenSettings.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT;

    // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
    {
      // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
      if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
      {
        gl_win.fs=FALSE;    // Windowed Mode Selected.  Fullscreen = FALSE
      }
      else
      {
        // Pop Up A Message Box Letting User Know The Program Is Closing.
        MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
        return FALSE;                  // Return FALSE
      }
    }
  }

  if (gl_win.fs)                        // Are We Still In Fullscreen Mode?
  {
    dwExStyle=WS_EX_APPWINDOW;                // Window Extended Style
    dwStyle=WS_POPUP;                    // Windows Style
    ShowCursor(FALSE);                    // Hide Mouse Pointer
  }
  else
  {
    dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;      // Window Extended Style
    dwStyle=WS_OVERLAPPEDWINDOW;              // Windows Style
  }

  //if (!gl_win.fs)
  AdjustWindowRectExForDpi(&WindowRect, dwStyle, FALSE, dwExStyle, dpi);

  // Create The Window
  if (!(hWnd=CreateWindowEx(  dwExStyle,              // Extended Style For The Window
                "OpenGL",              // Class Name
                title,                // Window Title
                dwStyle |              // Defined Window Style
                WS_CLIPSIBLINGS |          // Required Window Style
                WS_CLIPCHILDREN,          // Required Window Style
                0, 0,                // Window Position
                WindowRect.right-WindowRect.left,  // Calculate Window Width
                WindowRect.bottom-WindowRect.top,  // Calculate Window Height
                NULL,                // No Parent Window
                NULL,                // No Menu
                hInstance,              // Instance
                NULL)))                // Dont Pass Anything To WM_CREATE
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  static  PIXELFORMATDESCRIPTOR pfd=        // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),        // Size Of This Pixel Format Descriptor
    1,                      // Version Number
    PFD_DRAW_TO_WINDOW |            // Format Must Support Window
    PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,              // Must Support Double Buffering
    PFD_TYPE_RGBA,                // Request An RGBA Format
    bits,                    // Select Our Color Depth
    0, 0, 0, 0, 0, 0,              // Color Bits Ignored
    0,                      // No Alpha Buffer
    0,                      // Shift Bit Ignored
    0,                      // No Accumulation Buffer
    0, 0, 0, 0,                  // Accumulation Bits Ignored
    16,                      // 16Bit Z-Buffer (Depth Buffer)  
    1,                      // 1Bit Stencil Buffer
    0,                      // No Auxiliary Buffer
    PFD_MAIN_PLANE,                // Main Drawing Layer
    0,                      // Reserved
    0, 0, 0                    // Layer Masks Ignored
  };
  
  if (!(hDC=GetDC(hWnd)))              // Did We Get A Device Context?
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))  // Did Windows Find A Matching Pixel Format?
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  if(!SetPixelFormat(hDC,PixelFormat,&pfd))    // Are We Able To Set The Pixel Format?
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  if (!(hRC=wglCreateContext(hDC)))        // Are We Able To Get A Rendering Context?
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  if(!wglMakeCurrent(hDC,hRC))          // Try To Activate The Rendering Context
  {
    killGLWindow();                // Reset The Display
    MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return FALSE;                // Return FALSE
  }

  ShowWindow(hWnd,SW_SHOW);            // Show The Window
  SetForegroundWindow(hWnd);            // Slightly Higher Priority
  SetFocus(hWnd);                  // Sets Keyboard Focus To The Window
  CGraphics::instanceOf()->resizeGLScene(gl_win.width, gl_win.height);          // Set Up Our Perspective GL Screen

#endif //WIN32

  makeRasterFont();
  m_app->onCreateWindow().execute();
  return true;// Success

} // createGLWindow

#ifdef __ICC
#pragma warning (default:869)
#endif




/* function to release/destroy our resources and restoring the old desktop */
void CWindow::killGLWindow(GLvoid)
{
  COutput * output = COutput::instanceOf();
  m_app->onDestroyWindow().execute();
#if linux
  if (GLWin.ctx)
  {
    if (!glXMakeCurrent(GLWin.dpy, None, NULL))
    {
      output->printf("Could not release drawing context.\n");
    }
    glXDestroyContext(GLWin.dpy, GLWin.ctx);
    GLWin.ctx = NULL;
  }
  /* switch back to original desktop resolution if we were in fs */
  if (gl_win.fs)
  {
    XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
    XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
  }
  XCloseDisplay(GLWin.dpy);
  //teardownWorld();
#endif


#ifdef WIN32
  if (gl_win.fs)                    // Are We In Fullscreen Mode?
  {
    ChangeDisplaySettings(NULL,0);          // If So Switch Back To The Desktop
    ShowCursor(TRUE);                // Show Mouse Pointer
  }

  if (hRC)                      // Do We Have A Rendering Context?
  {
    if (!wglMakeCurrent(NULL,NULL))          // Are We Able To Release The DC And RC Contexts?
    {
      MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    }

    if (!wglDeleteContext(hRC))            // Are We Able To Delete The RC?
    {
      MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    }
    hRC=NULL;                    // Set RC To NULL
  }

  if (hDC && !ReleaseDC(hWnd,hDC))          // Are We Able To Release The DC
  {
    MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    hDC=NULL;                    // Set DC To NULL
  }

  if (hWnd && !DestroyWindow(hWnd))          // Are We Able To Destroy The Window?
  {
    MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    hWnd=NULL;                    // Set hWnd To NULL
  }

  if (!UnregisterClass("OpenGL",hInstance))      // Are We Able To Unregister Class
  {
    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    hInstance=NULL;                  // Set hInstance To NULL
  }

#endif
  
  
}



void CWindow::swapFunction() {
#ifdef linux
  glXSwapBuffers(GLWin.dpy, GLWin.win);
#endif
#ifdef WIN32
  SwapBuffers(hDC);
#endif
}




/* get the keycodes of the keys we want to react on and store them in an array.
 * This way we do not need to get them everytime we check for pressed keys.
 */


#ifdef linux

void CWindow::processEvents() {

  COutput * output = COutput::instanceOf();
  XEvent event;

    /* handle the events in the queue */
    while (XPending(GLWin.dpy) > 0)
    {
      XNextEvent(GLWin.dpy, &event);
      switch (event.type)
      {
        case Expose:
          if (event.xexpose.count != 0)
            break;
          //draw(); // No reason, since we are redrawing it constantly 
          break;
        case ConfigureNotify:
        /* call resizeGLScene only if our window-size changed */
          if (((unsigned int)event.xconfigure.width != gl_win.width) || 
            ((unsigned int)event.xconfigure.height != gl_win.height))
          {
            gl_win.width = event.xconfigure.width;
            gl_win.height = event.xconfigure.height;
            CGraphics::instanceOf()->resizeGLScene(gl_win.width, gl_win.height);
          }
          break;
        /* exit in case of a mouse button press */
        case EnterNotify:
          m_callbacks->enter();
          break;
        case MotionNotify:
          m_callbacks->motion(event.xmotion.x, event.xmotion.y);
          break;
        case ButtonPress:
          m_callbacks->buttonPress(event.xbutton.button);
          break;
        case ButtonRelease:
          m_callbacks->buttonRelease(event.xbutton.button);
          break;
        case KeyPress:
          m_callbacks->keyPress(event.xbutton.button/*, c*/);
          if (m_callbacks->isSendingLetter()) {
            unsigned long l = XKeycodeToKeysym(GLWin.dpy, event.xkey.keycode, 0);

            if (l == XK_Return || l == XK_KP_Enter) {
              l = 13;
            }
            
            
            if (l <= 0x7f || l == XK_BackSpace || l == XK_Tab) {
              
              KeySym lc, uc;
              XConvertCase(l, &lc, &uc);

              unsigned int mask = event.xkey.state;

              if (((mask & ShiftMask) != 0) ^ ((mask & LockMask) != 0)) {
                l = uc;
              } else {
                //l = lc;
              }
            
#ifdef __ICC
#pragma warning (disable : 810)
#endif
              m_callbacks->sendChar(0x7f & l);
#ifdef __ICC
#pragma warning( default : 810 )
#endif
            }
    }
          
          break;
        case KeyRelease:
          m_callbacks->keyRelease(event.xbutton.button);
          break;
        case ClientMessage:  
          if (*XGetAtomName(GLWin.dpy, event.xclient.message_type) == 
            *"WM_PROTOCOLS")
          {
            output->printf("Exiting sanely...\n");
            m_app->m_done = true;
          }
          break;
        default:
          break;
      }
    }

}

#endif

#ifdef WIN32



void CWindow::processEvents() {
  MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))  // Is There A Message Waiting?
    {
      if (msg.message==WM_QUIT)        // Have We Received A Quit Message?
      {
        m_app->m_done=true;   // If So done=TRUE
      }
      else                  // If Not, Deal With Window Messages
      {
        TranslateMessage(&msg);        // Translate The Message
        DispatchMessage(&msg);        // Dispatch The Message
      }
    }
}






void CWindow::centerMousePointer() {
      POINT pt;                  // cursor location  
      pt.x = gl_win.width/2;
      pt.y = gl_win.height/2;

          ClientToScreen(hWnd, &pt);


        SetCursorPos(pt.x, pt.y); 
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX); 

  wcex.style      = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc  = (WNDPROC)WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance    = hInstance;
  wcex.hIcon      = LoadIcon(hInstance, (LPCTSTR)IDI_FPS);
  wcex.hCursor    = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  //wcex.lpszMenuName  = (LPCTSTR)IDC_FPS;
  //wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm    = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

  return RegisterClassEx(&wcex);
}

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif //WM_MOUSEWHEEL
#ifndef WHEEL_DELTA
#define WHEEL_DELTA  120
#endif //WHEEL_DELTA




LRESULT CALLBACK CWindow::proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) 
  {
  case WM_CHAR:
      if (m_callbacks->isSendingLetter()) {
        m_callbacks->sendChar((unsigned int) wParam);
      }

    break;  

  case WM_KEYDOWN:   // Is A Key Being Held Down?
        m_callbacks->keyPress((unsigned int) wParam);
    break;  
  case WM_KEYUP:      // Has A Key Been Released?
                m_callbacks->keyRelease((unsigned int) wParam);
    break;  
  case WM_LBUTTONDOWN:
                m_callbacks->buttonPress(1);
    break;  
  case WM_MBUTTONDOWN:
                m_callbacks->buttonPress(2);
    break;  
  case WM_RBUTTONDOWN:
                m_callbacks->buttonPress(3);
    break;  
  case WM_LBUTTONUP:
                m_callbacks->buttonRelease(1);
    break;  
  case WM_MBUTTONUP:
                m_callbacks->buttonRelease(2);
    break;  
  case WM_RBUTTONUP:
                m_callbacks->buttonRelease(3);
    break;
  case WM_DESTROY:
    if (m_callbacks->isDestroyedByUser())
      PostQuitMessage(0);
    break;  
  case WM_MOUSEMOVE: 
    m_callbacks->motion(LOWORD(lParam), HIWORD(lParam));
    break;
  case WM_MOUSEWHEEL:
    m_callbacks->buttonPress( (short int)HIWORD(wParam)>0?4:5);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}



//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT  - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;

        CWindow * window = CWindow::instanceOf();
        
        switch (message) 
  {
  case WM_COMMAND:
    wmId    = LOWORD(wParam); 
    wmEvent = HIWORD(wParam); 
    // Parse the menu selections:
    switch (wmId)
    {
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    //draw();
    EndPaint(hWnd, &ps);
    break;
  case WM_CREATE:
        //draw();
    break;  
  case WM_DESTROY:
    if (window)
                  return window->proc(hWnd, message, wParam, lParam);
                else
      PostQuitMessage(0);
    break;  

  case WM_SIZE:                // Resize The OpenGL Window

    if (CWindow * window = CWindow::instanceOf()) {
                window->gl_win.width = LOWORD(lParam);
                window->gl_win.height = HIWORD(lParam);
    }
    CGraphics::instanceOf()->resizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height


    break;

  case WM_DPICHANGED:          // Moved to a monitor with different scaling
    {
      RECT *r = (RECT *) lParam;
      SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left,
                   r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    break;
    

  default:
                if (window) {
                  return window->proc(hWnd, message, wParam, lParam);
                } else
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}



#endif


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


