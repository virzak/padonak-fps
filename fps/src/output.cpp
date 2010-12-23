//*****************************************************************************
// @(#) $Id: output.cpp,v 1.18 2003/09/24 06:49:00 virzak Exp $ 
// 
//  Description: 
//      Provides output routines for the screen and the console
//
//  =
//
//*****************************************************************************



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "output.h"


COutput * COutput::m_instance = NULL;


COutput::COutput(
    unsigned int lines, 
    unsigned int chars_in_line
    ) : m_total_lines(lines), m_chars_in_line(chars_in_line), 
        m_first(0), m_size(0), m_cur_char(0), m_first_line(0), m_lines(0),
        m_outputFunction(NULL), m_tobuffer(true), m_lines_back(0) {

          
  
  m_line_buffer = new char * [m_total_lines];
  
  for (unsigned int i=0; i<m_total_lines; ++i) {
    m_line_buffer[i] = new char [m_chars_in_line+1];
    memset(m_line_buffer[i], 0, m_chars_in_line+1);
  }

}

COutput::~COutput(){

  m_instance=NULL;
  
  for (unsigned int i=0; i<m_total_lines; ++i) {
    delete [] m_line_buffer[i];
  }

  delete [] m_line_buffer;
}


COutput * COutput::instanceOf() {
  if (!m_instance) {
    m_instance = new COutput(100, 80);
  }
  return m_instance;
  
}




void COutput::addToBuffer(const char c) {
  bool crlf;

      // Store char unless it is a carriage return
  if (!(crlf = (c=='\n' || c=='\r' )) && c != 0x08) {
    m_line_buffer[(m_first_line+m_lines)%m_total_lines][m_cur_char] = c;

  }

  // Backspace
  if (c == 0x08) {
    if (m_cur_char) {
      m_line_buffer[(m_first_line+m_lines)%m_total_lines][--m_cur_char] = '\0';
      m_command.erase(m_command.length()-1,1);
    }
    return;
  }

  // check for end of the line
  if (m_cur_char == m_chars_in_line-1 || crlf) {
    m_cur_char = 0;
    
    // check if al lines are full
    if (m_lines == m_total_lines) {
      ++m_first_line;
      memset(m_line_buffer[(m_first_line+m_lines)%m_total_lines], 0, m_chars_in_line+1);
    } else {
      ++m_lines;
    }
    
  } else {
    ++m_cur_char;
  }
  
  if (crlf) {
    // TODO: Do the command
    if (m_outputFunction) m_outputFunction(m_command);
  
    // Reset the string
    m_command = "";
  } else {
    m_command += c;
  }
      
}

 

void COutput::printf(const char *fmt, ...)
{
  va_list argp;
  char tmp[200];

  va_start(argp, fmt);
  vsprintf(tmp, fmt, argp);
  COutput::print(tmp);
  va_end(argp);
}


void COutput::draw(unsigned int x, unsigned int y, unsigned int show_lines) {

  unsigned int can_show = m_lines+1;
  
  if (show_lines>can_show) show_lines=can_show;
  if (show_lines+m_lines_back>can_show) m_lines_back=m_lines+1-show_lines;

  for (unsigned int i=0; i<show_lines; ++i) {
    glRasterPos2i(x, y-i*14);
 
    unsigned int buffer_index = (m_first_line+can_show-show_lines+i-m_lines_back) % m_total_lines;
     
	char * output_line = m_line_buffer[buffer_index];

	char help_output_line[100];
	sprintf(help_output_line, "%d) %s", buffer_index, output_line);
	//sprintf(help_output_line, "> %s", output_line);

    glCallLists (
        (GLsizei) strlen(help_output_line), 
        GL_UNSIGNED_BYTE, 
        (GLubyte *) help_output_line
    );
  }
}


void COutput::gprintf(const int x, const int y, const char *fmt, ...)
{
  va_list argp;
  char tmp[200];

  va_start(argp, fmt);
  vsprintf(tmp, fmt, argp);
  glRasterPos2i(x, y);
  glCallLists ((GLsizei) strlen(tmp), GL_UNSIGNED_BYTE, (GLubyte *) tmp);
  
  va_end(argp);
}



//
// @(#) $Log: output.cpp,v $
// @(#) Revision 1.18  2003/09/24 06:49:00  virzak
// @(#) Added user command buffer and passing
// @(#)
// @(#) Revision 1.17  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.16  2003/09/10 03:29:51  virzak
// @(#) Moved implementation of COutput::selfTest to the test program to make
// @(#) the executable smaller
// @(#)
// @(#) Revision 1.15  2003/09/04 01:03:57  virzak
// @(#) Expanded test cases
// @(#)
// @(#) Revision 1.14  2003/08/28 09:11:36  virzak
// @(#) Added m_lines_back member which used to be a global variable
// @(#)
// @(#) Revision 1.13  2003/08/27 22:01:12  virzak
// @(#) Remover setprecision for now
// @(#)
// @(#) Revision 1.12  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.11  2003/08/26 03:26:20  virzak
// @(#) Implemented (need to fix) << in a specific location
// @(#) COutput now stores the base of the bitmap
// @(#)
// @(#) Revision 1.10  2003/08/14 01:54:26  virzak
// @(#) Added SelfTest and fixed the Backspace bug
// @(#)
// @(#) Revision 1.9  2003/07/09 05:27:06  virzak
// @(#) Fixed two windows warnings
// @(#)
// @(#) Revision 1.8  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.7  2003/07/03 22:10:07  virzak
// @(#) Added comments and put a line number on display
// @(#)
//


