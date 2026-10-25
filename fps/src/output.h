//*****************************************************************************
// @(#) $Id: output.h,v 1.19 2003/09/24 06:49:00 virzak Exp $ 
// 
//  Description: 
//      Provides output routines for the screen and the console
//
//  =
//
//*****************************************************************************


#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#ifdef _WIN32
#include "stdafx.h"
#endif


#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <sstream>
#include <string.h>

using namespace::std;

typedef ostream& (*manip) (ostream&);

class COutput {
  
  // Singleton variable
  static COutput * m_instance;

  public:
    class Pos {
        int m_x;
        int m_y;
      public:
        Pos() : m_x(0), m_y(0) {}
        Pos(int x, int y) : m_x(x), m_y(y) {}
        friend class COutput;
    };

  private:
    typedef void (COutput::*func)();
  
    ostringstream m_strstream;
    
    char ** m_line_buffer;
    string m_command;
    const unsigned int m_total_lines;
    const unsigned int m_chars_in_line;
    unsigned int m_first;
    unsigned int m_size;
    unsigned int m_cur_char;
    unsigned int m_first_line;
    unsigned int m_lines;
    void (*m_outputFunction)(const string &);
    
    // Constructor
    COutput(unsigned int lines,  unsigned int chars_in_line);

    
    void addToBuffer(const char* string) {
      for (unsigned int i=0; i<strlen(string); ++i) {
        addToBuffer(string[i]);
      }
    }
    void addToBuffer(const char c);


    string m_gr_buf;
    
    void send(const char* string) {
      if (m_tobuffer) {
        cout << string;
        addToBuffer(string);
      } else {
        m_gr_buf += string;
      }
    }
    
    void OutInPos() {
      gprintf(m_p.m_x, m_p.m_y, m_gr_buf.c_str());
      m_gr_buf = "";
    }
    
    void send(const char c) {
      if (m_tobuffer) {
        cout << c;
        addToBuffer(c);
      } else {
        if (c=='\n') {
          OutInPos();
        } else {
          m_gr_buf += c;
        }
      }
    }

    bool m_tobuffer;
    
    Pos m_p;
    
  public:
    
    unsigned int m_lines_back;

    
    void setOutputFunction(void (*outputFunction)(const string &)) {
      m_outputFunction = outputFunction;
    }
    
    ~COutput();
    static COutput * instanceOf();
    void print(const char * string) {
        ::printf(string);
        addToBuffer(string);
    }
    void printf(const char *fmt, ...);
    static void gprintf(const int x, const int y, const char *fmt, ...);
    void draw(unsigned int x, unsigned int y, unsigned int show_lines);


    COutput & operator << ( COutput &( *f )( COutput & ) ) {
      return (*f)( *this );
    }


    // Manipulators like endl, flush and ends
    COutput & operator<<(manip f) {
      //(*f)( cout );
      if (f == (manip) endl || f == (manip) flush || f == (manip) ends) {
        send('\n');
        m_tobuffer = true;
      } else {
        *this << "FUCKED";
      }
      return *this;
    }

    // Simple types 
    COutput & operator<<( const string s ) {
      send(s.c_str());
      return *this;
    }
    COutput & operator<<( const char * s ) {
      send(s);
      return *this;
    }
    COutput & operator<<( const char c ) {
      send(c);
      return *this;
    }

    COutput & operator<<( const unsigned char c ) {
      send(c);
      return *this;
    }
    
    // TODO: Something better
    COutput & operator<<( const Pos & p ) {
      m_tobuffer = false;
      if (m_gr_buf.length()) OutInPos();
      m_p=p;
      return *this;
    }


    // Hex, fixed and other manipulators
    COutput & operator<< (ios_base & hryu(ios_base &)) {
      m_strstream << hryu;
      return *this;
    }
    

    // All other types
    template <class T>
    COutput & operator<< (const T & t) {
      string tmp;
      m_strstream << t << ends;
      send(m_strstream.str().c_str());
      m_strstream.seekp(0); // Back up 
      return *this;
    }
    
    
    void selfTest();
    
};


#endif


//
// @(#) $Log: output.h,v $
// @(#) Revision 1.19  2003/09/24 06:49:00  virzak
// @(#) Added user command buffer and passing
// @(#)
// @(#) Revision 1.18  2003/09/11 19:44:45  virzak
// @(#) Comments
// @(#)
// @(#) Revision 1.17  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.16  2003/09/10 06:47:30  virzak
// @(#) Fixed g++ bug
// @(#)
// @(#) Revision 1.15  2003/09/04 01:06:20  virzak
// @(#) Improved the class by including ios_base (hex, oct, fixed...) types,
// @(#) converting everything to ostringstream
// @(#)
// @(#) Revision 1.14  2003/08/29 08:19:02  virzak
// @(#) Added const unsigned char
// @(#)
// @(#) Revision 1.13  2003/08/28 09:11:36  virzak
// @(#) Added m_lines_back member which used to be a global variable
// @(#)
// @(#) Revision 1.12  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.11  2003/08/26 03:26:20  virzak
// @(#) Implemented (need to fix) << in a specific location
// @(#) COutput now stores the base of the bitmap
// @(#)
// @(#) Revision 1.10  2003/08/14 01:55:22  virzak
// @(#) Replaced all the << function by template and added selfTest signature
// @(#)
// @(#) Revision 1.9  2003/07/27 02:34:53  virzak
// @(#) improved streaming by using strstream
// @(#)
// @(#) Revision 1.8  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.7  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.6  2003/07/03 22:10:07  virzak
// @(#) Added comments and put a line number on display
// @(#)
//

