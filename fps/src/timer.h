//*****************************************************************************
// @(#) $Id: timer.h,v 1.8 2003/10/18 00:20:50 virzak Exp $ 
// 
//  Description: 
//      This file contains class Timer which handles timing
//
//  =
//
//*****************************************************************************

#ifndef __TIMER_H__
#define __TIMER_H__

class Timer {
  unsigned long long m_start;   /* SDL_GetTicks() milliseconds */

  long int m_current;
  long int m_change;
  long int m_pausedTime;
  bool m_paused;
  
  unsigned int m_highest_fps;
  
  public:
  Timer();
  void highsetFPS(unsigned int highest_fps) {m_highest_fps = highest_fps;}
  bool isPaused() {return m_paused;}
  void pause();
  void update();

  long int getTime() {
    return (m_paused ? m_pausedTime : m_current);
  }

  long int getChange() {
    return (m_paused ? 0 : m_change);
  }

};


extern Timer timer;


#endif /*__TIMER_H__*/


//
// @(#) $Log: timer.h,v $
// @(#) Revision 1.8  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.7  2003/09/24 06:49:53  virzak
// @(#) Fixed a typo
// @(#)
// @(#) Revision 1.6  2003/09/23 04:41:29  virzak
// @(#) Added FPS control
// @(#)
// @(#) Revision 1.5  2003/09/22 20:35:40  virzak
// @(#) Merged windows and linux implementations
// @(#) Added pause() and isPaused method to CTimer class.
// @(#) Rewrote update method
// @(#)
// @(#) Revision 1.4  2003/09/21 21:47:46  virzak
// @(#) Fixed for high frame rate.
// @(#) Replaced the timer by Hi-Res timer
// @(#)
//


