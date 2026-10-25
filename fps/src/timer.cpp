//*****************************************************************************
// @(#) $Id: timer.cpp,v 1.13 2003/10/18 00:20:50 virzak Exp $ 
// 
//  Description: 
//      This file contains class Timer which handles timing
//
//  =
//
//*****************************************************************************

#include "timer.h"
#include <SDL3/SDL_timer.h>


Timer::Timer() : m_current(0), m_paused(false), m_highest_fps(0) {

  // Get start time
  m_start = SDL_GetTicks();

  pause();
}

void Timer::update() {

  // Get current time
  long new_current = 0;
  
  
  do {
    new_current = (long) (SDL_GetTicks() - m_start);

    // Get change
    m_change = new_current - m_current;
    //*COutput::instanceOf() << "n\n";
  } while (m_highest_fps && m_change < 1000.f/m_highest_fps);
  m_current = new_current;
}

  
void Timer::pause() {
  m_paused = !m_paused;
  update();
  if (m_paused) {
    // Set paused time
    m_pausedTime = m_current;
  } else {
    // Move start time by pause length
    long int was_paused_for = m_current - m_pausedTime;
    m_current = m_pausedTime;
    m_start += was_paused_for;
  }
}


// Application wide timer
Timer timer;


//
// @(#) $Log: timer.cpp,v $
// @(#) Revision 1.13  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.12  2003/10/04 22:25:00  virzak
// @(#) Timer constructor pauses the timer
// @(#)
// @(#) Revision 1.11  2003/10/01 00:03:42  virzak
// @(#) Got rid of g++ and icc warnings
// @(#)
// @(#) Revision 1.10  2003/09/30 23:14:34  virzak
// @(#) Initializing m_current to 0
// @(#)
// @(#) Revision 1.9  2003/09/30 22:46:54  virzak
// @(#) Fixed pause bug
// @(#)
// @(#) Revision 1.8  2003/09/23 04:41:29  virzak
// @(#) Added FPS control
// @(#)
// @(#) Revision 1.7  2003/09/22 20:35:40  virzak
// @(#) Merged windows and linux implementations
// @(#) Added pause() and isPaused method to CTimer class.
// @(#) Rewrote update method
// @(#)
// @(#) Revision 1.6  2003/09/21 21:47:46  virzak
// @(#) Fixed for high frame rate.
// @(#) Replaced the timer by Hi-Res timer
// @(#)
//


