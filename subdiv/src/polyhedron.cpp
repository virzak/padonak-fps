#include "polyhedron.h"

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>



bool CPolyhedron::draw_points = 1;
bool CPolyhedron::draw_lines = 1;


void CPolyhedron::togglePoints() {
  draw_points = !draw_points;
}

void CPolyhedron::toggleLines() {
  draw_lines = !draw_lines;
}


inline void mkQuad(
    unsigned int * quad,
    unsigned int i0,
    unsigned int i1,
    unsigned int i2,
    unsigned int i3
    ) {
  quad[0] = i0;
  quad[1] = i1;
  quad[2] = i2;
  quad[3] = i3;

}

inline void mkTri(
    unsigned int * tri,
    unsigned int i0,
    unsigned int i1,
    unsigned int i2
    ) {
  tri[0] = i0;
  tri[1] = i1;
  tri[2] = i2;

}



void CPolyhedron::mkNormals() {
  unsigned int i;

  for (i=0; i<m_nr_polygons; i++) {
    
#ifdef __ICC
#pragma warning (disable:981)
#endif

    GrVector3D a = (m_points[m_poly[i][0]]-m_points[m_poly[i][1]]);
    GrVector3D b = (m_points[m_poly[i][0]]-m_points[m_poly[i][2]]);
    m_normal[i] = a*b; 
    
    m_normal[i].normalize();
  }
  
}






CPolyhedron & CPolyhedron::operator=(const CPolyhedron & p) {
  
  if (m_numPolysides != p.m_numPolysides) {
    cout << "ASSERTION in CPolyhedron::operator=(const CPolyhedron & p)\n";
    return *this;
  };
    
  if (m_nr_points != p.m_nr_points) {
    
    delete [] m_points;
    
    m_nr_points = p.m_nr_points;
    m_points = new GrPoint3D [m_nr_points];
  }

  if (m_nr_polygons != p.m_nr_polygons) {
    
    delete [] m_poly;
    delete [] m_normal;

    m_nr_polygons = p.m_nr_polygons;

    m_poly = new unsigned int * [m_nr_polygons];
    for (unsigned int i=0; i<m_nr_polygons; ++i) {
      m_poly[i] = new unsigned int [m_numPolysides];
    }
    
    m_normal = new GrVector3D [m_nr_polygons];

  }

  unsigned int i,j;
  for (i=0; i<m_nr_polygons; i++) {
    for (j=0; j<m_numPolysides; j++) {
      m_poly[i][j] = p.m_poly[i][j];
    }
    m_normal[i] = p.m_normal[i];
  }

  for (i=0; i<m_nr_points; i++) {
    m_points[i] = p.m_points[i];
  }
  
  return *this;
  
}



CPolyhedron::CPolyhedron(const CPolyhedron & p) : 
                m_numPolysides(p.m_numPolysides),
                m_nr_points(p.m_nr_points), 
                m_nr_polygons(p.m_nr_polygons) 
{
  unsigned int i,j;
  m_points = new GrPoint3D [m_nr_points];
  //m_poly = new GrQuad [m_nr_polygons];
  //
  m_poly = new unsigned int * [m_nr_polygons];
  for (i=0; i<m_nr_polygons; ++i) {
    m_poly[i] = new unsigned int [m_numPolysides];
  }

  m_normal = new GrVector3D [m_nr_polygons];

  for (i=0; i<m_nr_polygons; i++) {
    for (j=0; j<m_numPolysides; j++) {
      m_poly[i][j] = p.m_poly[i][j];
    }
    m_normal[i] = p.m_normal[i];
  }

  for (i=0; i<m_nr_points; i++) {
    m_points[i] = p.m_points[i];
  }
  
}


CPolyhedron::CPolyhedron(
    unsigned int polysides, 
    unsigned int nr_pts, 
    unsigned int nr_faces
  ) : 
    m_numPolysides(polysides), 
    m_nr_points(nr_pts), 
    m_nr_polygons(nr_faces) 
{
          
  m_points = new GrPoint3D [m_nr_points];
  //m_poly = new GrQuad [m_nr_polygons];
  //
  m_poly = new unsigned int * [m_nr_polygons];
  for (unsigned int i=0; i<m_nr_polygons; ++i) {
    m_poly[i] = new unsigned int [m_numPolysides];
  }

  m_normal = new GrVector3D [m_nr_polygons];
}

CPolyhedron::~CPolyhedron() {
  delete [] m_points;
  delete [] m_poly;
  delete [] m_normal;
}



