//*****************************************************************************
// @(#) $Id: util.h,v 1.45 2003/12/10 09:28:41 virzak Exp $ 
// 
//  Description: 
//      Contains Vector, Point, Matrix and Color support
//
//  =
//
//*****************************************************************************


#ifndef _UTIL_
#define _UTIL_

#include <iostream>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace::std;




inline double deg2rad(double deg) {
    return M_PI * deg / 180;
}



struct GrColour {
    double r;
    double g;
    double b;
  public:
    /*GrColour*/
    GrColour(double r, double g, double b) : r(r), g(g), b(b) {}
    GrColour(double grey) : r(grey), g(grey), b(grey) {}
    operator double *() {return &r;}
};    /* RGB */


/* Material.
 */
struct GrMaterial {
    GrColour diffuse;
    GrColour specular_colour;
    double specular_exp;
    GrMaterial(GrColour d, GrColour s, double s_e) :
      diffuse(d), specular_colour(d), specular_exp(s_e)
      {}
};



class GrVector3D;
class GrPoint3D;



class GrMatrix4x4 {
    double e[4][4];
  public:
    GrMatrix4x4();
    GrMatrix4x4(double, char);
    void initMatrix();
    void rotationMatrix(double inAngle, char axis);
    void translationMatrix(double inX, double inY, double inZ);
    void scalingMatrix(double inX, double inY, double inZ);

    void rotate(double inAngle, char axis) {
      GrMatrix4x4 m;
      m.rotationMatrix(inAngle, axis);
      *this = *this * m;
    }
    
    void translate(double inX, double inY, double inZ) {
      GrMatrix4x4 m;
      m.translationMatrix(inX, inY, inZ);
      *this = *this * m;
    }
    
    void scale(double inX, double inY, double inZ) {
      GrMatrix4x4 m;
      m.scalingMatrix(inX, inY, inZ);
      *this = *this * m;
    }

    double * elemArray() {return (double *) e;}
    const GrMatrix4x4 invMatrix(int type);
    friend const GrVector3D operator*(const GrMatrix4x4 & mat, const GrVector3D & v);
    friend const GrPoint3D operator*(const GrMatrix4x4 & mat, const GrPoint3D & p);
    friend const GrVector3D operator*(const GrVector3D & n, const GrMatrix4x4 & mat);
    friend const GrMatrix4x4 operator*(const GrMatrix4x4 & mat1, const GrMatrix4x4 &);
    friend const GrMatrix4x4 operator*(double, const GrMatrix4x4 &);
    operator const double *() const {return e[0];}
    void glMultMatrix(); 
    void glGetModelView(); 

    void print(ostream * out = &cout);
    void println(ostream * out = &cout) {
      print(out);
      *out << endl;
    }

};



inline double modd(double d, unsigned int p) {
#ifdef __ICC
#pragma warning (disable : 810)
#endif
  return ( (unsigned int)d % p) + d - (unsigned int)d;
#ifdef __ICC
#pragma warning( default : 810 )
#endif
}



// This is a base class for all the tuples
class GrTuple {
    const unsigned int n;
  protected:
    double *e;
  public:
    
    // Constructor
    GrTuple(const unsigned int n) :  n(n) {
      e = new double [n];
    }

    // Destructor
    virtual ~GrTuple() {
      delete [] e;
    }

    // Copy constructor
    GrTuple(const GrTuple& t) : n(t.n) {
      e = new double [n];
      memcpy((void *) e, t.e, n*sizeof(double));
    }

    // Assignment operator
    const GrTuple & operator=(const GrTuple & t) {
      unsigned int sm_n = (n<t.n ? n : t.n);
      memcpy((void *) e, t.e, sm_n*sizeof(double));
      return *this;
    }

    // Other methods
    void zero() {memset(e, 0, n*sizeof(double));}
    const double * elemArray() const {return e;}
    double getComponent(const unsigned int component_index) const {
      return e[component_index]; 
    }
    void setComponent(
        const unsigned int component_index, 
        const double & val
    ) {
      e[component_index] = val; 
    }


