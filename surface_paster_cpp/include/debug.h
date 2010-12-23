// debug.h 
//	Data structures for debugging routines.
//
//	The debugging routines allow the user to turn on selected
//	debugging messages, controllable from the command line arguments
//	passed to Nachos (-d).  You are encouraged to add your own
//	debugging flags.  Please.... 
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef DEBUG_H
#define DEBUG_H
#include <assert.h>

//----------------------------------------------------------------------
// ASSERT
//      If condition is false,  print a message and dump core.
//	Useful for documenting assumptions in the code.
//
//	NOTE: needs to be a #define, to be able to print the location 
//	where the error occurred.
//----------------------------------------------------------------------

#ifdef NO_DEBUG
#define ASSERT(condition) 
#define ASSERT_COLOUR(c)
#else
#define ASSERT(condition)                                               \
    if (condition) {} else { 						\
	printf("Assertion failed: line %i file %s \n", (int) __LINE__, (char*) __FILE__);      \
        abort();\
    }

#define ASSERT_COLOUR(c) (ASSERT(c.r>=0 && c.r < 1 && c.g>=0 && c.g < 1 && c.b>=0 && c.b < 1))
#endif


#endif // DEBUG_H







