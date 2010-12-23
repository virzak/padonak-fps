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


Timer::Timer() : m_current(0), m_paused(false), m_highest_fps(0) {

#ifdef linux
  // Get start time
  ftime(&m_start);
#endif

#ifdef WIN32
  // Determine frequency in seconds
  LARGE_INTEGER ticksPerSecond;
  QueryPerformanceFrequency(&ticksPerSecond);

  // Convert to msecs
  m_ticksPerMSec = (unsigned long) (ticksPerSecond.QuadPart/1000);

  // Get start time
  QueryPerformanceCounter(&m_start);
#endif

  // Set last
  m_last = m_start;

  pause();
}

void Timer::update() {

  // Get current time
#ifdef linux
  struct timeb
#endif
#ifdef WIN32
  LARGE_INTEGER
#endif
                 cur;

  long new_current = 0;
  
  
  do {
#ifdef linux
    ftime(&cur);
    new_current = (cur.time-m_start.time)*1000+ cur.millitm-m_start.millitm;
#endif
#ifdef WIN32
    QueryPerformanceCounter(&cur);
    new_current = (long) ((cur.QuadPart-m_start.QuadPart)/m_ticksPerMSec);
#endif

    // Get change
    m_change = new_current - m_current;
    //*COutput::instanceOf() << "n\n";
  } while (m_highest_fps && m_change < 1000.f/m_highest_fps);
  m_current = new_current;

  // Set last
  m_last = cur;
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
#ifdef linux
    time_t paused_for_secs = was_paused_for / 1000;
    
#ifdef __ICC
#pragma warning (disable:810)
#endif

    unsigned short int paused_for_millisecs = was_paused_for % 1000;
    bool overflow = (m_start.millitm + paused_for_millisecs) >= 1000;
    if (!overflow) {
      m_start.millitm += paused_for_millisecs;
      m_start.time += paused_for_secs;
    } else {
      m_start.millitm += paused_for_millisecs - 1000;
      m_start.time += paused_for_secs + 1;
    }

#ifdef __ICC
#pragma warning (default:810)
#endif

#endif
#ifdef WIN32
    m_start.QuadPart += was_paused_for*m_ticksPerMSec;
#endif
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