    // Conversion Function
    operator const double *() const {return e;}

    /*
    GrTuple operator%(unsigned int p) {
      GrTuple ret(n);
      for (int i=0; i<n; i++) {
        ret.e[i] = modd(e[i], p);
      }

      return ret;
    }
    */

    
    template <class T> friend T operator%(const T & t, unsigned int p);

    friend inline istream & operator >> (istream &is, GrTuple & t);    
};

class GrPair2D : public GrTuple {
  public:

    // Constructors
    GrPair2D() : GrTuple(2) {
      zero();
    }
    
    GrPair2D(double x, double y) : GrTuple(2) {
      e[0]=x; e[1]=y;
    }

    GrPair2D(double * e) : GrTuple(2) {
      memcpy( (void *) (this->e), e, 2*sizeof(double));
    }
    
    GrPair2D (const double & degree) : GrTuple(2) {
      double rads = deg2rad(degree);
      e[0]=cos(rads); e[1]=sin(rads);
    }


    // Destructor
    virtual ~GrPair2D() {}

    // Member access
    const double & x() const {return e[0];}
    const double & y() const {return e[1];}

    void x(const double & a) {e[0] = a;}
    void y(const double & a) {e[1] = a;}

    // Equality Functions
    bool operator==(const GrPair2D &v2) const {
      return (x()==v2.x() && y()==v2.y());
    }
    bool operator==(const double & d) const {return (x()==d && y()==d);}
    
    
    bool operator!=(const GrPair2D &v2) const {return !(*this == v2);}
    bool operator!=(const double & d) const {return !(*this == d);}
    
    // Don't use... interferes with operator double *() in the base
    //operator bool() const {return (*this != (double)0.0f);}
    
    // calculates the dot product of two vectors and returns it
    double operator^( const GrPair2D & v2) const {
      return x() * v2.x() + y() * v2.y();
    }
    
    void print(ostream * out = &cout) const {
      *out << "(" << x() << "," << y() << ")";
    }
    
    void println(ostream * out = &cout) const {
      this->print(out);
      *out << endl;
    }

};




class GrTriple3D : public GrTuple {
  public:

    // Constructors
    GrTriple3D() : GrTuple(3) {
      zero();
    }
    
    GrTriple3D(double x, double y, double z) : GrTuple(3) {
      e[0]=x; e[1]=y; e[2]=z;
    }
    
    GrTriple3D(double * e) : GrTuple(3) {
      memcpy( (void *) (this->e), e, 3*sizeof(double));
    }

    GrTriple3D(float * e) : GrTuple(3) {
      for (unsigned int i=0; i<3; ++i)
        this->e[i] = e[i];
    }

    // Destructor
    virtual ~GrTriple3D() {}

    // Member access
    const double & x() const {return e[0];}
    const double & y() const {return e[1];}
    const double & z() const {return e[2];}

    void x(const double & a) {e[0] = a;}
    void y(const double & a) {e[1] = a;}
    void z(const double & a) {e[2] = a;}


    // Equality Functions
    bool operator==(const GrTriple3D &v2) const {
      return (x()==v2.x() && y()==v2.y() && z()==v2.z());
    }
    
    bool operator==(const double & d) const {
      return (x()==d && y()==d && z()==d);
    }
    
    
    bool operator!=(const GrTriple3D &v2) const {return !(*this == v2);}
    bool operator!=(double d) const {return !(*this == d);}
    operator bool() const {return (*this != (double)0.0f);}
    
    // calculates the dot product of two vectors and returns it
    double operator^( const GrTriple3D & v2) const {
      return x() * v2.x() + y() * v2.y() + z() * v2.z();
    }
    
    unsigned int dominant() const;

    void print(ostream * out = &cout) const {
      *out << "(" << x() << "," << y() << "," << z() << ")";
    }
    
    void println(ostream * out = &cout) const {
      this->print(out);
      *out << endl;
    }

};





