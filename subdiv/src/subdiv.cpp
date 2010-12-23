#include <map>

#include "subdiv.h"
#include "polyhedron.h"
#include "util.h"

using namespace::std;

//typedef unsigned int edge[2];

struct edge {
  unsigned int v1;
  unsigned int v2;
};

typedef map<unsigned int, edge, less<unsigned int> > midpts_map;
typedef pair<unsigned int, edge> mm_Pair;
static midpts_map mp_map;

static unsigned int addToMap(unsigned int newInd, edge e) {
  
  edge cur_e;
  
  midpts_map::const_iterator iter;
 
  
  for ( iter = mp_map.begin(); iter != mp_map.end(); ++iter ) {
    cur_e = iter->second;
    if ((cur_e.v1==e.v1) && (cur_e.v2==e.v2) || 
        (cur_e.v1==e.v2) && (cur_e.v2==e.v1)) {
      return iter->first;
    }
  } // for
  
  mp_map.insert(mm_Pair( newInd, e ));
  
    
  return 0;
}




void CPolyhedron::mkSubdivision() {


#if 0
  if (m_numPolysides == 4) {
  
  unsigned int i, j, pt_index, fc_index, vert_ind;

  edge e;

  unsigned int index_in_map;
  
  unsigned int nfi[5]; /*new face indices*/
  
  unsigned int nr_edges = m_nr_points + m_nr_polygons - 2;
    
  CPolyhedron *poly = new CPolyhedron 
    (m_numPolysides, m_nr_points + nr_edges + m_nr_polygons, 4*m_nr_polygons);
  
  mp_map.clear();

  /*
   * Subdivide Vertices
   */  
  
  pt_index = 0;
  fc_index = 0;
  for (i=0; i<m_nr_points; i++) {
    poly->m_points[pt_index] = m_points[i];
    pt_index++;
  }
  
  for (i=0; i<m_nr_polygons; i++) {
    GrPoint3D pt;
    for (j=0; j<4; j++) {

      vert_ind = m_poly[i][j];

      // Find the edge
      e.v1 = vert_ind;
      e.v2 = m_poly[i][(j+1)%4];
      
      // Add to the center point
      pt = pt+m_points[vert_ind];
      
      // Add to the list if needed
      if (!(index_in_map = addToMap(pt_index, e))) {
        poly->m_points[pt_index] = m_points[e.v1].midPoint(m_points[e.v2]);
        nfi[j] = pt_index;
        pt_index++;
      } else {
        nfi[j] = index_in_map;
      }
        
    }

    // Find the center point of the face
    pt = 0.25*pt;
        
    // Add the center point to the list
    poly->m_points[pt_index] = pt;
    nfi[4] = pt_index;
    pt_index++;
    
    /*
     * Subdivide Faces
     */
    for (j=0; j<4; j++) {
      poly->m_poly[fc_index][0] = m_poly[i][j];
      poly->m_poly[fc_index][1] = nfi[j];
      poly->m_poly[fc_index][2] = nfi[4];
      poly->m_poly[fc_index][3] = nfi[(j+3)%4];
      fc_index++;
    }
    
  }
  poly->mkNormals();


  //printf("We have %d vertices and %d points\n", poly->nr_points, pt_index);

  *this = *poly;
  
  delete poly;
  
  } else if (m_numPolysides == 3) {
#endif
    
    unsigned int i, j, pt_index, fc_index, vert_ind;

    edge e;

    unsigned int index_in_map;
  
    unsigned int nfi[5]; /*new face indices*/
  
    unsigned int nr_edges = m_nr_points + m_nr_polygons - 2;
    
    CPolyhedron *poly = new CPolyhedron 
      (m_numPolysides, 
          m_numPolysides == 4 ? 
          m_nr_points + nr_edges + m_nr_polygons : 
          m_nr_points + nr_edges, 
      4*m_nr_polygons);
  
    mp_map.clear();

    /*
     * Subdivide Vertices
     */  
  
    pt_index = 0;
    fc_index = 0;
    for (i=0; i<m_nr_points; i++) {
      poly->m_points[pt_index] = m_points[i];
      pt_index++;
    }
  
    for (i=0; i<m_nr_polygons; i++) {
      GrPoint3D pt;
      for (j=0; j<m_numPolysides; j++) {

        vert_ind = m_poly[i][j];

        // Find the edge
        e.v1 = vert_ind;
        e.v2 = m_poly[i][(j+1)%m_numPolysides];
      
        if (m_numPolysides == 4) {
          // Add to the center point
          pt = pt+m_points[vert_ind];
        }
      
        // Add to the list if needed
        if (!(index_in_map = addToMap(pt_index, e))) {
          poly->m_points[pt_index] = m_points[e.v1].midPoint(m_points[e.v2]);
          nfi[j] = pt_index;
          pt_index++;
        } else {
          nfi[j] = index_in_map;
        }
        
      }

      if (m_numPolysides == 4) {
        // Find the center point of the face
        pt = 0.25*pt;
        
        // Add the center point to the list
        poly->m_points[pt_index] = pt;
        nfi[4] = pt_index;
        pt_index++;
      }
    
      /*
      * Subdivide Faces
      */
      for (j=0; j<m_numPolysides; j++) {
        if (m_numPolysides == 4) {
          poly->m_poly[fc_index][0] = m_poly[i][j];
          poly->m_poly[fc_index][1] = nfi[j];
          poly->m_poly[fc_index][2] = nfi[4];
          poly->m_poly[fc_index][3] = nfi[(j+m_numPolysides-1)%m_numPolysides];
        } else {
          poly->m_poly[fc_index][0] = m_poly[i][j];
          poly->m_poly[fc_index][1] = nfi[j];
          poly->m_poly[fc_index][2] = nfi[(j+m_numPolysides-1)%m_numPolysides];
        }
        fc_index++;
      }
      
      if (m_numPolysides == 3) {
        poly->m_poly[fc_index][0]=nfi[0];
        poly->m_poly[fc_index][1]=nfi[1];
        poly->m_poly[fc_index][2]=nfi[2];
        fc_index++;
      }
    
    }
    poly->mkNormals();

    *this = *poly;
  
    delete poly;
  
//  }
  
}


/*
 * Allocates a new polygon and averages the source into this new polygon
 */
void CPolyhedron::mkAverage() {
  unsigned int i, j, pt_index;
    
  CPolyhedron *poly = new CPolyhedron (m_numPolysides, m_nr_points, m_nr_polygons);
  
  
  for (i=0; i<poly->m_nr_polygons; i++) {
    for (j=0; j<m_numPolysides; j++) {
      poly->m_poly[i][j] = m_poly[i][j];
    }
  }

  
  unsigned int * valence = new unsigned int [poly->m_nr_points];
  memset((void *) valence, 0, poly->m_nr_points*sizeof(unsigned int));
  


  /*
   * PASS 1: compute valence
   */
  for (i=0; i<poly->m_nr_polygons; ++i) {
    for (j=0; j<m_numPolysides; ++j) {
      valence[poly->m_poly[i][j]]++;
    }
  }
  

  /*
   * PASS 2: reposition vertex
   */
  for (i=0; i<poly->m_nr_polygons; ++i) {
    
    /* Find the center of the quad */
    GrPoint3D nonweighted_cent;
    GrPoint3D *weighted_cent = new GrPoint3D [m_numPolysides];
      
    for (j=0; j<m_numPolysides; j++) {
      pt_index = poly->m_poly[i][j];
      
      if (m_numPolysides == 4)
        nonweighted_cent += (1.f/(double)m_numPolysides)*m_points[pt_index];
      else
        for (unsigned int k=0; k<m_numPolysides; ++k) {
          weighted_cent[k] += (double)(((k==j)?2.f:3.f)/8.f)*m_points[pt_index];
        }
    }
    
    /* Add the centers to the points of new polygon */
    for (j=0; j<m_numPolysides; ++j) {
      pt_index = poly->m_poly[i][j];
      
      GrPoint3D center((m_numPolysides == 4)?nonweighted_cent:weighted_cent[j]);
        
      poly->m_points[pt_index] = 
        poly->m_points[pt_index] +((double) (1.0/valence[pt_index]))*center;
    }
    delete [] weighted_cent;
  }

  for (i=0; i<m_nr_points; ++i) {
    m_points[i] = poly->m_points[i];
  }
 
  mkNormals();
  delete poly;

  delete [] valence;
  
}

extern "C" char subdivFunctionForConfigSrcipt() {
  return '\0';
}

