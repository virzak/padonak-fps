/*
 * Module : trackball.h
 *
 * Author : Greg Veres
 *
 * Date   : April 10, 1995
 *
 * Purpose: Header file for trackball.c.
 *
 */
#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "util.h"

/*#include "events.h"*/
/*
 * Function Prototypes
 */
void vRotateScene(double, double, double, GrMatrix4x4 *);
void vGenerateRotVec(double, double, double*, double*, double*, int, int);

#endif
