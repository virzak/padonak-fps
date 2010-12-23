#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <values.h>
#include <GL/gl.h> 
#include <GL/glu.h> 

#include "util.h"
#include "gr.h"
#include "patch.h"
#include "trackball.h"
#include "pic.h"
#include "debug.h"
#include <math.h>
#include <signal.h>
#include <unistd.h>

/*#define __GR_SUCCESS__*/
/*#define __GR_COMMENTS__*/

/*========================================================================= 
 * Name-to-pointer hash tables 
 *=========================================================================*/

/* Tcl hash tables that maps names to clientdata */
Tcl_HashTable gGR_ModelTable;

/* Tcl hash tables that maps names to clientdata */
Tcl_HashTable gGR_MaterialTable;

/*========================================================================= 
 * variables to keep track of screen size
 *=========================================================================*/
static int scrWidth, scrHeight;

static int winX, winY;

static unsigned int number_of_nodes;
static char drawmode;
/*========================================================================= 
 * Types used internally
 *    Modify/extend these as you see fit.
 *=========================================================================*/

/*
 * Other Variables
 */

static GLuint displayList;
GrDagNode *rootNode;
static int drawCircle=0;


GrDagNode * nodeArr[200];




/*
 * newDagNode - creates a new node in the tree
 */
GrDagNode* newDagNode(
		      GrDagNode *parent, 
		      char *child_name
		      ) {
  GrDagNode* new_node;
  GrDagNode* child_node;

  /* Allocate space for the new node*/
  new_node = (GrDagNode *) malloc(sizeof(GrDagNode));

  /* write its name */
  if (!strcmp (child_name,":")) {
    new_node->name = strdup(child_name);
  }

  /* printf("id = %d   %s\n", number_of_nodes, child_name); */
  new_node->id = (number_of_nodes);
  number_of_nodes++;
  
  /* update a pointer to the parent and sibling */
  new_node->parent = parent;
  new_node->next = NULL;
  new_node->child = NULL;
  new_node->mat = NULL;

  /* set trans matrix to identity matrix */
  new_node->trans.initMatrix();

  if (strcmp (child_name,":") != 0) {
    if (parent == NULL ) {
      parent = rootNode;
    }
    if (parent->child == NULL) { 
      
      /* if parent had no children point to the new node*/
      parent->child = new_node;
      
    } else {
      
      /* otherwise find the last sibling and add the new node after it*/
      
      child_node = parent->child;
      while (child_node->next != NULL) {
	child_node = child_node->next;
      }
      child_node->next = new_node;
    }
  }

  /*
    fprintf(stderr, "newNode %s, 0x%08x\n", child_name, child_node->next);
  */
  return new_node;
}


void change_b_patches (
    GrDagNode* ptr, 
    GrBPatch::ChangeType change_type, 
    GrBPatch::CPScheme scm, 
    GrBPatch::CPGroup group, 
    GrVector3D vec
) { 
  
  GrDagNode *child, *instance; 
  
#ifdef __GR_COMMENTS__
  fprintf(stderr, "Traversing %s, %p\n", ptr->name, ptr);
#endif
  
  

  if (ptr->geom == INSTANCE) { 
    /*huynya*/
    instance = (GrDagNode *) ptr->child;
    change_b_patches(instance, change_type, scm, group, vec); 
    
  } else if (ptr->geom == B_PATCH) { 
    ptr->b_patch->change_b_patch (change_type, scm, group, vec);
  } 
  
  child = ptr->child; 
  
  while (child != NULL) { 
#ifdef __GR_COMMENTS__
    fprintf(stderr, "Traversing another chils of %s, %p\n", ptr->name, child);
#endif
    change_b_patches(child, change_type, scm, group, vec); 
    child = child->next; 
  } 
  
}





static void DrawPrimitive(GrGeometry fig, GrMaterial *mat, GrDagNode *node, GLenum mode) {
  
  GLfloat dif[4];
  GLfloat sp_c[4];


  if (mat != NULL) {
    
#ifdef __ICC
#pragma warning (disable : 810)
#endif
    dif[0] = mat->diffuse.r;
    dif[1] = mat->diffuse.g;
    dif[2] = mat->diffuse.b;
    dif[3] = 1.0;
    
    sp_c[0] = mat->specular_colour.r;
    sp_c[1] = mat->specular_colour.g;
    sp_c[2] = mat->specular_colour.b;
    sp_c[3] = 1.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif );
    glMaterialfv(GL_FRONT, GL_SPECULAR, sp_c );
    glMaterialf(GL_FRONT, GL_SHININESS, mat->specular_exp);
#ifdef __ICC
#pragma warning (default : 810)
#endif  
    glColor3dv(mat->diffuse);
  }


  if (fig == B_PATCH) {

    node->b_patch->drawPatch(mode, node->id, drawmode);
    
  }
}

