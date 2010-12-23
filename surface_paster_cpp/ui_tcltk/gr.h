/*========================================================================= 
 *                                 CS 488/688                                
 *                          Introduction to Graphics                        
 *                                                                         
 *                   Assignment 3: Hierarchical Transformations
 *                                                                       
 *=========================================================================*/
#ifndef _GR_
#define _GR_

#define __GR_NO_APPROX__

#define BAD_NODE "bad_node"
#define BAD_MATERIAL "bad_material"

/* Also includes Tcl/Tk, GL/gl.h, X11/Xlib stuff */
#include "togl.h"
#include "util.h"
#include "patch.h"


/* Scene graph node types.
 *   In A3, you will only need to support the EMPTY and SPHERE; the 
 *   others will be used for A4 
 */
typedef enum { 
  EMPTY, 
  CUBE, 
  BOX, 
  SPHERE,
  B_PATCH, 
  POLYHEDRON, 
  INSTANCE, 
  ROOT
} GrGeometry;


/* Scene graph node.
 * Feel free to add or remove variables from this structure.   Note 
 * that a parent only points to its first child, the rest are found
 * by sibling pointers.   To represent non-tree links (links to
 * shared nodes) an INSTANCE node type should be used.
 */
class GrDagNode {
  public:
    GrDagNode* parent;
    GrDagNode* next;
    GrDagNode* child;
    GrGeometry geom;   /* how to interpret the following field */

    GrBPatch *b_patch;
      
    GrMaterial*	mat;
    GrMatrix4x4	trans;

    /* The inverse transform is not needed for A3, but it will be needed
     * for A4 
     */
    GrMatrix4x4	invtrans;

    /* The following two fields can be used to assist with picking
     * in A3; they aren't needed for A4 
     */
    unsigned int id;
    char* name;
} ;


extern Tcl_HashTable gGR_ModelTable;
extern Tcl_HashTable gGR_MaterialTable;

/* Gr commands 
 *
 * A pointer to a (statically allocated) error message should be
 * returned explaining any errors; a NULL return indicates success.
 *
 * These functions don't have access to OpenGL or Togl functions, 
 * since they are not guaranteed to be called in an OpenGL context.   
 * They should just change some state stored in the Gr module
 * for later interpretation during the gr_display callback.  In
 * particular, gr_render should just store its data, and the Tcl
 * script should post a redisplay when it wants the scene to be
 * redrawn.
 */
extern char* gr_identity (char *node);
extern char* gr_rotate(char *node, char axis, double angle, int patch_map);
extern char* gr_translate(char *node, GrVector3D trans, int patch_map);
extern char* gr_scale(char *node, double factor[3], int patch_map);

extern char* gr_surfaceproperty(char *node, char *mat);
extern char* gr_material(char *mat, GrColour d, GrColour s, double s_exp);

extern char* gr_reset(void);
extern char* gr_render(char *root, double size[2], char *file_name, 
		               GrPoint3D eye, GrVector3D view, GrVector3D up, 
                       double fov); 
extern char* gr_pick(char *root, GrPoint2D pos, char** name, int * point /* returned */);
extern char* gr_trackball(char *node, GrPoint2D start, GrPoint2D end, 
                          double scale);

extern char* gr_addTransform(char *parent, char *name);
extern char* gr_addSphere(char *parent, char *name);

extern char* gr_save (char* filename);
extern char* gr_record(char* filename);
extern char* gr_mode (char mode);

extern char* gr_set_patch_parent(char *node, char *parent);
extern char* gr_cp_scheme(char *node, int scheme, GrBPatch::CPGroup);
extern char* gr_select_cp(char *node, int point);

/* Togl callbacks 
 */
extern void gr_initialize(struct Togl* togl);
extern void gr_reshape(struct Togl* togl);
extern void gr_display(struct Togl* togl);





extern GrDagNode* newDagNode(
		      GrDagNode *parent, 
		      char *child_name
		      );


extern int 
Gr_addNode (
	char *parent_name, 
	char *node_name,    /* child part only */
	GrDagNode *node
);


extern int 
Gr_getNode (
	char *node_name, 
	GrDagNode **node
);


extern GrDagNode * nodeArr[200];
extern GrDagNode *rootNode;

extern char *     gr_addPatch(char * parent_name, char * patch_name,
                              GrPoint3D * vertices, 
                              int nr_m, int nr_n, int ** point_array);

extern char *     gr_mv_ctrl_pts (	char *node, 	GrVector3D vec);

extern void change_b_patches (
    GrDagNode* ptr, 
    GrBPatch::ChangeType change_type, 
    GrBPatch::CPScheme scm, 
    GrBPatch::CPGroup group, 
    GrVector3D vec); 


#endif /* _GR_ */