class GrVector2D : public GrPair2D {
  public:
    GrVector2D() : GrPair2D() {}
    GrVector2D(double x, double y) : GrPair2D(x, y) {}
    GrVector2D(double *e) : GrPair2D(e) {}
    GrVector2D(const double & d) : GrPair2D(d) {}
    friend GrVector2D operator*(double s, const GrVector2D & v) {
      return GrVector2D(s * v.x(), s * v.y());
    }
    const GrVector2D rotateRight() const {return GrVector2D(y(), -x());}
    const GrVector2D rotateLeft() const {return GrVector2D(-y(), x());}
    const GrVector2D rotate90(bool left) const {
      return (left?rotateLeft():rotateRight());
    }
    double length() const {return sqrt(e[0]*e[0]+e[1]*e[1]);}
    
    double normalize() {
      double l = length();
      *this = (1.f/l)*(*this );
      return l;
    }
};   /* XY */



class GrPoint2D : public GrPair2D {
  public:
    GrPoint2D() : GrPair2D() {}
    GrPoint2D(double x, double y) : GrPair2D(x, y) {}
    GrPoint2D(double *e) : GrPair2D(e) {}
    GrPoint2D(const double & d) : GrPair2D(d) {}
    GrPoint2D operator+( const GrVector2D & v2) const {
      return GrPoint2D(x() + v2.x(), y() + v2.y());
    }    
    const GrVector2D operator-(const GrPoint2D & p2) const {
      return GrVector2D(x() - p2.x(), y() - p2.y());
    }
    
};   /* XY */


inline unsigned int getAxis(char c) {
  return (c == 'x'?0:(c=='y'?1:2));
}

class GrVector3D : public GrTriple3D {

  public:
    
    GrVector3D() : GrTriple3D() {}
    GrVector3D(double x, double y, double z) : GrTriple3D(x,y,z) {}
    GrVector3D(double *e) : GrTriple3D(e) {}
    GrVector3D(float *e)  : GrTriple3D(e) {}

    double length() const {return sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);}
    double normalize();
    //transforms a vector by multiplying it to a matrix from the left
    GrVector3D transVec(const GrMatrix4x4 & mat) const {
      return mat*(*this);
    }
    GrVector3D transNorm(const GrMatrix4x4 & mat) const {
      return (*this)*mat;
    }
    
    // Addition/subtraction two vectors and returns the sum vector.
    const GrVector3D operator+(const GrVector3D & v2) const {
      return GrVector3D(x() + v2.x(), y() + v2.y(), z() + v2.z());
    }
    const GrVector3D operator+=(const GrVector3D & v2) {
      return *this = *this+v2;
    }
    double sqmag() {return *this^*this;}
    const GrVector3D operator-() const {return GrVector3D(-x(), -y(), -z());}
    const GrVector3D operator-(const GrVector3D &v2) const {
      return (*this+(-v2));
    }
    const GrVector3D operator-=(const GrVector3D & v2) {
      return *this = *this-v2;
    }

    
    // Addision/Subtraction of a vector and a scalar.
    const GrVector3D operator+(double s) const {
      GrVector3D tmp(*this);
      tmp.normalize();
      tmp *= s;
      return *this + tmp;
    }
    const GrVector3D operator-(double s) const {
      return *this + (-s);
    }
    const GrVector3D operator-=(double s) {
      return *this = *this-s;
    }
    const GrVector3D operator+=(double s) {
      return *this = *this+s;
    }



    // Multipiliacation/division of a vector and a scalar.
    friend const GrVector3D operator*(double s, const GrVector3D & v) {
      return GrVector3D(s * v.x(), s * v.y(), s * v.z());
    }
    const GrVector3D operator*=(double s) {return *this = s * *this;}

    // Cross prod
    const GrVector3D operator*(const GrVector3D &) const;
    
    const GrVector3D scalarMult(const GrVector3D & v) {
      return GrVector3D(x()*v.x(), y()*v.y(), z()*v.z());
    }

    void planeProject(const GrVector3D & normal) {
       *this -= (*this^normal)*normal;
    }

    //GrVector2D drop(unsigned int axis) const;
    const GrVector2D drop(unsigned int axis) const {
      //return GrVector2D(axis==0 ? y() : x() ,  axis<2 ? z() : y());
      return GrVector2D(getComponent((axis+1)%3), getComponent((axis+2)%3));
    }
    const GrVector2D drop(char c) const {
      return drop(getAxis(c));
    }
    const GrVector2D dropDominant() const {return drop(dominant());}
    
    
    //friend GrPoint3D operator+( const GrPoint3D &, const GrVector3D &);
    friend const GrVector3D operator*(const GrMatrix4x4 &, const GrVector3D &);
    friend const GrVector3D operator*(const GrVector3D &, const GrMatrix4x4 &);
};   /* XYZ */