/*-------------------------------------------------------------------------
 * gr_mode:
 *-------------------------------------------------------------------------*/
char*
gr_mode (
	char mode
) {
#ifdef __GR_DEBUG__
	fprintf(stderr,"Called: gr_mode(mode=\"%c\")\n",mode);
#endif
        drawmode = mode;
        
        return (char *)0;
}


#define HITS_COMMENTS 0

/*  processHits prints out the contents of the 
 *  selection array.
 */
static void processHits (GrDagNode** node, int *point, GLint hits, GLuint buffer[])
{
   unsigned int i, j;

   GLuint 
#if HITS_COMMENTS
     ii, jj,
#endif
    names, *ptr;
   
    int pick_index;
    
    
    double minZ = 1000, lastZ;
   
    *node = NULL;
    *point = -1;
#if HITS_COMMENTS
   fprintf (stderr, "hits = %d\n", hits);
#endif
   ptr = (GLuint *) buffer;
   for (i = 0; i < (unsigned int) hits; i++) {	/*  for each hit  */
      names = *ptr;
#if HITS_COMMENTS
      fprintf (stderr, " number of names for this hit = %d\n", names); 
#endif
      ptr++;
#if HITS_COMMENTS
      fprintf(stderr, "  z1 is %f;", (double) *ptr/0x7fffffff); 
#endif
      lastZ = (double) *ptr/0x7fffffff;
      ptr++;
#if HITS_COMMENTS
      fprintf(stderr, " z2 is %f\n", (double) *ptr/0x7fffffff); 
#endif
      ptr++;
#if HITS_COMMENTS
      fprintf (stderr, "   names are ");
#endif
      
      if (lastZ < minZ) {
        pick_index = (int)*(ptr+1);
        ASSERT(0<=pick_index && pick_index<(int) (number_of_nodes));
        if (pick_index != 0) {
          *node = nodeArr[pick_index];
          ASSERT(*node);
          if (*point == -1) {
            *point = pick_index - (*node)->id - 1;
          }
          minZ = lastZ;
        }
      }
      
      for (j = 0; j < names; j++) { /*  for each name */
#if HITS_COMMENTS
         printf ("%d ", *ptr);
         if (j == 0)  /*  set row and column  */
            ii = *ptr;
         else if (j == 1)
            jj = *ptr;
#endif
         ptr++;
      }
      
#if HITS_COMMENTS
      printf ("\n");
#endif
      /*board[ii][jj] = (board[ii][jj] + 1) % 3;*/
   }
}





static void Traverse (GrDagNode* ptr, GLenum mode) { 
  
  GrDagNode *child, *instance; 

#ifdef __GR_COMMENTS__
  fprintf(stderr, "Traversing %s, %p\n", ptr->name, ptr);
#endif
  
  
  glPushMatrix(); 
  
  //MultMatrix(ptr->trans);
  //glMultMatrixd(ptr->trans);
  ptr->trans.glMultMatrix();

  if (ptr->geom == INSTANCE) { 
    instance = (GrDagNode *) ptr->child;
    Traverse(instance, mode); 
    
  } else if (ptr->geom == SPHERE || ptr->geom == B_PATCH) { 
#ifdef __GR_COMMENTS__
    fprintf(stderr, "Drawing primitive %s, %p\n", ptr->name, ptr);    
#endif
    DrawPrimitive(ptr->geom, ptr->mat, ptr, mode); 
  } 
  
  child = ptr->child; 
  
  while (child != NULL) { 
#ifdef __GR_COMMENTS__
    fprintf(stderr, "Traversing another chils of %s, %p\n", ptr->name, child);
#endif
    Traverse(child, mode); 
    child = child->next; 
  } 
  
  glPopMatrix(); 
}



/*************************************************************************
 * Functions used to access and update the name-to-pointer hash tables
 **************************************************************************/

/* Add a new node name to the node name hash table.
 */
int 
Gr_addNode (
	char *parent_name, 
	char *node_name,    /* child part only */
	GrDagNode *node
) {
	Tcl_HashEntry *entry;
	int newEntry;
	char new_name[1024];

	/* Add the current node to the hash table 
	*/
	if (0 == strcmp(parent_name,":")) {
		sprintf(new_name, ":%s", node_name);
	} else {
		sprintf(new_name, "%s:%s", parent_name, node_name);
	}

	node->name = strdup(new_name);

	entry = Tcl_CreateHashEntry(&gGR_ModelTable,new_name,&newEntry);

	if (newEntry) { 
	  Tcl_SetHashValue(entry, node);
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "Gr_addNode: est' node %s %p !!!\n",new_name, node);
#endif
	} else {
	  fprintf(stderr, "Gr_addNode: PIZDA!!!\n");
	  return TCL_ERROR;
	}

	return TCL_OK;
}

