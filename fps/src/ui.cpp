//*****************************************************************************
// @(#) $Id: ui.cpp,v 1.37 2003/10/05 01:15:27 virzak Exp $ 
// 
//  Description: 
//      This file provides to user I/O changes by calling appropriate methods
//
//  =
//
//*****************************************************************************

#include <math.h>
#include "ui.h"
#include "window.h"
#include "engine.h"
#include "camera.h"

#include <SDL3/SDL_scancode.h>

typedef enum {
  FPS_FS_TOGGLE,
  FPS_EXIT,
  FPS_LIGHT,
  FPS_FILTER,
  FPS_LOOK_UP,
  FPS_LOOK_DOWN,
  FPS_LOOK_LEFT,
  FPS_LOOK_RIGHT,
  FPS_MOVE_FORWARD,
  FPS_MOVE_BACK,
  FPS_MOVE_LEFT,
  FPS_MOVE_RIGHT,
  FPS_FLY,
  FPS_MIRROR,
  FPS_JUMP,
  FPS_TOGGLE_OUTPUT,
  FPS_OUTPUT_UP,
  FPS_OUTPUT_DOWN,
  FPS_TOGGLE_USER_PRINT,
  FPS_TOGGLE_DRAW_MYSELF,
  FPS_FIRE,
  FPS_VIEW,
  FPS_TOGGLE_STENCIL,
  FPS_TOGGLE_PAUSE,
  FPS_TOTAL 
} FPSActionType;

static bool action[FPS_TOTAL];
static bool last_action[FPS_TOTAL];

CUI * CUI::m_instance = NULL;


#define K_C(n) (m_keys[m_keyCodes[n]])

void CUI::keyButtonAction() {
  
  action[FPS_EXIT] = K_C(0);
  if (!m_send_letter) {
    action[FPS_FS_TOGGLE] = K_C(1);
    action[FPS_LIGHT] = K_C(2);
    action[FPS_FILTER] = K_C(3);
    
    if (output_toggle) {
  	action[FPS_OUTPUT_UP] = K_C(5);
  	action[FPS_OUTPUT_DOWN] = K_C(6); 
    } else {
  	action[FPS_LOOK_UP] = K_C(5);
  	action[FPS_LOOK_DOWN] = K_C(6); 
    }


    action[FPS_LOOK_LEFT] = K_C(9);
    action[FPS_LOOK_RIGHT] = K_C(10);
    
    action[FPS_MOVE_FORWARD] = K_C(7) || K_C(13);
    action[FPS_MOVE_BACK] = K_C(8) || K_C(14);

    action[FPS_MOVE_LEFT] = K_C(11);
    action[FPS_MOVE_RIGHT] = K_C(12);
    
    action[FPS_FLY] = K_C(15);
    action[FPS_MIRROR] = K_C(16);
    action[FPS_JUMP] = m_buttons[3];
    action[FPS_FIRE] = m_buttons[1];
    action[FPS_TOGGLE_OUTPUT] = K_C(17);

    action[FPS_TOGGLE_USER_PRINT] = K_C(18);
    action[FPS_TOGGLE_DRAW_MYSELF] = K_C(19);
    action[FPS_VIEW] = K_C(20);
    action[FPS_TOGGLE_STENCIL] = K_C(21);
    action[FPS_TOGGLE_PAUSE] = K_C(22);

  }

  buttonAction();
  keyAction();


}


void CUI::rollUp() {
  if (action[FPS_VIEW]) {
    cam->m_dist *= 1.2f;
  }
}

void CUI::rollDown() {
  if (action[FPS_VIEW]) {
    cam->m_dist /= 1.2f;
  }
}

void CUI::buttonAction()
{
  if (action[FPS_JUMP]) {
    cp->jump();
  }
  if (action[FPS_FIRE]) {
    cp->fire();
  }
}





/* this function uses the keycode to determine if an action should be performed.
 * It replaces the keyPressed-function of former lessons to provide smooth 
 * (more quake-like *grin*) moving in our 3D-world
 */



inline bool getAction(FPSActionType action_type, bool repetative = true) {
  bool ret = action[action_type] && (repetative || !last_action[action_type]);
  if (!repetative) last_action[action_type] = action[action_type];
  return ret;
}