class GrPoint3D  : public GrTriple3D  {
  public:
    
    GrPoint3D() : GrTriple3D() {}
    GrPoint3D(double x, double y, double z) : GrTriple3D(x,y,z) {}
    GrPoint3D(double *e) : GrTriple3D(e) {}
    GrPoint3D(float *e)  : GrTriple3D(e) {}
    

    const GrPoint3D operator+( const GrPoint3D & p2) const {
      return GrPoint3D(x() + p2.x(), y() + p2.y(), z() + p2.z());
    }
    const GrPoint3D operator+=( const GrPoint3D & p2) {
      return *this = *this + p2;
    }
    const GrVector3D operator-(const GrPoint3D & p2) const {
      return GrVector3D(x() - p2.x(), y() - p2.y(), z() - p2.z());
    }
    
/*
 * Add/Subtract a point and a vector. 
 */
    const GrPoint3D operator+( const GrVector3D & v2) const {
      return GrPoint3D(x() + v2.x(), y() + v2.y(), z() + v2.z());
    }    
    const GrPoint3D operator-( const GrVector3D & v2) const {
      return *this + (-v2);
    }
    const GrPoint3D operator+=( const GrVector3D & v2) {
      return *this = *this+v2;
    }
    const GrPoint3D operator-=( const GrVector3D & v2) {
      return *this = *this-v2;
    }

    friend const GrPoint3D operator*( double s, const GrPoint3D &);
    friend const GrPoint3D operator*(const GrMatrix4x4 &, const GrPoint3D &);
    
    const GrPoint3D midPoint( const GrPoint3D & p2) const {
      return GrPoint3D((x() + p2.x())/2, (y() + p2.y())/2, (z() + p2.z())/2);  
    }
    const GrPoint3D transPoint(const GrMatrix4x4 & mat) const {
      return mat*(*this);
    }
    const GrPoint2D drop(unsigned int axis) const {
      //return GrPoint2D(axis==0 ? y() : x() ,  axis<2 ? z() : y());
      return GrPoint2D(getComponent((axis+1)%3), getComponent((axis+2)%3));
    }
    const GrPoint2D drop(char c) const {
      return drop(getAxis(c));
    }
    const GrPoint2D dropDominant() const {return drop(dominant());}

};   /* XYZ */


class GrFrame3D {

    GrPoint3D  m_o;
    GrVector3D m_x;   
    GrVector3D m_y;
    GrVector3D m_z;
  public:
    
    
    
    const GrPoint3D o() const {return m_o;}
    void o(const GrPoint3D & o) {this->m_o = o;}
    
    const GrPoint3D operator^( const GrTriple3D & v2) const {
      return m_o + v2.x()*m_x + v2.y()*m_y + v2.z()*m_z;
    }
    GrFrame3D() {}
    GrFrame3D( GrPoint3D o, GrVector3D x, GrVector3D y, GrVector3D z ) : 
      m_o(o), m_x(x), m_y(y), m_z(z) {}
    const GrVector3D & getFrameComp(unsigned int component_index) {
      return *(&m_x+component_index);
    }
    