/* Get node pointer from node name 
 */
int 
Gr_getNode (
	char *node_name, 
	GrDagNode **node
) {
  	Tcl_HashEntry *entry;
  
  	entry = Tcl_FindHashEntry(&gGR_ModelTable, node_name);
  	if (0 == entry) {
	  if(0 == strcmp(node_name, ":")) {
	    /* the root has been specified 
	     */
	    *node = 0;
	  } else {
	    return TCL_ERROR;
	  }
  	} else {
	  *node = (GrDagNode*)Tcl_GetHashValue(entry);
	}
	
  	return TCL_OK;
}

/* Add a new material name to the material name hash table.
 */
static int 
Gr_addMaterial (
	char *mat_name, 
	GrMaterial *mat
) {
	Tcl_HashEntry *entry;
	int newEntry;

	entry = Tcl_CreateHashEntry(&gGR_MaterialTable,mat_name,&newEntry);
	if (newEntry) {
		Tcl_SetHashValue(entry, mat);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

/* Get material pointer from material name 
 */
static int 
Gr_getMaterial (
	char *mat_name, 
	GrMaterial **mat
) {
	Tcl_HashEntry *entry;

	entry = Tcl_FindHashEntry(&gGR_MaterialTable, mat_name);
	if (0 == entry) {
		return TCL_ERROR;
	} else {
		*mat = (GrMaterial*)Tcl_GetHashValue(entry);
	}

	return TCL_OK;
}

/*========================================================================= 
 * Tcl/Gr commands:
 *    These commands change the internal state of Gr, but do not
 *    directly request any rendering (they can't, since they have no
 *    access to the Togl handle, and may be called before an OpenGL
 *    context is even available).
 *
 *    A pointer to a statically allocated string should be returned in
 *    case of an error, (char *)0 in case of success.
 *=========================================================================*/

/*-------------------------------------------------------------------------
 * gr_identity:
 *-------------------------------------------------------------------------*/
char*
gr_identity (
	char *node
) {  
        GrDagNode *dagNode;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_identity(node=\"%s\")\n",
		node
	);
#endif

	if (Gr_getNode(node, &dagNode) != TCL_OK) {
	  fprintf(stderr, "gr_identity: IDI v PIZDU\n");
	  return BAD_NODE;
	};

	if (dagNode == NULL) {
	  dagNode = rootNode;
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "Operation on Root Node\n");
#endif
	}

	dagNode->trans.initMatrix();

  	return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_rotate:
 *-------------------------------------------------------------------------*/
char*
gr_rotate (
	char *node, 
	char axis, 
	double angle,
        int patch_map
) {

        GrDagNode *dagNode;
        GrMatrix4x4 trans;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_rotate(node=\"%s\",axis='%c',angle=%f)\n",
		node,
		axis,
		angle
	);
#endif
	
        trans.rotationMatrix ( angle, axis);
	
	if (Gr_getNode(node, &dagNode) != TCL_OK) {
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "gr_rotate: IDI v PIZDU\n");
#endif
	  return BAD_NODE;
	};
	/*fprintf(stderr, "%s node address 0x%08x <--\n",node,dagNode);*/
	
	if (dagNode == NULL) {
	  dagNode = rootNode;
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "Operation on Root Node\n");
#endif
	}
	
        if (patch_map) {

          
          //dagNode->b_patch->rotate_trans = dagNode->b_patch->rotate_trans * trans;
          dagNode->b_patch->updateRotateTrans(trans);
          dagNode->b_patch->buildPatchMap();
          
          /*invMatrix(invtrans, trans, 2);
          
          applyTransToChildren(
              &dagNode->b_patch, 
              invtrans
          );
          */
          
          dagNode->b_patch->GeneratePatch();
        } else {
          dagNode->trans = dagNode->trans * trans;
        }

  	return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_translate:
 *-------------------------------------------------------------------------*/
char*
gr_translate (
	char *node, 
	GrVector3D vec,
        int patch_map
) {
        GrDagNode *dagNode;
        GrMatrix4x4 trans;
        GrMatrix4x4 invtrans;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_translate(node=\"%s\",vec=(%f %f %f))\n",
		node,
		vec.x, vec.y, vec.z
	);
#endif

	trans.translationMatrix( vec.x(), vec.y(), vec.z());
	
	if (Gr_getNode(node, &dagNode) != TCL_OK) {
	  fprintf(stderr, "gr_translate : IDI v PIZDU\n");
	  return BAD_NODE;
	};
	/*fprintf(stderr, "%s node address 0x%08x <--\n",node,dagNode);*/

	if (dagNode == NULL) {
	  dagNode = rootNode;
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "Operation on Root Node\n");
#endif
	}
        
        if (patch_map) {

          
          //dagNode->b_patch->translate_trans = dagNode->b_patch->translate_trans *trans;
          dagNode->b_patch->updateTranslateTrans(trans);
          


          dagNode->b_patch->buildPatchMap();
          
          invtrans = trans.invMatrix(0);
          
          dagNode->b_patch->applyTransToChildren(invtrans);
          
          dagNode->b_patch->GeneratePatch();
        } else {
          dagNode->trans = dagNode->trans * trans;
        }
		
  	return (char *)0;
}


