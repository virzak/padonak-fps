#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "patch.h"
#include "util.h"
#include "debug.h"

bool GrBPatch::m_view_elems[E_VO_total] = {false, false, false, false};
unsigned int GrBPatch::m_approx = 2;

static unsigned int dimensions = 2;

const unsigned int bsp_deg = 3;
const unsigned int FRAC = 9;

GrPoint3D GrBPatch::getSurfacePoint(unsigned int s, unsigned int t) const {
  ASSERT(s <= (m_dims[1]-bsp_deg)*FRAC)
  ASSERT(t <= (m_dims[0]-bsp_deg)*FRAC)
    
  return m_bez_sfcpts[0][s][t][bsp_deg][0];
}

GrPoint2D GrBPatch::getSurfaceTex(unsigned int s, unsigned int t) const {
  ASSERT(s <= (m_dims[1]-bsp_deg)*FRAC)
  ASSERT(t <= (m_dims[0]-bsp_deg)*FRAC)
    
  return GrPoint2D ( 
      ((double)s / (double)((m_dims[1]-bsp_deg)*FRAC)),  
      ((double)t / (double)((m_dims[0]-bsp_deg)*FRAC))
  );
}

GrFrame3D GrBPatch::getGrevilleFrame(unsigned int s, unsigned int t) const {
  
  GrVector3D v[3];
  GrPoint2D abscissa;
  
  ASSERT(s <= (m_dims[1]-bsp_deg)*FRAC)
  ASSERT(t <= (m_dims[0]-bsp_deg)*FRAC)
  
  v[0] = GrVector3D(1, 0, 0);
  v[1] = GrVector3D(0, 1, 0);
  v[2] = GrVector3D(0, 0, 1);
  
  abscissa = getGrevilleAbscissa(s, t);
  
  return GrFrame3D(GrPoint3D(abscissa.x(), 0, abscissa.y()), v[0], v[1], v[2]);

}


GrFrame3D GrBPatch::getSurfacePointFrame(unsigned int s, unsigned int t) const {
  ASSERT(s <= (m_dims[1]-bsp_deg)*FRAC)
  ASSERT(t <= (m_dims[0]-bsp_deg)*FRAC)
  
  return GetFrame(m_bez_sfcpts[0][s][t], m_bez_sfcpts[1][t][s]);
}

GrFrame3D GetFrame(GrPoint3D ** s_tree, GrPoint3D ** t_tree) {
  
  GrVector3D v[3];
  v[0] = s_tree[bsp_deg-1][1] - s_tree[bsp_deg-1][0];
  v[0].normalize();
  v[2] = t_tree[bsp_deg-1][1] - t_tree[bsp_deg-1][0];
  v[2].normalize();
  v[1] = v[2]*v[0];
  
  return GrFrame3D(t_tree[bsp_deg][0], v[0], v[1], v[2]);
}
// Are they the same???
GrVector3D GrBPatch::GetNormal (unsigned int s, unsigned int t) const {

  GrVector3D res;
  GrVector3D tangent[2];
  
  GrPoint3D * tree_level;

  tree_level = m_bez_sfcpts[0][s][t][bsp_deg-1];
  tangent[0] = tree_level[0] - tree_level[1];
  tree_level = m_bez_sfcpts[1][t][s][bsp_deg-1];
  tangent[1] = tree_level[0] - tree_level[1];

  res = (tangent[1] * tangent[0]);
  
  res.normalize();
  
  return res;
  
}




// GrBPatch::controlT
//
// Returns control points supporting full multiplicity at the edges
double GrBPatch::controlT(unsigned int dim, unsigned int n) const {

  double res;

  unsigned int total = m_dims[dim];

  ASSERT(n<total+bsp_deg-1)
  
  if (n<bsp_deg)
    res = 0;
  else if (n>=total)
    res = total-bsp_deg;
  else
    res = n-bsp_deg+1;

  return res;

}


#define GR_GREV_COMMENTS 0

/*
 * getGrevilleAbscissa - gets the Greville Abscissa by calculating the average 
 * of the t values.
 */
