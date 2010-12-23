#ifndef __PATCH_H__
#define __PATCH_H__

#ifdef _WIN32
#include "stdafx.h"
#endif

#include "util.h"
#include <GL/gl.h>

#ifdef __PATCH_EXTRA_INCLUDES__
__PATCH_EXTRA_INCLUDES__
#endif


class GrBPatch
#ifdef __PATCH_EXTRA_BASE_CLASSES__
__PATCH_EXTRA_BASE_CLASSES__
#endif
{

  public:

    /* View Options */
    enum views {
      E_VO_view_local_frame = 0,
      E_VO_net_and_curves,
      E_VO_greville_range,
      E_VO_view_parent_sfc, 
      E_VO_total
    };


    enum ChangeType {
      interactive = 0,
      automatic
    };


    enum CPGroup {
      E_GRP_border = 0,
      E_GRP_all
    };
    
    enum CPScheme {
      E_SCM_greville = 0,
      E_SCM_even
    };

  private:
    static bool m_view_elems[E_VO_total];
    static unsigned int m_approx;  
  

    typedef void (GrBPatch::*CtrlPtFunc)(unsigned int i, unsigned int j); 
    void forControlPoints(CtrlPtFunc ctrlPtFunc);
    
    double controlT(unsigned int dim, unsigned int n) const;
    void FillArray(double t, int near_point, int curve, int j,  int height,  int width, int fillsfc, int dim);
    void FillTree(int curve, int j, int fillsfc, int dim);
    void GenerateBCrvpts();
    void GenerateBSfcpts();
    void change_CP(unsigned int i, unsigned int j, CPScheme scm_type, double dist_between_cps[2]);
    void GenerateDisplacements();
    
    void GenerateParentInfo() {
      GenerateParentSfcPoints();
      GeneratePastedPoints();
    }
    
    void GeneratePastedPoints();

    void GenerateParentSfcPoints();
    GrPoint2D GetBaseDomain(unsigned int i1 = 0, unsigned int i2 = 0);

    void GenerateGrevillePoints();
    GrPoint2D getGrevilleAbscissa(unsigned int i, unsigned int j) const;
    double get_t_value(unsigned int dim, unsigned int j, unsigned int frac,unsigned  int * near_point) const;

    GrPoint3D getSurfacePoint(unsigned int s, unsigned int t) const;
    GrPoint2D getSurfaceTex(unsigned int s, unsigned int t) const;
    GrFrame3D getSurfacePointFrame(unsigned int s, unsigned int t) const;
    GrFrame3D getGrevilleFrame(unsigned int s, unsigned int t) const;
    int GenerateParentSurfaceFrame(GrPoint2D grev_pt, GrFrame3D *result);
  
    void drawBezTriangle(unsigned int curve, unsigned int j, char drawmode) const;
    GrVector3D GetNormal (unsigned int s, unsigned int t) const;
    void drawGrevPoint(unsigned int i, unsigned int j) const;

    

    unsigned int m_nr_points;     /* Number of points, used??? */
    GrPoint3D *m_points;

    unsigned int **m_point_array;

    GrPoint3D **m_greville_points;
#if 0
    GrPoint3D **m_greville_points_range;
#endif
    GrPoint3D **m_pasted_points;
    GrFrame3D **m_parent_sfc_frames;
    GrVector3D **m_displacement_vectors;
    
    GrPoint3D *****m_bez_crvpts; /*[2][curve][point][height][width]*/
    GrPoint3D *****m_bez_sfcpts;
    GrBPatch * m_pparent;
    GrBPatch * m_pchild;
    GrBPatch * m_pnext;
    unsigned int **m_selected;
    unsigned int m_dims[2];
    
    GrMatrix4x4	m_parent_trans;
    GrMatrix4x4	m_translate_trans;
    GrMatrix4x4	m_rotate_trans;
    GrMatrix4x4	m_scale_trans;

  public:


    static void toggleVE(views num) {
      m_view_elems[num] = !m_view_elems[num];
    }

    static void toggleApprox() {
      m_approx = (m_approx+1) % 3;
      printf("GrBPatch::m_approx = %d\n", m_approx);
    }


    GrBPatch(GrPoint3D * vertices, unsigned int nr_m, unsigned int nr_n, unsigned int ** point_array);
    GrBPatch(unsigned int, unsigned int);
    
    void constructor_common(GrPoint3D *, unsigned int, unsigned int, unsigned int **);
    ~GrBPatch();
    
    void GeneratePatch();
    void drawPatch(GLenum mode, unsigned int node_id, char drawmode='F') const;
    
    void applyTransToChildren(GrMatrix4x4 trans);
    void buildPatchMap() {
      m_parent_trans = m_translate_trans * m_scale_trans * m_rotate_trans;
    }

    void setParentTrans(const GrMatrix4x4 & trans) {
      m_parent_trans = trans;
    }


    void updateRotateTrans(const GrMatrix4x4 & trans) {
      m_rotate_trans = m_rotate_trans * trans;
    }
    
    void updateTranslateTrans(const GrMatrix4x4 & trans) {
      m_translate_trans = m_translate_trans * trans;
    }
    
    void updateScaleTrans(const GrMatrix4x4 & trans) {
      m_scale_trans = m_scale_trans * trans;
    }
    

    void setParent(GrBPatch * parent);
    void selectCP(unsigned int point);
  
    void selectCP(unsigned int i, unsigned int j) {
      m_selected[i][j] = !m_selected[i][j];
    }


    void change_b_patch (ChangeType change_type, CPScheme scm, CPGroup group, GrVector3D vec);
    void change_patch_CPs(GrVector3D vec);
    void change_CPs_scheme(CPScheme scm, CPGroup scm_group);
};

extern GrFrame3D  GetFrame(GrPoint3D ** s_tree, GrPoint3D ** t_tree);

#ifdef __cplusplus
extern "C" {
#endif

extern char patchFunctionForConfigSrcipt();
    
#ifdef __cplusplus
}
#endif


#endif  /* __PATCH_H__ */