/*-------------------------------------------------------------------------
 * gr_scale:
 *-------------------------------------------------------------------------*/
char*
gr_scale (
	char *node, 
	double factor[3],
        int patch_map
) {
        GrDagNode *dagNode;
        GrMatrix4x4 trans;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_scale(node=\"%s\",center=(%f %f %f),factors=(%f,%f,%f))\n",
		node,
		center.x, center.y, center.z,
		factor[0], factor[1], factor[2]
	);
#endif

        trans.scalingMatrix (factor[0], factor[1], factor[2]);
	
	if (Gr_getNode(node, &dagNode) != TCL_OK) {
	  fprintf(stderr, "gr_rotate: IDI v PIZDU\n");
	  return BAD_NODE;
	};
	/*fprintf(stderr, "%s node address 0x%08x <--\n",node,dagNode);*/
	
	if (dagNode == NULL) {
	  dagNode = rootNode;
#ifdef __GR_COMMENTS__
	  fprintf(stderr, "Operation on Root Node\n");
#endif
	}

        if (patch_map) {
          
          
          //dagNode->b_patch->scale_trans = dagNode->b_patch->scale_trans *trans;
          dagNode->b_patch->updateScaleTrans(trans);
          
          dagNode->b_patch->buildPatchMap();

          dagNode->b_patch->GeneratePatch();
        } else {
          dagNode->trans = dagNode->trans * trans;
        }
		
  	return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_surfaceproperty:
 *-------------------------------------------------------------------------*/
char*
gr_surfaceproperty (
	char *node, 
	char *mat
) {
        GrDagNode *dagNode;
	GrMaterial *material;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_surfaceproperty(node=\"%s\",mat=\"%s\")\n",
		node,mat
	);
#endif

	if (Gr_getNode(node, &dagNode) != TCL_OK) {
	  fprintf(stderr, "gr_surfaceproperty: node: IDI v PIZDU %s\n", node);
	  return BAD_NODE;
	};
	
	if (Gr_getMaterial(mat, &material) != TCL_OK) {
	  fprintf(stderr, "gr_surfaceproperty: mat: IDI v PIZDU\n");
	  return BAD_MATERIAL;
	};
	
	dagNode->mat = material;

        return (char *)0;
}

#define BUFSIZE 512

/*-------------------------------------------------------------------------
 * gr_pick:
 *   You actually have to render the scene with OpenGL here in "select"
 *   mode.   This will of course break if you have more than one Togl
 *   widget, but in this assignment you only have one, so it's OK; the
 *   active OpenGL context is always the right one.
 *-------------------------------------------------------------------------*/
char*
gr_pick (
	char *node, 
	GrPoint2D pos, 
	char **closest_node,
        int *point
) {

  
  double x = pos.x();
  double y = pos.y();
  GrDagNode* dagNode;
  /*char * res;*/
  
  /*int point;*/
 
  GLuint selectBuf[BUFSIZE];
  GLint hits=0;
  GLint viewport[4];

#ifdef __GR_COMMENTS__
  fprintf(stderr,
    "Called: gr_pick(node=\"%s\",pos=(%f,%f))\n",
    node,
    pos.x(), pos.y()
  );
#endif
  
  *closest_node = NULL;
   
  
  if(Gr_getNode(node,&dagNode)!=TCL_OK) 
    return "gr_pick: invalid root node";

  if (dagNode == NULL) 
    dagNode = rootNode;
  
  glSelectBuffer (BUFSIZE, selectBuf);
  (void) glRenderMode (GL_SELECT);
  
  glInitNames();
  glPushName(0);
  glGetIntegerv (GL_VIEWPORT, viewport);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();

  //  create 5x5 pixel picking region near cursor location    

  gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 
		 10.0, 10.0, viewport);

  glViewport(0, 0, scrWidth, scrHeight);
  gluPerspective(30.0, (GLfloat)scrWidth/(GLfloat)scrHeight, 0.1, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Traverse(rootNode, GL_SELECT);
  glPopMatrix ();

  glFlush ();
  
  glMatrixMode(GL_MODELVIEW);

  hits = glRenderMode (GL_RENDER);
  
  if (hits) {
    processHits (&dagNode, point, hits, selectBuf);

    if (!dagNode) return (char *)0;
    *closest_node=dagNode->name;
  }
  return (char *)0;

}

/*-------------------------------------------------------------------------
 * gr_reset:
 *-------------------------------------------------------------------------*/
char*
gr_reset()
{
#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_reset()\n"
	);
#endif
  	return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_addTransform:
 *-------------------------------------------------------------------------*/
char*
gr_addTransform (
	char *parent_name, 
	char *child_name
) {

        GrDagNode *node;
        GrDagNode *parent;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_addTransform(parent=\"%s\",child=\"%s\")\n",
		parent_name, child_name
	);
#endif

	if (Gr_getNode(parent_name, &parent) != TCL_OK) {
	  fprintf(stderr, "gr_addTransform: IDI v PIZDU\n");
	  return BAD_NODE;
	};

	node = newDagNode(parent, child_name);
	node->geom = EMPTY;
	Gr_addNode( parent_name, child_name, node);

        return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_addSphere
 *-------------------------------------------------------------------------*/
char*
gr_addSphere (
	char *parent_name, 
	char *child_name
) {

        GrDagNode *node;
        GrDagNode *parent;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_addSphere(parent=\"%s\",child=\"%s\") -->%d<--\n",
		parent_name, child_name, sizeof(GrDagNode)
	);
#endif

	Gr_getNode(parent_name, &parent);


	node = newDagNode(parent, child_name);
	node->geom = SPHERE;
        /*
	node->sphere.radius = 1;
        node->id = id++;
        */
        nodeArr[node->id] = node;

	Gr_addNode( parent_name, child_name, node);
	/*fprintf(stderr, "1 node address 0x%08x <--\n",node);*/
        
	
        return (char *)0;
}




char * gr_addPatch(char * parent_name, char * patch_name, 
          GrPoint3D * vertices, int nr_m, int nr_n, int ** point_array) {

  int i;
  GrDagNode *node;
  GrDagNode *parent;
  

  
#ifdef __GR_COMMENTS__
  printf("parameters: parent %s new name %s\n", parent_name, patch_name);
  printf("vertices:\n");
  for(i = 0; i < nr_vertices; i++)
    printf("{ %f %f %f }\n", vertices[i].x,vertices[i].y, vertices[i].z);

  printf("Dimensions: nr_m: %d, nr_n: %d\n", nr_m, nr_n);
  for(i = 0; i < nr_m; i++)
  {
    printf("array: ");
    for(j = 0; j < nr_n; j++)
      printf("%i ", point_array[i][j]);
    printf("\n");
  }
#endif

  Gr_getNode(parent_name, &parent);


  node = newDagNode(parent, patch_name);
  node->geom = B_PATCH;

  nodeArr[node->id] = node;
  /*printf("Allocating nodeArr[%d] -> patch\n", node->id);*/

  
  for (i=0; i< nr_m*nr_n; i++) {
    nodeArr[number_of_nodes+i] = node;
    /*printf("Allocating nodeArr[%d] -> patch\n", number_of_nodes+i);*/
  }
  
  number_of_nodes += nr_m*nr_n;
  
/*
  node->b_patch = new GrBPatch(
      vertices, (unsigned int) nr_m, (unsigned int) nr_n, 
      (unsigned int **) point_array
  );
*/
  
  node->b_patch = new GrBPatch(
      (unsigned int) nr_m, (unsigned int) nr_n);
  


  Gr_addNode( parent_name, patch_name, node);
  /*fprintf(stderr, "1 node address 0x%08x <--\n",node);*/
        
	
  return (char *) 0;
}








/*-------------------------------------------------------------------------
 * gr_mv_ctrl_pts:
 *-------------------------------------------------------------------------*/
char*
gr_mv_ctrl_pts (
	char *node, 
	GrVector3D vec
) {
        GrDagNode *dagNode;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_mv_ctrl_pts(node=\"%s\",vec=(%f %f %f))\n",
		node,
		vec.x, vec.y, vec.z
	);
        
#endif


	if (Gr_getNode(node, &dagNode) != TCL_OK) {
	  fprintf(stderr, "gr_mv_ctrl_pts: IDI v PIZDU\n");
	  return BAD_NODE;
	};


        if (!dagNode) dagNode = rootNode;
          
        change_b_patches(dagNode, GrBPatch::interactive, 
            GrBPatch::E_SCM_greville, GrBPatch::E_GRP_border, vec); 
        // Since changing interactively, 
        // arguments greville and E_GRP_border - don't matter
        
  	return (char *)0;
}





char *
gr_set_patch_parent(char *node, char *parent) {
  GrDagNode *dagNode;
  GrDagNode *par_dagNode;
  
  if (Gr_getNode(node, &dagNode) != TCL_OK) {
    fprintf(stderr, "gr_set_patch_parent: IDI v PIZDU %s\n", node);
    return BAD_NODE;
  };

  if (dagNode->geom != B_PATCH) {
    fprintf(stderr, "gr_set_patch_parent: not a patch %s\n", node);
    return BAD_NODE;
  }
  
  if (!strlen(parent)) {
    return (char *)0;
  }

  if (Gr_getNode(parent, &par_dagNode) != TCL_OK) {
    fprintf(stderr, "gr_set_patch_parent: IDI v PIZDU %s\n", parent);
    return BAD_NODE;
  };

  if (par_dagNode->geom != B_PATCH) {
    fprintf(stderr, "gr_set_patch_parent: not a patch %s\n", node);
    return BAD_NODE;
  }
  

  dagNode->b_patch->setParent(par_dagNode->b_patch);
  
  
  return (char *)0;
}






char *
gr_cp_scheme(char *node, int scheme, GrBPatch::CPGroup group) {
  
  GrDagNode *dagNode;
  /*GrBPatch * patch_ptr;*/
  
  GrBPatch::CPScheme scheme_1 = 
    (scheme == 0 ? GrBPatch::E_SCM_greville : GrBPatch::E_SCM_even);
    
  if (!strlen(node)) {
    dagNode = rootNode;
    change_b_patches(dagNode, GrBPatch::automatic, scheme_1, group, GrVector3D());
  } else {
    if (Gr_getNode(node, &dagNode) != TCL_OK) {
      fprintf(stderr, "gr_cp_scheme: IDI v PIZDU %s strlen=%d\n", 
          node, strlen(node));
      return BAD_NODE;
    };

    if (dagNode->geom != B_PATCH) {
      fprintf(stderr, "gr_cp_scheme: not a patch %s\n", node);
      return BAD_NODE;
    }
    dagNode->b_patch->change_b_patch(GrBPatch::automatic, scheme_1, group, GrVector3D());
  }


  /*GeneratePatch(&dagNode->b_patch);*/
  return (char *)0;

}





char *
gr_select_cp(char *node, int point) {
  
  GrDagNode *dagNode;
  
  if (Gr_getNode(node, &dagNode) != TCL_OK) {
    fprintf(stderr, "gr_select_cp: IDI v PIZDU %s strlen=%d\n", 
        node, strlen(node));
    return BAD_NODE;
  };

  if (dagNode->geom != B_PATCH) {
    fprintf(stderr, "gr_select_cp: not a patch %s\n", node);
    return BAD_NODE;
  }

  dagNode->b_patch->selectCP(point);

  /*GeneratePatch(&dagNode->b_patch);*/
  return (char *)0;

}










/*-------------------------------------------------------------------------
 * gr_material
 *-------------------------------------------------------------------------*/
char*
gr_material (
	char *mat, 
	GrColour diffuse, 
	GrColour specular,
	double spec_exp
) {
  GrMaterial * new_material;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_material(mat=\"%s\",diffuse=(%f,%f,%f),specular=(%f,%f,%f),spec_exp=%f)\n",
	 	mat, 
		diffuse.r, diffuse.g, diffuse.b, 
	 	specular.r, specular.g, specular.b, 
		spec_exp
	);
#endif

	  /* Allocate space for the new node*/
	new_material = (GrMaterial *) malloc(sizeof(GrMaterial));
	new_material->diffuse = diffuse;
	new_material->specular_colour = specular;
	new_material->specular_exp = spec_exp;

	Gr_addMaterial( mat, new_material );	
	
  	return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_trackball
 *-------------------------------------------------------------------------*/
char* 
gr_trackball (
	char *node, 
	GrPoint2D old_p, 
	GrPoint2D new_p, 
	double scale
) {
	GrDagNode* dagNode;
	double x, y, z;
	GrMatrix4x4 rot;

#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_trackball(node=\"%s\",old=(%f,%f),new=(%f,%f),scale=%f)\n",
	 	node, 
		old_p.x(), old_p.y(), 
	 	new_p.x(), new_p.y(), 
		scale
	);
#endif

	if (!strcmp(node, "")) {
	    drawCircle = !drawCircle;
	    return (char *)0;
	}

        old_p = GrPoint2D(scrWidth - old_p.x(), scrHeight - old_p.y());
        new_p = GrPoint2D(scrWidth - new_p.x(), scrHeight - new_p.y());
        
	
		

	if (Gr_getNode(node,&dagNode) != TCL_OK) {
	  return "gr_trackball: unknown node";
	}

	if (dagNode == NULL) dagNode = rootNode;
	
	vGenerateRotVec(old_p.x()/scale,old_p.y()/scale,&x,&y,&z, scrWidth, scrHeight);
	vGenerateRotVec(new_p.x()/scale,new_p.y()/scale,&x,&y,&z, scrWidth, scrHeight);
	vRotateScene(x,y,z,&rot);
        
	dagNode->trans = rot * dagNode->trans;
	


        return (char *)0;
}

/*-------------------------------------------------------------------------
 * gr_render
 *    NOTE!   Cannot actually DO rendering, can just store parameters.
 *    Script has to use "render" subcommand on togl widget to request a
 *    call to gr_display, a togl callback.  Basically, you need this
 *    procedure in Assignment 4, but it is not necessary for Assignment 3.
 *
 *    If you DID do OpenGL here, you could probably get it to work, but
 *    then you would have to bind your own <Expose> events, etc... better
 *    to let togl handle these.
 *-------------------------------------------------------------------------*/
#ifdef __ICC
#pragma warning (disable : 869)
#endif
char*
gr_render ( 
	char *root, 
	double size[2], 
	char *file_name, 
	GrPoint3D eye, 
	GrVector3D view,
	GrVector3D up, 
	double fov 
) {
#ifdef __GR_COMMENTS__
  	fprintf(stderr,
		"Called: gr_render(node=\"%s\",",
		root
	);
  	fprintf(stderr,
		"size=(%f,%f),",
		size[0],size[1]
	);
  	fprintf(stderr,
		"file=\"%s\",",
		file_name
	);
  	fprintf(stderr,
		"eye=(%f,%f,%f),",
		eye.x, eye.y, eye.z
	);
  	fprintf(stderr,
		"view=(%f,%f,%f),",
		view.x, view.y, view.z
	);
  	fprintf(stderr,
		"up=(%f,%f,%f))\n",
		up.x, up.y, up.z
	);
#endif


  	return (char *)0;
}
#ifdef __ICC
#pragma warning (default : 869)
#endif


/*-------------------------------------------------------------------------
 * gr_save:
 *   Save the current image to a file
 *-------------------------------------------------------------------------*/
char*
gr_save (
	char* filename
) {
  Pic*        pic;
  Pic_Pixel*  pixels;
  int         i, j;
  Pic_byte*   huy;

#ifdef __GR_DEBUG__
	fprintf(stderr,"Called: gr_save(filename=\"%s\")\n",filename);
#endif


#ifdef __ICC
#pragma warning (disable : 810)
#endif
  pic = PicOpen(filename,scrWidth,scrHeight);
#ifdef __ICC
#pragma warning (default : 810)
#endif


  /*4 bytes * 3 colors*/
  huy = (Pic_byte *)malloc(scrWidth*3);

  pixels = PixelAlloc(scrWidth);
/*  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, huy);*/
  for (i=scrHeight-1; i>=0; i--) {
    

    glReadPixels(0, i, scrWidth, 1, GL_RGB, GL_UNSIGNED_BYTE, huy);
  
    for (j=0; j<scrWidth; j++) {
/*      
      pixels[j].r = huy[(i)*width*3+(i)*2+j*3];
      pixels[j].g = huy[(i)*width*3+(i)*2+j*3+1];
      pixels[j].b = huy[(i)*width*3+(i)*2+j*3+2];
*/
      pixels[j].r = huy[j*3];
      pixels[j].g = huy[j*3+1];
      pixels[j].b = huy[j*3+2];

    }
    
    PicWriteLine(pic, pixels);
    
  }
  PixelFree(pixels);
  
  free(huy);
  
  PicClose(pic);
  return (char *)0;
}


static int frame = 0;
static int record = 0;

static char frame_file_base[30];

#ifdef __ICC
#pragma warning (disable : 869)
#endif
static void catch_function(int signal)
{
  char file[30];
  frame++;
  sprintf(file, "%s%d.png", frame_file_base, frame);
  gr_save(file);
}
#ifdef __ICC
#pragma warning (default : 869)
#endif


char*
gr_record(char* filename) {
  record = !record;
  printf("record %d\n", record);
  if (record) {
    strcpy(frame_file_base, filename);

#ifdef __ICC
#pragma warning (disable : 171)
#endif
    if(signal(SIGALRM, catch_function)==SIG_ERR)
     {
      printf("An error occured while setting a signal handler.\n");
      exit(0);
     }
#ifdef __ICC
#pragma warning (default : 810)
#endif
    
     ualarm (1, 33333);
    
  } else {
    *frame_file_base = '\0';
    ualarm (0, 0);
  }
  return (char *)0;
}


/*========================================================================= 
 * Togl callbacks:
 *    These functions are called by Togl when certain events (such as
 *    <Expose>) occur, or when requested using a togl subcommand (such as
 *    render).
 *=========================================================================*/

/*-------------------------------------------------------------------------
 * gr_reshape:
 *    When the display area changes size, recompute the perspective matrix
 *    and reload, maintaining the aspect ratio.
 *-------------------------------------------------------------------------*/
void
gr_reshape (
	struct Togl* togl
) {
	/* Get new size of display 
     */
  Tk_Window tkwin;
  
	scrWidth = Togl_Width(togl);
	scrHeight = Togl_Height(togl);

	/* Set up perspective projection, using current size and aspect
	 * ratio of display
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, scrWidth, scrHeight);
	gluPerspective(30.0, (GLfloat)scrWidth/(GLfloat)scrHeight, 0.1, 1000.0);

	/* Reset to modelview matrix mode to avoid confusion later
     */
	glMatrixMode(GL_MODELVIEW);

	tkwin = Togl_TkWin(togl);
	Tk_GetRootCoords(tkwin,&winX,&winY);

}

/*-------------------------------------------------------------------------
 * gr_display:
 *    Actually draw the scene using OpenGL, using the state previously set
 *    Can be explicitly requested using "render" subcommand to togl widget.
 *-------------------------------------------------------------------------*/
void
gr_display (
	struct Togl* togl
) {
  GrDagNode * node;
	double cosine, sine, radius;     // variables needed to draw circle 
	int i;

  Tk_Window tkwin;

  GLfloat no[] = {0,0,0,1};
	/* set up for perspective drawing 
     */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, scrWidth, scrHeight);
	gluPerspective(30.0, (GLfloat)scrWidth/(GLfloat)scrHeight, 0.1, 1000.0);

	/* change to model view for drawing
     */
	glMatrixMode(GL_MODELVIEW);

	/* Reset modelview matrix
     */
	glLoadIdentity();

	/* Clear framebuffer (both colour and depth buffers)
     */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set up light sources
     */
	/**** Not implemented ****/

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, no);
	glLightfv(GL_LIGHT0, GL_AMBIENT, no);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glMaterialfv(GL_FRONT, GL_EMISSION, no);
        glDisable(GL_COLOR_MATERIAL);

	/* Draw stuff
     */
	
	if (Gr_getNode(":", &node) == TCL_OK) {
	  if (node == NULL) {
	    Traverse(rootNode, GL_RENDER);
	  } else {
	    Traverse(node, GL_RENDER);
	  }

	} else {
	  fprintf(stderr, "IDI v PIZDU");
	}
	
	
	/* set up for orthogonal drawing to draw a circle on screen 
     */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, scrWidth, scrHeight);
	glOrtho(0.0, (float)scrWidth, 0.0, (float)scrHeight, -0.1, 0.1);

	/* change to model view for drawing
     */
	glMatrixMode(GL_MODELVIEW);

	/* Reset modelview matrix
     */
	glLoadIdentity();

	/* draw a circle for use with the trackball 
     */
	if (drawCircle) {
	  glEnable(GL_COLOR_MATERIAL);
	  glDisable(GL_LIGHTING);
	  glColor3f(1.0, 1.0, 1.0);
	  radius = scrWidth < scrHeight ? (float)scrWidth * 0.25 : (float)scrHeight * 0.25;
	  glTranslated((float)scrWidth * 0.5, (float)scrHeight * 0.5, 0);
	  glBegin(GL_LINE_LOOP);
	  for (i=0; i<40; i++) {
	    cosine = radius * cos(i*2*M_PI/40);
	    sine = radius * sin(i*2*M_PI/40);
	    glVertex2d(cosine, sine);
	    /*glVertex2f(0.1, 100);*/
	  }
	  glEnd();
	  glEnable(GL_LIGHTING);
	  glDisable(GL_COLOR_MATERIAL);
	}

	/* Force OpenGL drawing to completion and swap buffers
     */
	Togl_SwapBuffers(togl);

	tkwin = Togl_TkWin(togl);
	Tk_GetRootCoords(tkwin,&winX,&winY);

}

