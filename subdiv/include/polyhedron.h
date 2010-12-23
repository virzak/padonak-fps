#ifndef __POLYHEDRON_H__
#define __POLYHEDRON_H__

#include "util.h"


extern void togglePoints();
extern void toggleLines();

class CPolyhedron {

  public:
    enum E_PolyType {
      poly_box,
      tri_star
    };

  protected:
    const unsigned int m_numPolysides;
    unsigned int m_nr_points;     /* Number of points */
    GrPoint3D *m_points;
    unsigned int m_nr_polygons;     /* Number of polygons */
    unsigned int ** m_poly;
    GrVector3D *m_normal;
    static bool draw_points;
    static bool draw_lines;

  public:
    CPolyhedron(unsigned int, unsigned int, unsigned int);
    CPolyhedron(const CPolyhedron & p);
    CPolyhedron & operator=(const CPolyhedron & p);
    CPolyhedron(E_PolyType type);
    ~CPolyhedron();
    void mkNormals();
    void mkAverage();
    void mkSubdivision();
    void drawObject() const;
    static void togglePoints();
    static void toggleLines();
};

class CQuadPolyhedron : public CPolyhedron {
  CQuadPolyhedron(unsigned int pts, unsigned int poly) : 
    CPolyhedron(4, pts, poly) {}
};

class CTriPolyhedron : public CPolyhedron {
  CTriPolyhedron(unsigned int pts, unsigned int poly) : 
    CPolyhedron(3, pts, poly) {}
  CTriPolyhedron();
};



#endif /*__POLYHEDRON_H__*/

