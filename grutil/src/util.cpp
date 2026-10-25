//*****************************************************************************
// @(#) $Id: util.cpp,v 1.19 2003/12/10 09:29:00 virzak Exp $ 
// 
//  Description: 
//      Contains Vector, Point, Matrix and Color support
//
//  =
//
//*****************************************************************************

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <math.h>
#include <stdio.h>
#include <math.h>
#ifndef GRUTIL_NO_GL
#include <GL/gl.h>
#endif

#include "util.h"




GrMatrix4x4::GrMatrix4x4() {
  initMatrix();
}



GrMatrix4x4::GrMatrix4x4(double inAngle, char axis) {
  rotationMatrix(inAngle, axis);
}


/*
 * Some functions handling vectors
 */


/*
 * Calculates the crossproduct two vectors.
 */
const GrVector3D GrVector3D::operator*(const GrVector3D & v) const
{
  return GrVector3D(
      y() * v.z() - z() * v.y(), 
      z() * v.x() - x() * v.z(), 
      x() * v.y() - y() * v.x()
  );
}


unsigned int GrTriple3D::dominant() const
{
	const double x = fabs(this->x());
	const double y = fabs(this->y());
	const double z = fabs(this->z());

	if (x>y && x>z)
		return 0;
	else
		if (y>z)
			return 1;
		else
			return 2;
}



/*
 * transform a vector by multiplying it to a matrix from the right
 */

const GrVector3D operator*(const GrMatrix4x4 & mat, const GrVector3D & v)
{
  return GrVector3D(
      v.x() * mat.e[0][0] + v.y() * mat.e[0][1] + v.z() * mat.e[0][2],
      v.x() * mat.e[1][0] + v.y() * mat.e[1][1] + v.z() * mat.e[1][2],
      v.x() * mat.e[2][0] + v.y() * mat.e[2][1] + v.z() * mat.e[2][2]
  );
}




/*
 * transform a point by multiplying it to a matrix from the right
 */
const GrPoint3D operator*(const GrMatrix4x4 & mat, const GrPoint3D & p)
{
  GrPoint3D r = GrPoint3D(
    p.x() * mat.e[0][0] + p.y() * mat.e[0][1] + p.z() * mat.e[0][2],
    p.x() * mat.e[1][0] + p.y() * mat.e[1][1] + p.z() * mat.e[1][2],
    p.x() * mat.e[2][0] + p.y() * mat.e[2][1] + p.z() * mat.e[2][2]
  );
  
  r.e[0] += mat.e[0][3];
  r.e[1] += mat.e[1][3];
  r.e[2] += mat.e[2][3];
  return r;
}


const GrVector3D operator*(const GrVector3D & n, const GrMatrix4x4 & mat)
{
  return GrVector3D (
      n.x() * mat.e[0][0] + n.y() * mat.e[1][0] + n.z() * mat.e[2][0],
      n.x() * mat.e[0][1] + n.y() * mat.e[1][1] + n.z() * mat.e[2][1],
      n.x() * mat.e[0][2] + n.y() * mat.e[1][2] + n.z() * mat.e[2][2]
  );
  
}



/* *************************************************************************
 *
 * Handling matrices
 *
 * *************************************************************************/

/*
 * initialize a matrix to the identity matrix.
 */
void GrMatrix4x4::initMatrix()
{
  int i,j;
  
  for( i= 0 ; i< 4 ; ++i )
    for( j= 0 ; j< 4 ; ++j )
      e[i][j]= (i==j);
}

/*
 * Multiplying mat1*mat2
 */

const GrMatrix4x4 operator*(const GrMatrix4x4 & mat1, const GrMatrix4x4 & mat2) {
  int i,j;
  GrMatrix4x4 r;

  for( i= 0 ; i< 4 ; ++i )
    for( j= 0 ; j< 4 ; ++j )
      r.e[i][j]=  mat1.e[i][0] * mat2.e[0][j] +
                  mat1.e[i][1] * mat2.e[1][j] +
		  mat1.e[i][2] * mat2.e[2][j] +
		  mat1.e[i][3] * mat2.e[3][j];

  return r;  
}


const GrMatrix4x4 operator*(double s, const GrMatrix4x4 & mat) {
  int i,j;
  GrMatrix4x4 ret;
  
  for( i= 0 ; i< 4 ; ++i )
    for( j= 0 ; j< 4 ; ++j )
      ret.e[i][j]= s*mat.e[i][j];

  return ret;  
}