/*-------------------------------------------------------------------------
 * gr_initialize:
 *    Once togl widget created, intialize OpenGL context and gr state to
 *    values we want
 *-------------------------------------------------------------------------*/
#ifdef __ICC
#pragma warning (disable : 869)
#endif
void 
gr_initialize (
	struct Togl* togl
) {
  GLUquadricObj *qobj;
  GLfloat light_position[] = {0.0,0.0,-10.0,0.0};
        /* Turn on Gouraud shading
     */
	glShadeModel(GL_SMOOTH);

    number_of_nodes = 1; // First node is reserved for picking nothing
        
	/* Create a black background when clear colour buffers
     */
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearColor(0.0, 0.5, 0.5, 1.0);

	/* Enable depth test
     */
	glEnable(GL_DEPTH_TEST);



        glEnable(GL_COLOR_MATERIAL);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        
        

	rootNode = newDagNode(NULL,":");
	rootNode->geom = EMPTY;

	/* Initialize the sphere display list
     */

	

	displayList = glGenLists(1);	
	glNewList(displayList, GL_COMPILE);

		qobj = gluNewQuadric();	
		gluQuadricDrawStyle(qobj, GLU_FILL);
		/*gluQuadricDrawStyle(qobj, GLU_LINE);*/
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluSphere(qobj, 1.0, 30, 30);
	

	glEndList();


}
#ifdef __ICC
#pragma warning (default : 869)
#endif