    void drawColoured();

};   /* XYZ */ 


inline ostream & operator << (ostream &os, const GrTriple3D & v) {
  os << '<' << v.x() << ',' << v.y() << ',' << v.z() << '>';
  return os;
}


inline ostream & operator << (ostream &os, const GrPair2D & v) {
  os << '<' << v.x() << ',' << v.y() << '>';
  return os;
}



inline istream & operator >> (istream &is, GrTuple & t) {
  
  for (unsigned int i=0; i < t.n; ++i) 
    is >> t.e[i];

  return is;
}




/*
 * Multiplies a point by a scaler
 */
inline const GrPoint3D operator*( double s, const GrPoint3D & p )
{
    return GrPoint3D(s * p.x(), s * p.y(), s * p.z());
}




extern GrColour operator*(GrColour c1, GrColour c2 );
extern GrColour operator+(GrColour c1, GrColour c2 );


/* colours */
extern GrColour    cMultAdd( GrColour c1, GrColour c2, GrColour c3);

/*other*/

extern const GrVector3D GrMkNormal(GrPoint3D p0, GrPoint3D p1, GrPoint3D p2);

inline GrVector3D GrMkNormalv(GrPoint3D * p) {
  return GrMkNormal(p[0], p[1], p[2]);
}

template <class T> static T operator%(const T & t, unsigned int p) {
  T ret;
  //*output << "t.n = " << t.n << endl;
  for (int i=0; i<t.n; ++i) {
    ret.e[i] = modd(t.e[i], p);
  }

  return ret;
}





class CVertex {
  public:
    GrPoint3D p;
    GrPoint2D tex;
};





class CGrRay3D {
    GrPoint3D m_pt;
    GrVector3D m_dir;
  public:
    CGrRay3D() {}
    CGrRay3D(GrPoint3D pt, GrVector3D dir) : m_pt(pt), m_dir(dir) {}
    const GrPoint3D & pt() const {return m_pt;}
    const GrVector3D & dir() const {return m_dir;}
    void pt(const GrPoint3D & p) {m_pt = p;}
    void dir(const GrVector3D & d) {m_dir = d;}
};



#endif /* _UTIL_ */