GrPoint2D GrBPatch::getGrevilleAbscissa(unsigned int s, unsigned int t) const {
  unsigned int i,dim;
  int ind[] = {s,t};
  double grev_ind[2];
  double ctrl_t;  

#if GR_GREV_COMMENTS
  printf("HERE (%d,%d (%d, %d)) -> ", s, t, m_dims[0], m_dims[1]);
#endif
  for (dim=0; dim<2; dim++) {
    grev_ind[dim] = 0;
#if GR_GREV_COMMENTS
    printf(" (");
#endif
    for (i=0; i<bsp_deg; i++) {
      ctrl_t = controlT(dim, ind[dim]+i);
      grev_ind[dim] += ctrl_t;
#if GR_GREV_COMMENTS
      printf("%f+", ctrl_t);
#endif
    }
#if GR_GREV_COMMENTS
    printf(") ");
#endif
  }
#if GR_GREV_COMMENTS
  printf(" -> (%f,%f)\n", grev_ind[0], grev_ind[1]);
#endif
  return GrPoint2D ((double) (grev_ind[0]/bsp_deg), 
      (double) (grev_ind[1]/bsp_deg));
}


double GrBPatch::get_t_value(unsigned int dim, unsigned int j, unsigned int frac, unsigned int * near_point) const {
  int near_j;
  double t;
  
  *near_point = (unsigned int) j/frac;
  near_j = *near_point * frac;
  if (*near_point > 0 && j == (unsigned int) near_j) {
    *near_point = *near_point-1;
    near_j = near_j-frac;
  }

  t = controlT(dim, *near_point+bsp_deg-1) + 
      (double) (j-near_j)/frac*
      (controlT(dim, *near_point+bsp_deg) - controlT(dim, *near_point+bsp_deg-1)); 
  
  return t;
  
}


inline void FillPoint(
    double t, 
    double a, 
    double b, 
    GrPoint3D left, 
    GrPoint3D right, 
    GrPoint3D *pt
) {
  
      if (a==b) {
        printf ("H\n");
      }
      
      *pt = 1.f/(b-a) *
          ((b-t)* left + (t-a)* right);
  
}




void GrBPatch::FillArray(
    double t,
    int near_point,
    int curve,
    int j, 
    int height, 
    int width,
    int fillsfc,
    int dim
  ) {

  double b=1;
  double a=0;
  /*double t;*/

  
  int i_a, i_b;

  
  GrPoint3D ****the_array;
  GrPoint3D **tree;

  if (fillsfc)   the_array = m_bez_sfcpts[dim];
  else           the_array = m_bez_crvpts[dim];


  
    if (height == 0) {
      the_array[curve][j][height][width] = 
        (fillsfc) ? 
        m_bez_crvpts[dim][near_point+width][curve][bsp_deg][0] : 
        (dim) ? m_pasted_points[near_point+width][curve]
              : m_pasted_points[curve][near_point+width]
        ;
    } else {

      

      
      i_a = near_point+width+height-1;
      i_b = near_point+width+bsp_deg;

      a = controlT((!fillsfc^dim), i_a);
      b = controlT((!fillsfc^dim), i_b);
      
      tree = the_array[curve][j];
      FillPoint(t, a, b, 
          tree[height-1][width], tree[height-1][width+1],
          &tree[height][width]);


    }

  return;
}




int GrBPatch::GenerateParentSurfaceFrame(GrPoint2D grev_pt, GrFrame3D *result) {

  unsigned int s, t;
   
#ifdef __ICC
#pragma warning (disable : 810)
#endif
  t = (unsigned int) (grev_pt.x()*FRAC+0.5);
  s = (unsigned int) (grev_pt.y()*FRAC+0.5);
#ifdef __ICC
#pragma warning (default : 810)
#endif

  if (!(s <= (m_dims[1]-bsp_deg)*FRAC && t <= (m_dims[0]-bsp_deg)*FRAC) ) {
    *result = GrFrame3D();
    return 1;
  }
 

  
  *result = getSurfacePointFrame(s, t);
  return 0;

}









void GrBPatch::FillTree(
    int curve,
    int j, 
    int fillsfc,
    int dim
) {
  
  unsigned int n_p;
  unsigned int height, width;
  
  double t = get_t_value((!fillsfc^dim), j, FRAC, &n_p);

  for (height=0; height<=bsp_deg; height++) {
    for (width=0; width<=bsp_deg-height; width++) {
      FillArray(t, n_p, curve, j, height, width, fillsfc, dim);
    }
  }
}