/*
 * Generate rotation matrix
 */
void GrMatrix4x4::rotationMatrix(double inAngle, char axis)
{
    int modifyEntry = 1;
    int success = 1;
    
    initMatrix();
    switch (axis) {
     case 'x': case 'X': modifyEntry += 0; break;
     case 'y': case 'Y': modifyEntry += 1; break;
     case 'z': case 'Z': modifyEntry += 2; break;
     default: initMatrix(); success = 0;
    }
    
    if (success) {
        e[(modifyEntry)%3][(modifyEntry)%3]=cos(deg2rad(inAngle));
        e[(modifyEntry)%3][(modifyEntry+1)%3]=-sin(deg2rad(inAngle));
        e[(modifyEntry+1)%3][(modifyEntry)%3]=sin(deg2rad(inAngle));
        e[(modifyEntry+1)%3][(modifyEntry+1)%3]=cos(deg2rad(inAngle));
    } else               
        fprintf(stderr,"NO Success, tvoyu mat' ;))\n");
}                        
                         
                         
/*                       
 * Generate translation matrix
 */                      
void GrMatrix4x4::translationMatrix(double inX, double inY, double inZ)
{                        
                         
    initMatrix();
    
    e[0][3]=inX;
    e[1][3]=inY;
    e[2][3]=inZ;
}


/*
 * Generate scaling matrix
 */
void GrMatrix4x4::scalingMatrix(double inX, double inY, double inZ)
{

    initMatrix();
    
    e[0][0]=inX;
    e[1][1]=inY;
    e[2][2]=inZ;
}



/*
 * 
 * 
 */
const GrMatrix4x4 GrMatrix4x4::invMatrix(int type) {
  GrMatrix4x4 ret = *this;
  
  if (type == 0) {
    /*translation*/
    ret.e[0][3] *= -1;
    ret.e[1][3] *= -1;
    ret.e[2][3] *= -1;
  } else if (type == 1) {
    /*scale*/
    ret.e[0][0] = 1/ret.e[0][0];
    ret.e[1][1] = 1/ret.e[1][1];
    ret.e[2][2] = 1/ret.e[2][2];
  } 
  return ret;
  
}

#ifndef GRUTIL_NO_GL  /* fixed-function OpenGL helpers */
void GrMatrix4x4::glMultMatrix () {
    int i,j;
    double mm[16];

    for( i= 0 ; i< 4 ; ++i ) 
      for( j= 0 ; j< 4 ; ++j ) 
        mm[j*4+i] = e[i][j];
  glMultMatrixd(mm);
}



void GrMatrix4x4::glGetModelView() {
  glGetDoublev(GL_MODELVIEW_MATRIX, (GLdouble *)e);
}
#endif
    
void GrMatrix4x4::print(ostream * out) {
  unsigned int i,j;
  
  *out << "{ ";
  for( i= 0 ; i< 4 ; ++i ) {
    *out << "<";
    for( j= 0 ; j< 4 ; ++j ) { 
      *out << e[i][j];
      if (j != 3) *out << ", ";
    }
    *out << "> ";
  }
  *out << "}";
}
    

/* *************************************************************************
 *
 * Handling colours
 *
 * *************************************************************************/



/*
 * component by component multiplication of two colours
 */
GrColour operator*( GrColour c1, GrColour c2 )
{
  //GrColour result;
  return GrColour(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b);

}

/*
 * calculates c1+c2
 */
GrColour operator+( GrColour c1, GrColour c2)
{
  //GrColour result;
  return GrColour(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);

}

/*
 * calculates c1+(c2*c3)
 */
GrColour cMultAdd( GrColour c1, GrColour c2, GrColour c3 )
{
  return c1+c2*c3;
  
}



/*
 * Normalizes vector v and returns the length before the
 * normalization
 */