//
// @(#) $Log: util.h,v $
// @(#) Revision 1.45  2003/12/10 09:28:41  virzak
// @(#) added point+=point
// @(#)
// @(#) Revision 1.44  2003/12/05 18:47:56  virzak
// @(#) added functions
// @(#)
// @(#) Revision 1.43  2003/11/26 03:49:57  virzak
// @(#) Added function for configure script
// @(#)
// @(#) Revision 1.42  2003/10/21 05:50:58  virzak
// @(#) Added planeProject function
// @(#)
// @(#) Revision 1.41  2003/10/18 11:20:32  virzak
// @(#) Added vector +/- double
// @(#)
// @(#) Revision 1.40  2003/10/04 22:23:53  virzak
// @(#) Added 2d functions
// @(#)
// @(#) Revision 1.39  2003/10/03 19:23:45  virzak
// @(#) Added istream interface to Tuple
// @(#)
// @(#) Revision 1.38  2003/09/21 20:23:19  virzak
// @(#) Added default constructor for CGrRay3D and setting functions
// @(#)
// @(#) Revision 1.37  2003/09/21 06:40:36  virzak
// @(#) Added GrTuple::setComponent function
// @(#) Added getAxis function
// @(#)
// @(#) Revision 1.36  2003/09/17 03:48:14  virzak
// @(#) Fixed drop to produce components in the correct order
// @(#) Other small changes
// @(#)
// @(#) Revision 1.35  2003/09/11 17:40:21  virzak
// @(#) Added const everywhere a class is returned by value
// @(#)
// @(#) Revision 1.34  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.33  2003/09/09 23:38:31  virzak
// @(#) Rewrote Conversion Function to return const and be const function
// @(#)
// @(#) Revision 1.32  2003/09/04 02:04:49  virzak
// @(#) Provided ostream interface to GrTriple3D and GrPair2D
// @(#)
// @(#) Revision 1.31  2003/08/27 21:59:01  virzak
// @(#) Made all print funcitons const
// @(#)
// @(#) Revision 1.30  2003/08/26 03:23:57  virzak
// @(#) Added CRay class
// @(#)
// @(#) Revision 1.29  2003/08/25 20:01:01  virzak
// @(#) Moved deg2rad to the header from cpp of util
// @(#) Added 2D relating functions
// @(#)
// @(#) Revision 1.28  2003/08/24 20:03:31  virzak
// @(#) Small change: in GrTuple copy-constructor tuple size must be the same,
// @(#) so copying as many bites as in one type without check
// @(#)
// @(#) Revision 1.27  2003/08/21 06:01:58  virzak
// @(#) Small changes
// @(#)
// @(#) Revision 1.26  2003/08/20 02:45:00  virzak
// @(#) Added GrMaterial class.
// @(#) More Matrix routines.
// @(#) Added class GrFrame3D.
// @(#)
// @(#) Revision 1.25  2003/08/14 01:07:24  virzak
// @(#) Added mod operator to Tuple, which will  take an integer module
// @(#) and return the derived type.
// @(#)
// @(#) Revision 1.24  2003/08/08 20:23:16  virzak
// @(#) Inlined more functions
// @(#) Removed "inline" word within class declaration (inline by default)
// @(#)
// @(#) Revision 1.23  2003/08/05 18:12:19  virzak
// @(#) Changed M_PI to match the definition in math.h
// @(#)
// @(#) Revision 1.22  2003/07/28 03:45:27  virzak
// @(#) changed assignement operator to use a reference
// @(#)
// @(#) Revision 1.21  2003/07/27 03:13:53  virzak
// @(#) CC and AO in tuple are using the smallest array size while copying
// @(#)
// @(#) Revision 1.20  2003/07/27 02:57:18  virzak
// @(#) Added zero() function for tuple
// @(#)
// @(#) Revision 1.19  2003/07/27 02:31:49  virzak
// @(#) /Using memset where necessary
// @(#)
// @(#) Revision 1.18  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.17  2003/07/12 22:26:31  virzak
// @(#) Small fixes
// @(#)
// @(#) Revision 1.16  2003/07/11 23:22:15  virzak
// @(#) inline
// @(#)
// @(#) Revision 1.15  2003/07/11 23:20:59  virzak
// @(#) Added double * conversion to GrTuple and used it
// @(#)
// @(#) Revision 1.14  2003/07/11 23:00:25  virzak
// @(#) Added GrVector2D and implemented conversion from 3D
// @(#)
// @(#) Revision 1.13  2003/07/11 22:25:57  virzak
// @(#) Achieved real inheritance. GrTuple contains the array which is
// @(#) dynamically allocated.
// @(#)
// @(#) Revision 1.12  2003/07/11 19:20:34  virzak
// @(#) Developed inheritance more. Need to fix Tuple problem
// @(#)
// @(#) Revision 1.11  2003/07/11 03:35:01  virzak
// @(#) Constructed class GrTriple3D, which is a base class for GrPoint3D
// @(#) and GrVector3D
// @(#)
// @(#) Revision 1.10  2003/07/11 01:28:39  virzak
// @(#) added vector comparison
// @(#)
// @(#) Revision 1.9  2003/07/09 05:22:35  virzak
// @(#) Fixed the windows version
// @(#)
// @(#) Revision 1.8  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.7  2003/07/04 19:29:07  virzak
// @(#) Fixed a horrible bug that had operator- change *this
// @(#)
// @(#) Revision 1.6  2003/07/04 07:48:39  virzak
// @(#) Fixed functions by adding "const" storage directive
// @(#)
// @(#) Revision 1.5  2003/07/03 22:19:22  virzak
// @(#) Added magnitude function for vector
// @(#) Inlined more functions
// @(#)
//