void GrBPatch::GenerateBCrvpts() {

  unsigned int curve, j;
  unsigned int dim = 0;
  
  for (dim = 0; dim < dimensions; dim++) {
    for (curve=0; curve<m_dims[dim]; curve++) {
      for (j=0; j<=(m_dims[!dim]-bsp_deg)*FRAC; j++) {
        FillTree(curve, j, 0, dim);
      }
    }
  }
  return;
}

/**********************************/
/* initial curve number = dims[0] */
/**********************************/

void GrBPatch::GenerateBSfcpts() {

  unsigned int t_val, j;
  unsigned int dim = 0;

  for (dim = 0; dim < dimensions; dim++) {
    for (t_val=0; t_val<=(m_dims[!dim]-bsp_deg)*FRAC; t_val++) {
      for (j=0; j<=(m_dims[dim]-bsp_deg)*FRAC; j++) {
        FillTree(t_val, j, 1, dim);
      }
    }
  }
  return;
}



void GrBPatch::GenerateDisplacements() {

  unsigned int i, j;

  for (i=0; i<m_dims[0]; i++) {
    for (j=0; j<m_dims[1]; j++) {
      m_displacement_vectors[i][j] = 
        m_points[m_point_array[i][j]] - m_greville_points[i][j];
    }
  }
  
  return;
}

void GrBPatch::GenerateGrevillePoints() {

  unsigned int i, j;
  GrPoint2D grev_pt;
  
  for (i=0; i<m_dims[0]; i++) {
    for (j=0; j<m_dims[1]; j++) {
      grev_pt = getGrevilleAbscissa(i,j);
      m_greville_points[i][j] = GrPoint3D(grev_pt.x(), 0, grev_pt.y());
      
    }
  }
}

#if 0
void GrBPatch::forControlPoints(CtrlPtFunc ctrlPtFunc) {

  unsigned int i, j;
  
  for (i=0; i<m_dims[0]; i++) {
    for (j=0; j<m_dims[1]; j++) {
      (this->*ctrlPtFunc)(i, j);
    }
  }
}
#endif


GrPoint2D GrBPatch::GetBaseDomain(unsigned int i1, unsigned int i2) {
  
  GrPoint3D grev_pt_3d;
  GrPoint3D grev_pt_3d_prime;
  
      /* Transform a point into the base domain */
      grev_pt_3d = m_greville_points[i1][i2];
      grev_pt_3d_prime = m_parent_trans * grev_pt_3d;
      return grev_pt_3d_prime.drop('y');

}


void GrBPatch::GenerateParentSfcPoints() {

  unsigned int i, j;
  GrPoint2D grev_pt_prime;
  int embedded = 1;

  
  /* check whether the feature domain is enclosed in the parent domain */
  /* Check only the corners */

  for (i=0; i<2; i++) {
    for (j=0; j<2; j++) {
      grev_pt_prime = GetBaseDomain(i*(m_dims[0]-1), j*(m_dims[1]-1));
      if (  m_pparent->GenerateParentSurfaceFrame(grev_pt_prime, 
            &m_parent_sfc_frames[i][j])      
          ) {
        embedded = 0;
      }
    }
  }

  
  for (i=0; i<m_dims[0]; i++) {
    for (j=0; j<m_dims[1]; j++) {
      if (embedded) {
        grev_pt_prime = GetBaseDomain(i, j);
        m_pparent->GenerateParentSurfaceFrame(grev_pt_prime, 
            &m_parent_sfc_frames[i][j]
        );
      } else {
        /* Why does this work? */
        grev_pt_prime = GetBaseDomain();
        

        m_parent_sfc_frames[i][j] = 
          GrFrame3D
            ( (m_greville_points[i][j]+ GrVector3D(grev_pt_prime.x(), 0, grev_pt_prime.y())), 
              GrVector3D(1, 0, 0), 
              GrVector3D(0, 1, 0), 
              GrVector3D(0, 0, 1)
          );

      }
      
    }
  }

  
}

void GrBPatch::GeneratePastedPoints() {

  unsigned int i, j;
  for (i=0; i<m_dims[0]; i++) {
    for (j=0; j<m_dims[1]; j++) {
      m_pasted_points[i][j] = 
        m_parent_sfc_frames[i][j] ^ m_displacement_vectors[i][j];
    }
  }
  
}