inline bool risingAction(FPSActionType action_type) {
  return action[action_type] && !last_action[action_type];
}

inline bool fallingAction(FPSActionType action_type) {
  return !action[action_type] && last_action[action_type];
}

inline bool edgeAction(FPSActionType action_type) {
  return risingAction(action_type) || fallingAction(action_type);
}



void CUI::keyAction()
{

  static bool switched_view = false;
    
  if (getAction(FPS_EXIT, false))
  {

	if (m_send_letter) {
          m_send_letter = false;
	} else {
	  CApp::instanceOf()->m_done = true;
	}

  }
  if (getAction(FPS_FS_TOGGLE, false))
  {
    m_destroyed_by_user = false;
    bool new_fs_flag = !window->gl_win.fs;
    timer.pause();
    delete window;
    window = CWindow::instanceOf("Victor's FPS", 640, 480, 24, new_fs_flag, 
        this);
    timer.pause();
    m_destroyed_by_user = true;
  }
  if (getAction(FPS_LIGHT, false))
  {
    gr->m_light = !gr->m_light;
    if (!gr->m_light) {
      rnd::disable(rnd::LIGHTING);
    } else {
      rnd::enable(rnd::LIGHTING);
    }
  }
  if (getAction(FPS_FILTER, false))
  {
    filter += 1;
    if (filter > 2) {
      filter = 0;
    }
  }
  if (action[FPS_LOOK_UP])
      cp->changeLook(0, 1);
  
  if (getAction(FPS_LOOK_DOWN))
      cp->changeLook(0, -1);
  
  unsigned int dir = 0;
  
  if (getAction(FPS_MOVE_FORWARD)) {
    dir |= 1<<D_forward;
  }
  
  if (getAction(FPS_MOVE_BACK)) {
    dir |= (1<<D_backward);
  }

  if (getAction(FPS_MOVE_LEFT)) {
    dir |= (1<<D_left);
  }
  
  if (getAction(FPS_MOVE_RIGHT)) {
    dir |= (1<<D_right);
  }
  
  static unsigned int last_dir = 0;
  
  if (dir != last_dir) {
    cp->accel(dir);
    last_dir = dir;
  }
  
  
  
  if (getAction(FPS_LOOK_LEFT))
    cp->changeLook(-.6, 0);
  
  if (getAction(FPS_LOOK_RIGHT))
    cp->changeLook(.6, 0);
  
  if (getAction(FPS_FLY, false)) 
    cp->m_flyingMode = !cp->m_flyingMode;
  
  
  if (getAction(FPS_MIRROR, false)) 
    mirror = !mirror;
  
  
  if (getAction(FPS_TOGGLE_OUTPUT, false))
  {
      output_toggle = !output_toggle;
      output->m_lines_back = 0;
  }
  if (getAction(FPS_OUTPUT_UP))
  {
	  output->m_lines_back++;
  }
  if (getAction(FPS_OUTPUT_DOWN))
  {
    if (output->m_lines_back) {
      output->m_lines_back--;
    }
  }
  if (getAction(FPS_TOGGLE_USER_PRINT, false))
  {
    output_toggle = 1;
    output->m_lines_back = 0;
    m_send_letter = true;
  }
  if (getAction(FPS_TOGGLE_STENCIL, false))
  {
    mirror_stencil = !mirror_stencil;
  }
  
  if (getAction(FPS_TOGGLE_PAUSE, false))
  {
    timer.pause();
  }
  
  
  if (getAction(FPS_TOGGLE_DRAW_MYSELF, false)) draw_myself = !draw_myself;
  
  if (risingAction(FPS_VIEW) && !switched_view ||
        fallingAction(FPS_VIEW) && switched_view) {
    cam->switchView();
  }
  if (fallingAction(FPS_VIEW)) {
    switched_view = !switched_view;
    cam->init();
  }
  
  
  last_action[FPS_VIEW] = action[FPS_VIEW];

}




void CUI::motionAction(int dx, int dy) {
  
  if (timer.isPaused()) return;

  if (!action[FPS_VIEW]) {
    cp->changeLook(0.1*dx, -0.1*dy);
  } else {
    cam->changeLook(0.1*dx, -0.1*dy);
  }
}