CPolyhedron::CPolyhedron(E_PolyType type) : 
  m_numPolysides(type == poly_box?4:(type == tri_star?3:10))
{

  if (type == poly_box) {
    m_nr_points = 8;
    m_nr_polygons = 6;
  } else if (type == tri_star) {
    m_nr_points = 14;
    m_nr_polygons = 24;
  } else {
    fprintf (stderr, "Wrong type\n");
  }
    
  m_points = new GrPoint3D [m_nr_points];
  //m_poly = new GrQuad [m_nr_polygons];
  
  m_poly = new unsigned int * [m_nr_polygons];
  for (unsigned int i=0; i<m_nr_polygons; ++i) {
    m_poly[i] = new unsigned int [m_numPolysides];
  }
    
  m_normal = new GrVector3D [m_nr_polygons];

  if (type == poly_box || type == tri_star) {
    m_points[0] = GrPoint3D(-1.0, -1.0, -1.0);
    m_points[1] = GrPoint3D(-1.0, -1.0, 1.0);
    m_points[2] = GrPoint3D(1.0, -1.0, 1.0);
    m_points[3] = GrPoint3D(1.0, -1.0, -1.0);
    m_points[4] = GrPoint3D(1.0, 1.0, -1.0);
    m_points[5] = GrPoint3D(-1.0, 1.0, -1.0);
    m_points[6] = GrPoint3D(-1.0, 1.0, 1.0);
    m_points[7] = GrPoint3D(1.0, 1.0, 1.0);
  }

  if (type == tri_star) {
    m_points[ 8] = GrPoint3D( 0, 0, 1);
    m_points[ 9] = GrPoint3D( 0, 0,-1);
    m_points[10] = GrPoint3D( 0, 1, 0);
    m_points[11] = GrPoint3D( 0,-1, 0);
    m_points[12] = GrPoint3D( 1, 0, 0);
    m_points[13] = GrPoint3D(-1, 0, 0);
  }
  
  if (type == poly_box) {
    mkQuad( m_poly[0], 3, 2, 1, 0);
    mkQuad( m_poly[1], 5, 6, 7, 4);
    mkQuad( m_poly[2], 1, 2, 7, 6);
    mkQuad( m_poly[3], 5, 4, 3, 0);
    mkQuad( m_poly[4], 0, 1, 6, 5);
    mkQuad( m_poly[5], 4, 7, 2, 3);
  } else if (type == tri_star) {
    /*
    mkTri(m_poly[0], 0, 4, 2);
    mkTri(m_poly[1], 0, 2, 6);
    mkTri(m_poly[2], 0, 6, 4);
    mkTri(m_poly[3], 2, 4, 6);
    mkTri(m_poly[4], 1, 5, 3);
    mkTri(m_poly[5], 1, 3, 7);
    mkTri(m_poly[6], 1, 7, 5);
    mkTri(m_poly[7], 3, 5, 7);
    */
    
    //0
    mkTri(m_poly[0], 0, 9, 11);
    mkTri(m_poly[1], 0, 11, 13);
    mkTri(m_poly[2], 0, 13, 9);
    
    //1
    mkTri(m_poly[3], 1, 13, 11);
    mkTri(m_poly[4], 1, 11, 8);
    mkTri(m_poly[5], 1, 8, 13);

    //2
    mkTri(m_poly[6], 11, 12, 2);
    mkTri(m_poly[7], 11, 2, 8);
    mkTri(m_poly[8], 2, 12, 8);

    //3
    mkTri(m_poly[9], 11, 9, 3);
    mkTri(m_poly[10], 11, 3, 12);
    mkTri(m_poly[11], 3, 9, 12);

    //4
    mkTri(m_poly[12], 9, 4, 12);
    mkTri(m_poly[13], 9, 10, 4);
    mkTri(m_poly[14], 12, 4, 10);

    //5
    mkTri(m_poly[15], 13, 5, 9);
    mkTri(m_poly[16], 13, 10, 5);
    mkTri(m_poly[17], 9, 5, 10);

    //6
    mkTri(m_poly[18], 13, 8, 6);
    mkTri(m_poly[19], 13, 6, 10);
    mkTri(m_poly[20], 8, 10, 6);

    //7
    mkTri(m_poly[21], 8, 12, 7);
    mkTri(m_poly[22], 8, 7, 10);
    mkTri(m_poly[23], 12, 10, 7);
    
  }
  
  mkNormals();
}


void CPolyhedron::drawObject() const {
  unsigned int i, j, k;
  int vert_ind;
  GLenum gl_mode;
    
  if (draw_lines) glLineWidth(4.0);

  for (i=0; i<m_nr_polygons; i++) {
    
    GrVector3D normal = this->m_normal[i];
    for (k=0; k<(unsigned int)(draw_lines?2:1); k++) {
      if (!k) {
        glEnable(GL_LIGHTING);
        gl_mode = (m_numPolysides == 3) ? GL_TRIANGLES : GL_QUADS;
        glColor3f(0.0, 0.0, 1.0);
      } else {
        glDisable(GL_LIGHTING);
        gl_mode = GL_LINE_LOOP;
        glColor3f(1.0, 0.0, 0.0);
      }
      glBegin(gl_mode);

        glNormal3dv( normal );
    
        for (j=0; j<m_numPolysides; j++) {
          vert_ind = m_poly[i][j];
          glVertex3dv( m_points[vert_ind] );
          
        }
      glEnd();
    }
  }
  
  if (draw_points) {
    glPointSize(6.0);
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 1.0, 0.0);
    for (i=0; i<m_nr_points; i++) {
      glBegin(GL_POINTS);
      glVertex3dv( m_points[i] );
      glEnd();
    }
  }
}