void GrBPatch::GeneratePatch() {
  GrBPatch *chld;
    
  if (m_pparent != NULL) {
    GenerateParentInfo();
  }
  GenerateBCrvpts();
  GenerateBSfcpts();  
  
  /* Update children */
  chld = m_pchild;
  while (chld != NULL) {
    chld->GeneratePatch();
    chld = chld->m_pnext;
  }
      
  
}








void GrBPatch::change_patch_CPs(GrVector3D vec) {
  unsigned int i, j;
  int regen = 0;
  
  for(i = 0; i < m_dims[0]; i++) {
    for(j = 0; j < m_dims[1]; j++) {
      if (m_selected[i][j]) {
        regen = 1;
        
        m_points[m_point_array[i][j]] += vec;
        m_pasted_points[i][j] += vec;

      }
    }
  }


  if (regen) {
    /* change this... to do only the selected points*/
    GenerateDisplacements();
    /*   */
    GeneratePatch();
  }
}


// Changes one control point with indices i and j

void GrBPatch::change_CP(
    unsigned int i, unsigned int j,
    CPScheme scm_type, double dist_between_cps[2]
) {
  
  if (scm_type == E_SCM_greville)
    m_pasted_points[i][j] = m_points[m_point_array[i][j]] = 
      m_greville_points[i][j];
  
  else
    m_pasted_points[i][j] = m_points[m_point_array[i][j]] = 
      GrPoint3D(dist_between_cps[0]*i, 0, dist_between_cps[1]*j);
        

}
  

void GrBPatch::change_CPs_scheme(CPScheme scm, CPGroup scm_group) {

  unsigned int dim, i, j;
  int s, t;
  double dist_between_cps[2];
  CPScheme scm_type = scm;


  dist_between_cps[0] = controlT(0, m_dims[0]-1)/(m_dims[0]-1);
  dist_between_cps[1] = controlT(1, m_dims[1]-1)/(m_dims[1]-1);
  

  if (scm_group == E_GRP_all) {
    /* All points */
    
    for (i=0; i<m_dims[0]; i++) {
      for (j=0; j<m_dims[1]; j++) {
        change_CP(i, j, scm_type, dist_between_cps);          
      }
    }
    
  } else {
    /* Border points only */
    
    for (dim=0; dim<2; dim++) {
      for (i=0; i<2; i++) {
        for (j=(dim?1:0); j<(dim?m_dims[dim]-1:m_dims[dim]); j++) {
          s= dim?i*(m_dims[!dim]-1):j;
          t= dim?j:i*(m_dims[!dim]-1);
          
          change_CP(s, t, scm_type, dist_between_cps);
            
        }
      }
    }
  }

  /* change this... to do only the selected points*/
   GenerateDisplacements();
  /*   */
  GeneratePatch();
 
}


void GrBPatch::change_b_patch (
    ChangeType change_type, 
    CPScheme scm, 
    CPGroup group, 
    GrVector3D vec
) { 
    if (change_type == interactive) {
      change_patch_CPs(vec);
    } else {
      change_CPs_scheme(scm, group);
    }
    
}



void GrBPatch::setParent(GrBPatch * parent) {
  
  GrBPatch * patch_ptr;

  m_pparent = parent;
  if (m_pparent->m_pchild == NULL) {
    m_pparent->m_pchild = this;
  } else {
      patch_ptr = m_pparent->m_pchild;
    for(;;) {
      if (patch_ptr->m_pnext == NULL) {
        patch_ptr->m_pnext = this;
        break;
      }
      patch_ptr = patch_ptr->m_pnext;
    }
  }
  
  GeneratePatch();
  
  
}






