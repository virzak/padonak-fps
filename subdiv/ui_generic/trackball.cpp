/*
 * Module : trackball.c
 *
 * Author : Greg Veres
 *
 * Date   : April 10, 1995
 *
 * Purpose: Contains routines to provide trackball-style 
 *          rotations.
 *
 */
#include <math.h>
#include <GL/gl.h>
#include "trackball.h"
/*
#include "OGLwin.h"
#include "events.h"
*/
/* 
 * Local Global Variables
 */
static double fLastX, fLastY, fLastZ;

/*
 * Name      : void vRotateScene(float fVecX, float fVecY, float fVecZ, 
 *                               Matrix mNewMat)
 *
 * Parameters: float  fVecX   - The X component of the rotation vector.
 *             float  fVecY   - The Y component of the rotation vector.
 *             float  fVecZ   - The Z component of the rotation vector.
 *             Matrix mNewMat - The matrix to store the rotation in.
 *
 * Returns   : void
 *
 * Purpose   : Generates a matrix based on a rotation vector.
 */
void vRotateScene(double fVecX, double fVecY, double fVecZ, GrMatrix4x4 & mNewMat)
{
    static double fScale = 1.0 * 180.0 / M_PI;
    double fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    fVecX *= -1.0;
    fVecY *= -1.0;

    /* 
     * Find the length of the vector.
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /*  
     * If the vector has zero length - return the identity matrix.
     */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        mNewMat.initMatrix();
        return;
    }

    /* 
     * Normalize the rotation vector.
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = -fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;
    fRadians  *= fScale;  

    /*
     * Use OpenGL to rotate about an arbitrary axis
     */
    glPushMatrix();
    glLoadIdentity();
    glRotated(fRadians, fNewVecX, fNewVecY, fNewVecZ);
    glGetDoublev(GL_MODELVIEW_MATRIX, (GLdouble *)mNewMat.elemArray());
    glPopMatrix();
}

/*
 * Name      : void vGenerateRotVec(float fCurrentX, float fCurrentY, 
 *                                  float *fVecX, float *fVecY, float *fVecZ)
 *
 * Parameters: float fCurrentX - The current X location of the cursor.
 *             float fCurrentY - The current Y location of the cursor.
 *             float *fVecX    - The X component of the rotation vector.
 *             float *fVecY    - The Y component of the rotation vector.
 *             float *fVecZ    - The Z component of the rotation vector.
 *
 * Returns   : void
 *
 * Purpose   : Computes a rotation vector for the trackball
 *             interaction.
 */
void vGenerateRotVec(double fCurrentX, double fCurrentY, 
		     double *fVecX, double *fVecY, double *fVecZ, 
		     int nWinWidth, int nWinHeight) {

   long  nXOrigin, nYOrigin;
   double fNewX, fNewY, fNewZ, fLength;
   int min = nWinHeight;

   if (nWinWidth < min) min = nWinHeight;
   /*
    * Compute center of window.
    */
   nXOrigin = nWinWidth / 2;
   nYOrigin = nWinHeight / 2;

   /*
    * Calculate components.
    */
   fNewX    = (fCurrentX - (double)nXOrigin) * 4.0 / (double)min;
   fNewY    = (fCurrentY - (double)nYOrigin) * 4.0 / (double)min;
   fNewZ    = (1.0 - fNewX * fNewX - fNewY * fNewY);

   /*
    * Moving around outside of window.
    */
   if (fNewZ < 0.0) {
      fLength = sqrt(1.0 - fNewZ);
      fNewZ  = 0.0;
      fNewX /= fLength;
      fNewY /= fLength;
   } else {
      fNewZ = sqrt(fNewZ);
   }

   /*
    * Generate rotation vector.
    */
   *fVecX = fLastY * fNewZ - fNewY * fLastZ;
   *fVecY = fLastZ * fNewX - fNewZ * fLastX;
   *fVecZ = (fLastX * fNewY - fNewX * fLastY);

   /*
    * Update for next time.
    */
   fLastX = fNewX;
   fLastY = fNewY;
   fLastZ = fNewZ;
}