void CUI::reset() {
  memset(m_keys, 0, sizeof(m_keys));
  memset(m_buttons, 0, 10*sizeof(bool));
}


void CUI::initKeys()
{
  output->printf("Initializing keys...\n");
  /* scancodes are layout independent: WASD stays put on AZERTY */
  /* get keycode for escape-key */
  m_keyCodes[0] = SDL_SCANCODE_ESCAPE;
  /* get keycode for F1 */
  m_keyCodes[1] = SDL_SCANCODE_F1;
  /* get keycode for l */
  m_keyCodes[2] = SDL_SCANCODE_L;
  /* get keycode for f */
  m_keyCodes[3] = SDL_SCANCODE_F;
  /* get keycode for b */
  m_keyCodes[4] = SDL_SCANCODE_B;  //FREE
  /* get keycode for Page_Up */
  m_keyCodes[5] = SDL_SCANCODE_PAGEUP;
  /* get keycode for Page_Down */
  m_keyCodes[6] = SDL_SCANCODE_PAGEDOWN;
  /* get keycode for Up-Arrow */
  m_keyCodes[7] = SDL_SCANCODE_UP;
  /* get keycode for Down-Arrow */
  m_keyCodes[8] = SDL_SCANCODE_DOWN;
  /* get keycode for Left-Arrow */
  m_keyCodes[9] = SDL_SCANCODE_LEFT;
  /* get keycode for Right-Arrow */
  m_keyCodes[10] = SDL_SCANCODE_RIGHT;
  /* get keycode for a */
  m_keyCodes[11] = SDL_SCANCODE_A;
  /* get keycode for d */
  m_keyCodes[12] = SDL_SCANCODE_D;
  /* get keycode for w */
  m_keyCodes[13] = SDL_SCANCODE_W;
  /* get keycode for s */
  m_keyCodes[14] = SDL_SCANCODE_S;
  /* get keycode for o */
  m_keyCodes[15] = SDL_SCANCODE_O;
  /* get keycode for m */
  m_keyCodes[16] = SDL_SCANCODE_M;
  /* get keycode for ` */
  m_keyCodes[17] = SDL_SCANCODE_GRAVE;
  /* get keycode for p */
  m_keyCodes[18] = SDL_SCANCODE_P;
  /* get keycode for n */
  m_keyCodes[19] = SDL_SCANCODE_N;
  /* get keycode for TAB */
  m_keyCodes[20] = SDL_SCANCODE_TAB;
  /* get keycode for t */
  m_keyCodes[21] = SDL_SCANCODE_T;
  /* get keycode for pause (F3, as on Windows) */
  m_keyCodes[22] = SDL_SCANCODE_F3;

}

void CUI::parseUserSource(istream & in) {
  
  m_send_letter = true;
  
  for (;;) {
    string line;
    getline(in, line);
    if (in.eof()) break;
    if (line[0] == '#') continue;
    parseUserCommand(line);
  }
}

void CUI::parseUserCommand(const string & s) {

  if (!m_send_letter) return;

  m_send_letter = false;
  
  istringstream sstream(s);

  string command;
  string variable;
  sstream >> command;

  if (command == "set") {
    sstream >> variable;
    if (variable == "fps") {
      unsigned int fps_rate;
      sstream >> fps_rate;
      timer.highsetFPS(fps_rate);
    } else if (variable == "pos") {
      sstream >> cp->m_pos;
    } else if (variable == "vel") {
      sstream >> cp->m_velocity;
    } else {
      *output << "ERROR (set): unknown variable: " << variable << "\n";
    }
    
  } else if (command == "reset") {
    sstream >> variable;
    
    if (sstream.eof() || variable == "player") {
      cp->m_pos.zero();
      cp->m_velocity.zero();
    }
    
  } else if (command == "source") {
    string filename;
    sstream >> filename;
    
    ifstream source_stream(filename.c_str());
    if (source_stream) {
      parseUserSource(source_stream);
    } else {
      *output << "ERROR (source): could not open script: " << filename << "\n";
    }
    source_stream.close();
    
  } else if (command == "help") {
    
    string help_command;
    
    if (sstream.eof()) {
      *output << "Usage: help <command> where command is one of:\n";
      *output << "    set, reset, source, help\n";
    } else {
      sstream >> help_command;
      if (help_command == "set") {
        *output << "set - sets a variable\n";
        *output << "Usage: set <variable> <params>\n";
        *output << "    set pos <x> <y> <z> - sets player posision\n";
        *output << "    set vel <x> <y> <z> - sets player velocity\n";
        *output << "    set fps <fps> - sets maximum refresh rate\n";
        
      } else if (help_command == "reset") {
        *output << "reset - resets player's position and velocities to origin\n";
        *output << "Usage: reset\n";
        
      } else if (help_command == "source") {
        *output << "source - loads commands from a file\n";
        *output << "Usage: source <pathname>\n";
        
      } else if (help_command == "help") {
        *output << "help - displays help on various topics\n";
        *output << "Usage: help <command>\n";
        
      } else {
        *output << "ERROR (help): no help for command: \"" << 
          help_command << "\" provided.\n"; 
      }
    }
    
  } else {
    *output << "ERROR: unknown command: " << command << 
      ". Type \"help\" for help.\n";
  }
  
  m_send_letter = true;
}