void GrBPatch::constructor_common (
    GrPoint3D * vertices, 
    unsigned int nr_m, 
    unsigned int nr_n, 
    unsigned int ** point_array  
    
) {
  
  unsigned int i, j, k, dim;
  
  m_dims[0] = nr_m;
  m_dims[1] = nr_n;

  m_points = new GrPoint3D [m_dims[0]*m_dims[1]];
  
  if (vertices) {
    for (i = 0; i<m_dims[0]*m_dims[1]; ++i) {
      m_points[i] = vertices[i];
    }
  }

  m_point_array = new unsigned int * [m_dims[0]];
  for (i = 0; i<m_dims[0]; ++i) {
    m_point_array[i] = new unsigned int [m_dims[1]];
    for (j = 0; j<m_dims[1]; ++j) {
      if (point_array) 
        m_point_array[i][j] = point_array[i][j];
      else
        m_point_array[i][j] = i*m_dims[1]+j;
    }
  }
  

  m_bez_crvpts = new GrPoint3D**** [2];
  m_bez_sfcpts = new GrPoint3D**** [2];

  
  for (dim = 0; dim < 2; dim++)  {
  
    m_bez_crvpts[dim] = new GrPoint3D *** [m_dims[dim]];
    for (i=0; i<m_dims[dim]; i++) {
      m_bez_crvpts[dim][i] = new GrPoint3D ** [(m_dims[!dim]-bsp_deg)*FRAC+1];
      for (j=0; j<=(m_dims[!dim]-bsp_deg)*FRAC; j++) {
        m_bez_crvpts[dim][i][j] = new GrPoint3D * [bsp_deg+1];
        for (k=0; k<=bsp_deg; k++) {
          m_bez_crvpts[dim][i][j][k]= new GrPoint3D [bsp_deg+1];
        }
      }
    }


    m_bez_sfcpts[dim] = new GrPoint3D *** [(m_dims[!dim]-bsp_deg)*FRAC+1];
    for (i=0; i<=(m_dims[!dim]-bsp_deg)*FRAC; i++) {
      m_bez_sfcpts[dim][i] = new GrPoint3D ** [(m_dims[dim]-bsp_deg)*FRAC+1];
      for (j=0; j<=(m_dims[dim]-bsp_deg)*FRAC; j++) {
        m_bez_sfcpts[dim][i][j] = new GrPoint3D * [bsp_deg+1];
        for (k=0; k<=bsp_deg; k++) {
          m_bez_sfcpts[dim][i][j][k]= new GrPoint3D [bsp_deg+1];
        }
      }
    }
  }
  

  /* Allocate space for selected points */
  m_selected = new unsigned int * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_selected[i] = new unsigned int [m_dims[1]];
    for(j = 0; j < m_dims[1]; j++) {
      m_selected[i][j] = 0;
    }    
  }


  /* Allocate space for greville points */
  m_greville_points = new GrPoint3D * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_greville_points[i] = new GrPoint3D [m_dims[1]];
  }

#if 0
  /* Allocate space for greville points range */
  m_greville_points_range = new GrPoint3D * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_greville_points_range[i] = new GrPoint3D [m_dims[1]];
  }
#endif


  /* Allocate space for pasted points */
  m_pasted_points = new GrPoint3D * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_pasted_points[i] = new GrPoint3D [m_dims[1]];
    /* // only need this if points aren't generated by change_b_patch later
    for(j = 0; j < m_dims[1]; j++) {
      m_pasted_points[i][j] = m_points[point_array[i][j]];
    } 
    */
  }

  /* Allocate space for parent surface points */
  m_parent_sfc_frames = new GrFrame3D * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_parent_sfc_frames[i] = new GrFrame3D [m_dims[1]];
  }
  
  /*Allocate space for displacement vectors at greville points*/  

  m_displacement_vectors = new GrVector3D * [m_dims[0]];
  for(i = 0; i < m_dims[0]; i++) {
    m_displacement_vectors[i] = new GrVector3D [m_dims[1]];
  }
  
  GenerateGrevillePoints();
  change_b_patch(automatic, E_SCM_greville, E_GRP_all, GrVector3D());
  GenerateDisplacements();
  GeneratePatch();

}



// Constructor

GrBPatch::GrBPatch (
    GrPoint3D * vertices, 
    unsigned int nr_m, 
    unsigned int nr_n, 
    unsigned int ** point_array
  ) : 
    //m_points(vertices), 
    //m_point_array(point_array), /* Maybe something better? */
    m_pparent(NULL), 
    m_pchild(NULL), 
    m_pnext(NULL)
{
  constructor_common(vertices, nr_m, nr_n, point_array);
}


// Constructor

GrBPatch::GrBPatch (
    unsigned int nr_m, 
    unsigned int nr_n 
  ) : 
    m_pparent(NULL), 
    m_pchild(NULL), 
    m_pnext(NULL)
{
  constructor_common(NULL, nr_m, nr_n, NULL);
}