double GrVector3D::normalize()
{
    double denom = 1;
    double x = ( this->x() > 0.0 ) ? this->x() : - this->x();
    double y = ( this->y() > 0.0 ) ? this->y() : - this->y();
    double z = ( this->z() > 0.0 ) ? this->z() : - this->z();
    if( x > y )
        {
        if( x > z )
            {
            if( 1.0 + x > 1.0 )
                {
                y = y / x;
                z = z / x;
                denom = 1.0 / ( x * sqrt( 1.0 + y*y + z*z ) );
                }
            }
        else /* z > x > y */
            {
            if( 1.0 + z > 1.0 )
                {
                y = y / z;
                x = x / z;
                denom = 1.0 / ( z * sqrt( 1.0 + y*y + x*x ) );
                }
            }
        }
    else
        {
        if( y > z )
            {
            if( 1.0 + y > 1.0 )
                {
                z = z / y;
                x = x / y;
                denom = 1.0 / ( y * sqrt( 1.0 + z*z + x*x ) );
                }
            }
        else /* x < y < z */
            {
            if( 1.0 + z > 1.0 )
                {
                y = y / z;
                x = x / z;
                denom = 1.0 / ( z * sqrt( 1.0 + y*y + x*x ) );
                }
            }
        }
    if( 1.0 + x + y + z > 1.0 )
        {
        *this = denom*(*this );
	return 1.0/denom;
        }
    return 0;
}

/*
void GrTriple3D::print(ostream * out) {
  *out << "(" << x() << "," << y() << "," << z() << ")";
}


void GrTriple3D::println(ostream * out) {
  this->print(out);
  *out << endl;
}
*/


const GrVector3D GrMkNormal(GrPoint3D p0, GrPoint3D p1, GrPoint3D p2) {
  
  GrVector3D normal = GrVector3D((p0-p1) * (p0-p2));

  normal.normalize();
  return normal;
}









// draw frame

#ifndef GRUTIL_NO_GL
void GrFrame3D::drawColoured() {
  for (unsigned int axis=0; axis < 3; ++axis) {
    glColor3d((axis == 0), (axis == 1), (axis == 2));
    glBegin(GL_LINE_STRIP);
    
    glVertex3dv(m_o);
    
    glVertex3dv(m_o + getFrameComp(axis));
    glEnd();
  }


  glColor3f(1.0, 1.0, 0.0);
  
  glBegin(GL_POINTS);
  glVertex3dv(m_o);
  glEnd();
  
}
#endif



extern "C" char grutilFunctionForConfigSrcipt() {
  return '\0';
}


//
// @(#) $Log: util.cpp,v $
// @(#) Revision 1.19  2003/12/10 09:29:00  virzak
// @(#) rewrote configure function
// @(#)
// @(#) Revision 1.18  2003/11/26 03:48:45  virzak
// @(#) added function for configure script
// @(#)
// @(#) Revision 1.17  2003/09/11 17:40:21  virzak
// @(#) Added const everywhere a class is returned by value
// @(#)
// @(#) Revision 1.16  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.15  2003/08/25 20:01:01  virzak
// @(#) Moved deg2rad to the header from cpp of util
// @(#) Added 2D relating functions
// @(#)
// @(#) Revision 1.14  2003/08/21 06:00:11  virzak
// @(#) Added GrFrame3D::drawColoured for frame drawing
// @(#)
// @(#) Revision 1.13  2003/08/20 16:27:11  virzak
// @(#) Windows bug
// @(#)
// @(#) Revision 1.12  2003/08/20 02:43:32  virzak
// @(#) Added many matrix routines.
// @(#) Moved dominant() to GrTriple3D from GrVector3D.
// @(#)
// @(#) Revision 1.11  2003/08/08 20:23:16  virzak
// @(#) Inlined more functions
// @(#) Removed "inline" word within class declaration (inline by default)
// @(#)
// @(#) Revision 1.10  2003/07/11 23:00:25  virzak
// @(#) Added GrVector2D and implemented conversion from 3D
// @(#)
// @(#) Revision 1.9  2003/07/11 19:20:34  virzak
// @(#) Developed inheritance more. Need to fix Tuple problem
// @(#)
// @(#) Revision 1.8  2003/07/11 03:35:01  virzak
// @(#) Constructed class GrTriple3D, which is a base class for GrPoint3D
// @(#) and GrVector3D
// @(#)
// @(#) Revision 1.7  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.6  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.5  2003/07/04 07:48:39  virzak
// @(#) Fixed functions by adding "const" storage directive
// @(#)
// @(#) Revision 1.4  2003/07/03 22:19:22  virzak
// @(#) Added magnitude function for vector
// @(#) Inlined more functions
// @(#)
//