//
// @(#) $Log: ui.cpp,v $
// @(#) Revision 1.37  2003/10/05 01:15:27  virzak
// @(#) Pasung while destroying a window
// @(#)
// @(#) Revision 1.36  2003/10/04 23:04:12  virzak
// @(#) Added mouse wheel functions
// @(#) Added input parsing
// @(#)
// @(#) Revision 1.35  2003/09/30 22:47:14  virzak
// @(#) Allowing positioning and resetting
// @(#)
// @(#) Revision 1.34  2003/09/30 10:55:03  virzak
// @(#) Allowing position changing
// @(#)
// @(#) Revision 1.33  2003/09/24 06:49:32  virzak
// @(#) Added user command parsing and executing
// @(#)
// @(#) Revision 1.32  2003/09/23 01:03:24  virzak
// @(#) Changed VK_PAUSE to VK_F3 for windows
// @(#)
// @(#) Revision 1.31  2003/09/22 20:36:36  virzak
// @(#) Added pause handling
// @(#)
// @(#) Revision 1.30  2003/09/21 06:39:42  virzak
// @(#) Small change
// @(#)
// @(#) Revision 1.29  2003/09/12 03:30:45  virzak
// @(#) removed unnecessary output
// @(#)
// @(#) Revision 1.28  2003/09/04 01:16:36  virzak
// @(#) Changed global destroyed_by_user to member m_destroyed_by_user
// @(#)
// @(#) Revision 1.27  2003/08/29 08:22:51  virzak
// @(#) Many small changes
// @(#)
// @(#) Revision 1.26  2003/08/28 09:09:01  virzak
// @(#) Got rid of all blending functionalities for the user.
// @(#) output_lines_back are now with COutput class
// @(#)
// @(#) Revision 1.25  2003/08/27 22:45:51  virzak
// @(#) Another horrible bug
// @(#)
// @(#) Revision 1.24  2003/08/27 21:59:48  virzak
// @(#) Added initKeys function
// @(#)
// @(#) Revision 1.23  2003/08/26 18:25:18  virzak
// @(#) Fixed a most horrible bug
// @(#)
// @(#) Revision 1.22  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.21  2003/08/26 05:34:14  virzak
// @(#) gui.h >> window.h
// @(#)
// @(#) Revision 1.20  2003/08/26 04:09:01  virzak
// @(#) Changes resulting from movement of windowing functions into CWindow class
// @(#)
// @(#) Revision 1.19  2003/08/14 01:08:32  virzak
// @(#) On "p" the output appears either way.
// @(#)
// @(#) Revision 1.18  2003/08/09 01:05:01  virzak
// @(#) Added stencil mirror.
// @(#)
// @(#) Revision 1.17  2003/08/03 21:10:40  virzak
// @(#) Renamed go to m_centering with opposite effects
// @(#)
// @(#) Revision 1.16  2003/08/02 20:26:44  virzak
// @(#) Added camera interface.
// @(#) Added risingAction, fallingAction and edgeAction to detect change in
// @(#) key pressing.
// @(#)
// @(#) Revision 1.15  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.14  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.13  2003/07/03 22:22:45  virzak
// @(#) Added capabilities for acceleration, velocity.
// @(#) Tied the positioning to them in order to have more realistic movement
// @(#)
//