// Destructor

GrBPatch::~GrBPatch() {
  unsigned int i, j, k, dim;
  
  for (dim = 0; dim < 2; dim++)  {
  
    for (i=0; i<m_dims[dim]; i++) {
      for (j=0; j<=(m_dims[!dim]-bsp_deg)*FRAC; j++) {
        for (k=0; k<=bsp_deg; k++) {
          delete [] m_bez_crvpts[dim][i][j][k];
        }
        delete [] m_bez_crvpts[dim][i][j];
      }
      delete [] m_bez_crvpts[dim][i];
    }
    delete [] m_bez_crvpts[dim];


    for (i=0; i<=(m_dims[!dim]-bsp_deg)*FRAC; i++) {
      for (j=0; j<=(m_dims[dim]-bsp_deg)*FRAC; j++) {
        for (k=0; k<=bsp_deg; k++) {
          delete [] m_bez_sfcpts[dim][i][j][k];
        }
        delete [] m_bez_sfcpts[dim][i][j];
      }
      delete [] m_bez_sfcpts[dim][i];
    }
    delete [] m_bez_sfcpts[dim];
  }
  
  delete [] m_bez_crvpts;
  delete [] m_bez_sfcpts;

  
  

  /* Deallocate space for selected points */
  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_selected[i];
  }
  delete [] m_selected;


  /* Deallocate space for greville points */
  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_greville_points[i];
  }
  delete [] m_greville_points;

#if 0
  /* Deallocate space for greville points range */
  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_greville_points_range[i];
  }
  delete [] m_greville_points_range;
#endif
  
  /* Deallocate space for pasted points */
  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_pasted_points[i];
  }
  delete [] m_pasted_points;

  /* Deallocate space for parent surface points */
  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_parent_sfc_frames[i];
  }
  delete [] m_parent_sfc_frames;
  
  /*Deallocate space for displacement vectors at greville points*/  

  for(i = 0; i < m_dims[0]; i++) {
    delete [] m_displacement_vectors[i];
  }
  delete [] m_displacement_vectors;
  
  for (unsigned int i = 0; i<m_dims[0]; ++i) {
    delete [] m_point_array[i];
  }
  delete [] m_point_array;

  delete [] m_points;
  

}


void GrBPatch::applyTransToChildren(GrMatrix4x4 trans) {
  GrBPatch * chld = m_pchild;
  while (chld != NULL) {
    chld->m_translate_trans=chld->m_translate_trans*trans;
    chld->buildPatchMap();
    chld->applyTransToChildren(trans);
    chld = chld->m_pnext;
  }
}





void GrBPatch::selectCP(unsigned int point) {
  
  unsigned int ind_i, ind_j;

  ind_i = (point/m_dims[1]);
  ind_j = point%m_dims[1];
  
  selectCP(ind_i, ind_j);
}














// DRAWING
//
//
/* drawBezTriangle - draws a triangle */

void GrBPatch::drawBezTriangle(unsigned int curve, unsigned int j, 
    char drawmode) const {
  
  GrPoint3D pts[4];
  GrPoint2D tex[4];
  GrVector3D normals[4];
  GrVector3D normal;
 
#ifdef __GR_SUCCESS__
    fprintf(stderr, "SUCCESS5\n");
#endif
  //(m_dims[1]-bsp_deg)
  pts[0] = getSurfacePoint(curve-1, j-1);
  pts[1] = getSurfacePoint(curve-1, j);
  pts[2] = getSurfacePoint(curve,   j-1);
  pts[3] = getSurfacePoint(curve,   j);

  tex[0] = getSurfaceTex(curve-1, j-1);
  tex[1] = getSurfaceTex(curve-1, j);
  tex[2] = getSurfaceTex(curve,   j-1);
  tex[3] = getSurfaceTex(curve,   j);
  
  
  normals[0] = GetNormal(curve-1, j-1);
  normals[1] = GetNormal(curve-1, j);
  normals[2] = GetNormal(curve, j-1);
  normals[3] = GetNormal(curve, j);
  
    
  if (drawmode == 'W') {
    glDisable(GL_LIGHTING);
  }
  
  glBegin((drawmode == 'W')? GL_LINE_LOOP:GL_TRIANGLES);
    if (m_approx == 0) {
      normal = ((pts[2]- pts[0])* (pts[1]- pts[0]));
      normal.normalize();
    } else if (m_approx == 1) {
      normal = (1.f/3.0) * (normals[0]+ normals[1]+ normals[2]);
    } else if (m_approx == 2) {
    } else {
      printf("ASSERTION\n");
    }
        
    glNormal3dv(normal);
    if (m_approx == 2) glNormal3dv(normals[0]);
    glVertex3dv(pts[0]);
    if (m_approx == 2) glNormal3dv(normals[1]);
    glVertex3dv(pts[1]);
    if (m_approx == 2) glNormal3dv(normals[2]);
    glVertex3dv(pts[2]);
  glEnd();
  if (drawmode != 'W') {
    glBegin(GL_TRIANGLES);
    
      normal = -normal;
      glNormal3dv(normal);
      
      if (m_approx == 2) {
        normal = -normals[2];
        glNormal3dv(normal);
      }
      glTexCoord2dv(tex[2]);
      glVertex3dv(pts[2]);
      
      if (m_approx == 2) {
        normal = -normals[1];
        glNormal3dv(normal);
      }
      glTexCoord2dv(tex[1]);
      glVertex3dv(pts[1]);
      
      if (m_approx == 2) {
        normal = -normals[0];
        glNormal3dv(normal);
      }
      glTexCoord2dv(tex[0]);
      glVertex3dv(pts[0]);
      
    glEnd();
  }
  
  glBegin((drawmode == 'W')? GL_LINE_LOOP:GL_TRIANGLES);
    if (m_approx)
      normal = ((pts[2]- pts[1])* (pts[3]- pts[1]));
    else
      normal = (1/3.0)*(normals[3]+ normals[1]+ normals[2]);
      
    normal.normalize();
    glNormal3dv(normal);
    if (m_approx == 2) glNormal3dv( normals[3]);
    glVertex3dv( pts[3]);
    if (m_approx == 2) glNormal3dv( normals[2]);
    glVertex3dv( pts[2]);
    if (m_approx == 2) glNormal3dv( normals[1]);
    glVertex3dv( pts[1]);
  glEnd();
  if (drawmode != 'W') {
    glBegin(GL_TRIANGLES);
      normal = -normal;
      glNormal3dv( (normal));

      if (m_approx == 2) {
        normal = -normals[1];
        glNormal3dv( normal);
      }
      glTexCoord2dv(tex[1]);
      glVertex3dv( (pts[1]));
      
      if (m_approx == 2) {
        normal = -normals[2];
        glNormal3dv( normal);
      }
      glTexCoord2dv(tex[2]);
      glVertex3dv( (pts[2]));
      
      if (m_approx == 2) {
        normal = -normals[3];
        glNormal3dv( normal);
      }
      glTexCoord2dv(tex[3]);
      glVertex3dv( (pts[3]));
      
    glEnd();
  }
  if (drawmode == 'W') glEnable(GL_LIGHTING);
          

}




/* drawGrevPoint - finds a Greville point and draws it */
void GrBPatch::drawGrevPoint(unsigned int i, unsigned int j) const {
  
  GrPoint2D grev;
  GrPoint3D grev_pt, pt,  sfc_pt;
          
  int axis;
  
  int s, t;
  

  glPointSize(15);
  
  

  grev_pt = m_greville_points[i][j];
  grev = grev_pt.drop('y');
  
#ifdef __ICC
#pragma warning (disable : 810)
#endif
  t = (unsigned int) (grev.x()*FRAC+0.5);
  s = (unsigned int) (grev.y()*FRAC+0.5);
#ifdef __ICC
#pragma warning (default : 810)
#endif
 
  
  sfc_pt = getSurfacePoint(s, t);
    
  
    
  if (m_view_elems[E_VO_greville_range]) {
    glBegin(GL_POINTS);
    glVertex3dv(sfc_pt);
    glEnd();
  }

  
  if (m_view_elems[E_VO_view_local_frame]) {
    glLineWidth(3);  
    for (axis=0; axis < 3; axis++) {
      glBegin(GL_LINE_STRIP);
      
      glVertex3dv(grev_pt);
      pt = grev_pt + GrVector3D((axis == 0)*0.2, (axis == 1)*0.2, (axis == 2)*0.2);
      
      glVertex3dv(pt);
      glEnd();
    }
    glLineWidth(1);
  }
}







void GrBPatch::drawPatch(GLenum mode, unsigned int node_id, char drawmode) const {
  unsigned int /*pindex, */i, j, k, curve;
  GrPoint3D crv_pt;
  GrBPatch * chld;
  
#ifdef __GR_COMMENTS__
    fprintf(stderr, "Drawing patch %s, %p\n", node->name, node);    
#endif

    if (drawmode == 'C' || drawmode == 'W') {
      
      glDisable(GL_LIGHTING);


      /* Points */
      for (i=0; i < m_dims[0]; i++) {
        for (j=0; j<m_dims[1]; j++) {

          glPointSize(8);
          
          if (mode == GL_SELECT) {
            glPushName(node_id+1+i*(m_dims[1])+j);
          }
          
          if (m_selected[i][j])
            glColor3f(0.0, 1.0, 0.0);
          else
            glColor3f(1.0, 0.0, 0.0);
          
          glBegin(GL_POINTS);
          glVertex3dv(m_pasted_points[i][j]);
          glEnd();


          

          if (m_view_elems[E_VO_greville_range] || 
              m_view_elems[E_VO_view_local_frame]) {
            
            if (m_selected[i][j])
              glColor3f(0.0, 1.0, 0.0);
            else
              glColor3f(0.5, 0.5, 0.5);
            
            drawGrevPoint(i, j);
            
          }


          
          if (mode == GL_SELECT) {
            glPopName();
          }

        }
      }

      
      if (m_view_elems[E_VO_view_parent_sfc]) {
        if ((chld = m_pchild) != NULL) {


          if (mode == GL_SELECT) {
            //FIXME: fix selectmode
            glPushName(node_id);
          }

          glColor3f(1.0, 1.0, 0.0);
          while (chld != NULL) {
            for (i=0; i < (chld->m_dims[0]); i++) {
              for (j=0; j<(chld->m_dims[1]); j++) {
                
                glLineWidth(3);
                
                m_pchild->m_parent_sfc_frames[i][j].drawColoured();
                
                glLineWidth(1);
              }
            }
            chld = chld->m_pnext;
          }

          if (mode == GL_SELECT) {
            glPopName();
          }

          
        }
      }
      
      
      if (m_view_elems[E_VO_net_and_curves]) {
        /*
         * Net and Curves
         */
        
        if (mode == GL_SELECT) {
          //FIXME: fix selectmode
          glPushName(node_id);
        }
        
        /* Control Net */
      
        glColor3f(0.0, 0.0, 0.0);
        for (k=0; k<2; k++) {
          for(i = 0; i < (m_dims[k?0:1]); i++)
          {
            glBegin(GL_LINE_STRIP);
            for(j = 0; j < (m_dims[k?1:0]);j++){
              glVertex3dv(m_pasted_points[k?i:j][k?j:i]);
            }
            glEnd();
          }
        }
        
  
        
        /*
        * Curves
        */
        
        glShadeModel(GL_FLAT);
  
        for (curve=0; curve<m_dims[0]; curve++) {
          glBegin(GL_LINE_STRIP);
          for (j=0; j<=(m_dims[1]-bsp_deg)*FRAC; j++) {
            glColor3d(j%2, (double) j/((m_dims[1]-bsp_deg)*FRAC), 0.0);
            crv_pt = m_bez_crvpts[0][curve][j][bsp_deg][0];
            glVertex3dv(crv_pt);
  
          }
          glEnd();
          
        }
        
        glShadeModel(GL_SMOOTH);
        
      
        if (mode == GL_SELECT) {
          glPopName();
        }
      }
      
      glEnable(GL_LIGHTING);

    }


    /*
     * Patch itself
     */
    

    if (mode == GL_SELECT) {
      glPushName(node_id);
    }
      
    for (curve=1;curve<=(m_dims[1]-bsp_deg)*FRAC;curve++) {
      for (j=1;   j<=(m_dims[0]-bsp_deg)*FRAC; j++) {
        drawBezTriangle(curve, j, drawmode);
      }
    }
    
    if (mode == GL_SELECT) {
      glPopName();
    }

    

}



//void patchFunctionForConfigSrcipt() {
//}
char patchFunctionForConfigSrcipt () {
  return 'r';
}

