/***************************************************************************
 * CS488/688 Assignment 3 --- Hierarchical Transformations
 *
 * grsh.c:
 *    Implement a "gr" shell that interprets Tcl/Tk with a few added
 *    commands adequate to implement assignment 1.  See the assignment
 *    spec for more information.
 ***************************************************************************/
#include <stdlib.h>
#include <string.h>

/* gr includes togl.h which then includes tcl.h, tk.h, X11/Xlib.h, GL/gl.h */
#include "gr.h"
#include "patch.h"

/* for manipulating points, vectors, colours, etc */
#include "util.h"

/***************************************************************************
 * Helper routines 
 ***************************************************************************/

/* Parse an n-tuple of doubles specified as a tcl-list.
 * Used for grabbing point or vector coordinates, colours, and other things.
 * Puts results into an array of doubles.
 */
static int 
gr_getTuple (
	Tcl_Interp *ip, 
	char *inList, 
	double *p,    /* pointer to array */
    int n         /* number of elements to expect */
) {
	char **indices;
	int num_doubles;
	int rtn;
	char s[100];
	int i;

	rtn = Tcl_SplitList(ip, inList, &num_doubles, &indices);

	if ((TCL_OK != rtn) || (n != num_doubles)) {
		sprintf(s,"%d",n);
		Tcl_AppendResult(ip, 
			 "Expected a tuple of ", s, " doubles.\n",
			 (char *) 0
		);
		Tcl_Free((char *)indices);
		return TCL_ERROR;
	}

	for (i = 0; i < n; i++) {
		if (TCL_OK != Tcl_GetDouble(ip, indices[i], &(p[i]))) {
			Tcl_Free((char *)indices);
			sprintf(s,"%d",n);
			Tcl_AppendResult(ip, 
			 	"Expected a tuple of ", s, " doubles.\n",
			 	(char *) 0
			);
			return TCL_ERROR;
		}
	}
	Tcl_Free((char *)indices);
	return TCL_OK;

}


static GrColour
Gr_mkColour (
	double *c
) {
	return GrColour(c[0],c[1],c[2]);
}

/***************************************************************************
 * The Tcl glue for the callbacks requested by the assignment specification.
 * 
 * While some error checking is performed by these functions, the
 * error messages are not as precise as they might be. This is done
 * to keep the code simple.   However, if a single ? is given as an
 * argument, a usage message will be printed.  Also, the corresponding
 * gr_* function can return a pointer to a (statically allocated) string
 * to describe an internal error; NULL should be returned for success.
 *
 * Feel free to add better error messages.
 ***************************************************************************/
#ifdef __ICC
#pragma warning (disable : 869)
#endif

static void
Gr_identityCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename>\n",
		"Purpose:\n",
        "  Reset the given node's transformation to the identity.\n",
		(char *)0 
	);
}

static int 
Gr_identityCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char *errmsg;

    char *nodename;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_identityCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (2 != argc) {
		Gr_identityCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    /* obtain the nodename */
	nodename = argv[1];
    
	/* execute gr function */
	errmsg = gr_identity(nodename);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_identityCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}

static void
Gr_rotateCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <axisname:[xyz]> <angle:degrees>\n",
		"Purpose:\n",
        "   Postmultiply a rotation transformation to the given node's\n",
        "   transformation.  The rotation should by the given angle\n",
        "   about the given axis.\n",
		(char *)0 
	);
}

static int 
Gr_rotateCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

    char *nodename;
    char axis;
    double angle ;
    int patch_map=0;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_rotateCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (4 != argc && 5 != argc) {
		Gr_rotateCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    if (5 == argc) {
      patch_map = atoi(argv[4]);
    }
    
    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the axis */
    axis = argv[2][0];
    if ('x' != axis && 'y' != axis && 'z' != axis) {
		Gr_rotateCMD_Usage(ip,argc,argv);
    	return TCL_ERROR;
	}
    
    /* obtain the angle */
    if (TCL_OK != Tcl_GetDouble(ip, argv[3], &angle)) {
		Gr_rotateCMD_Usage(ip,argc,argv);
    	return TCL_ERROR;
	}
    
	/* execute gr function */
	errmsg = gr_rotate(nodename,axis,angle, patch_map);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_rotateCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}

static void
Gr_translateCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <translation:{x y z}>\n",
		"Purpose:\n",
        "   Postmultiply a translation transformation to the given node's\n",
        "   transformation.  The translation will by the vector.\n",
		(char *)0 
	);
}

static int 
Gr_translateCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

    char *nodename;
    double vec[3];
    int patch_map=0;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_translateCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc && 4 != argc) {
		Gr_translateCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    if (4 == argc) {
      patch_map = atoi(argv[3]);
    }
    

    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the translation vector */
    if (TCL_OK != gr_getTuple(ip,argv[2],vec,3)) {
		Gr_translateCMD_Usage(ip,argc,argv);
    	return TCL_ERROR;
	}
    
	/* execute gr function */
	errmsg = gr_translate(nodename, vec, patch_map);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_translateCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}



static void
Gr_scaleCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <factors:{x y z}>\n",
		"Purpose:\n",
        "   Postmultiply a nonuniform scale transformation to the given\n",
        "   nodes's transformation.  The scale will by the given factors\n",
        "   about the origin point.\n",
		(char *)0 
	);
}

static int 
Gr_scaleCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

  	char *nodename;
  	double factors[3]; 
    int patch_map=0;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_scaleCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc && 4 != argc) {
		Gr_scaleCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    
    if (4 == argc) {
      patch_map = atoi(argv[3]);
    }
 
    /* obtain the nodename */
    nodename = argv[1];



    /* obtain the scale factors */
    if (TCL_OK != gr_getTuple(ip,argv[2],factors,3)) {
		Gr_scaleCMD_Usage(ip,argc,argv);
    	return TCL_ERROR;
	}
    
	/* execute gr function */
	errmsg = gr_scale(nodename, factors, patch_map);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_scaleCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}








static int
Gr_mv_ctrl_ptsCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

    char *nodename;
    double vec[3];

    if (2 == argc && '?' == argv[1][0]) {
		Gr_translateCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc) {
		Gr_translateCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    /* obtain the nodename */
    nodename = argv[1];

    /* obtain the translation vector */
    if (TCL_OK != gr_getTuple(ip,argv[2],vec,3)) {
		Gr_translateCMD_Usage(ip,argc,argv);
    	return TCL_ERROR;
	}
    
	/* execute gr function */
	errmsg = gr_mv_ctrl_pts(nodename, vec);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_translateCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}


static void
Gr_saveCMD_Usage(
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"\nUsage: ",
		argv[0], 
		" <filename>",
		(char *)0 
	);
}

static int
Gr_saveCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char* filename;
	char* errmsg;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (2 != argc) {
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    /* get the filename to save */
	filename = argv[1];

    /* execute gr function */
	errmsg = gr_save(filename);

    if (errmsg) {
		Tcl_AppendResult(ip, 
			argv[0], 
			" error: ",  
			errmsg,
			(char *)0 
		);
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }
  
    return TCL_OK;
}












static void
Gr_renderCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <option...>\n",
		"Options: -file <filename>\n",
		"         -size <size:{w h}>\n",
        "         -eyepoint <point:{x y z}>\n",
		"         -viewdirection <vector:{x y z}>\n",
	    "         -updirection <vector:{x y z}>\n"
		"         -fov <angle:degrees>\n",
		"Purpose:\n",
        "   Post a redisplay with the given options, to render the scene\n",
        "   starting at the given node.\n",
		(char *)0 
	);
}
  
static int 
Gr_renderCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char *errmsg;

  	/* Default values of all options */
  	char *filename	= "image.ppm";
  	double size[2] = {200,200};
  	double eye[3] = {0,0,0};
  	double view[3] = {0,0,-1};
  	double up[3] = {0,1,0};
  	double fov = 45;

  	char *root;
  	int i;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_renderCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (argc < 2) {
		Gr_renderCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

  	/* obtain the root node of the scene */
  	root = argv[1];

  	/* parse the optional arguments, if any */
  	for (i = 2; i < argc; i += 2) {
    	if (argc < i+2) return TCL_ERROR;
    
    	if (strcmp(argv[i], "-file") == 0) {
			filename = argv[i+1];
    	} else if( strcmp(argv[i], "-size") == 0) {
      		if (TCL_OK != gr_getTuple(ip,argv[i+1],size,2)) {
				Gr_renderCMD_Usage(ip,argc,argv);
				return TCL_ERROR;
			}
    	} else if( strcmp(argv[i], "-eyepoint") == 0) {
      		if (TCL_OK != gr_getTuple(ip,argv[i+1],eye,3)) {
				Gr_renderCMD_Usage(ip,argc,argv);
				return TCL_ERROR;
			}
    	} else if( strcmp(argv[i], "-viewdirection") == 0) {
      		if (TCL_OK != gr_getTuple(ip,argv[i+1],view,3)) {
				Gr_renderCMD_Usage(ip,argc,argv);
				return TCL_ERROR;
			}
    	} else if( strcmp(argv[i], "-updirection") == 0) {
      		if (TCL_OK != gr_getTuple(ip,argv[i+1],up,3)) {
				Gr_renderCMD_Usage(ip,argc,argv);
				return TCL_ERROR;
			}
    	} else if( strcmp(argv[i], "-fov") == 0) {
      		if (TCL_OK != Tcl_GetDouble(ip,argv[i+1],&fov)) {
				Gr_renderCMD_Usage(ip,argc,argv);
				return TCL_ERROR;
			}
    	} else {
      		return TCL_ERROR;
		}
  	}
  
	/* execute gr function */
	errmsg = gr_render(root,size,filename, eye,
				view, up,fov);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_renderCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}

static void
Gr_pickCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <pos:{x y}>\n",
		"Purpose:\n",
        "   Find the node underneath the given 2d position and under\n",
        "   the given scene root.\n",
		(char *)0 
	);
}

static int 
Gr_pickCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char* errmsg;

	double pos[2];
	char *name;
        int point;
	char *root;

        char num[4];
        
    if (2 == argc && '?' == argv[1][0]) {
		Gr_pickCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc) {
		Gr_pickCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain the root node's name */
	root = argv[1];

	/* obtain the screen position */
	if (TCL_OK != gr_getTuple(ip,argv[2],pos,2)) {
		Gr_pickCMD_Usage(ip,argc,argv);
	  	return TCL_ERROR;
	}

	/* execute gr function */
	errmsg = gr_pick(root,pos,&name, &point);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_pickCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    if (name) {
	/* return result */
        sprintf(num , "%d", point);
	Tcl_AppendResult(ip, name, " ", num, (char *)0);
    }
	return TCL_OK;
}

static void
Gr_resetCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], "\n",
		"Purpose:\n",
        "   Reset system to initial configuration\n",
		(char *)0 
	);
}

static int 
Gr_resetCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char *errmsg;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_resetCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (1 != argc) {
		Gr_resetCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* execute gr function */
	errmsg = gr_reset();

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_resetCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}

/* The following callbacks are used to create objects in the scene graph.
 * We don't follow the usual Tcl/Tk convention of creating new commands
 * this the name of the object for simplicities sake.
 */
static void
Gr_transformCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <parentnode> <childname>\n",
		"Purpose:\n",
        "   Create a new transform node\n",
		(char *)0 
	);
}

static int 
Gr_transformCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

	char *parent_name;
	char *child_name;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_transformCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc) {
		Gr_transformCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain parent's name */
	parent_name = argv[1];

	/* obtain child's name */
	child_name  = argv[2];

	/* call gr function */
	errmsg = gr_addTransform(parent_name, child_name);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_transformCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}

static void
Gr_sphereCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <parentnode> <childname>\n",
		"Purpose:\n",
        "   Create a new sphere node\n",
		(char *)0 
	);
}

static int 
Gr_sphereCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

	char *parent_name;
	char *child_name;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_sphereCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc) {
		Gr_sphereCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain parent's name */
	parent_name = argv[1];

	/* obtain child's name */
	child_name  = argv[2];

	/* call gr function */
	errmsg = gr_addSphere(parent_name, child_name);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_sphereCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}

/* The following functions are used to define materials.
 */
static void
Gr_materialCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <name> ",
        "<diffuse:{r g b}> <specular:{r g b}> <exponent>\n",
		"Purpose:\n",
        "   Create a new material with the given properties.\n",
		(char *)0 
	);
}

static int 
Gr_materialCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

	char *mat_name;
	double diffuse[3];
	double specular[3];
	double spec_exp;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_materialCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (5 != argc) {
		Gr_materialCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain material name */
	mat_name = argv[1];

	/* obtain diffuse colour */
	if (TCL_OK != gr_getTuple(ip,argv[2],diffuse,3)) {
		Gr_materialCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* obtain specular colour */
	if (TCL_OK != gr_getTuple(ip,argv[3],specular,3)) {
		Gr_materialCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* obtain specular exponent */
	if (TCL_OK != Tcl_GetDouble(ip,argv[4],&spec_exp)) {
		Gr_materialCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* call gr function */
	errmsg = gr_material(mat_name,Gr_mkColour(diffuse),
				Gr_mkColour(specular), spec_exp);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_materialCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}

static void
Gr_surfacepropertyCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <matname>\n",
		"Purpose:\n",
        "   Attach a material to a node.\n",
		(char *)0 
	);
}

static int 
Gr_surfacepropertyCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

	char *node_name;
	char *mat_name;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_surfacepropertyCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (3 != argc) {
		Gr_surfacepropertyCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain node name */
	node_name = argv[1];

	/* obtain material name */
	mat_name  = argv[2];

	/* call gr function */
	errmsg = gr_surfaceproperty(node_name, mat_name);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_surfacepropertyCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}

static void
Gr_trackballCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <nodename> <old point:{x y}> <new point:{x y}> ",
        "<scale>\n",
		"Purpose:\n",
        "   Applies a rotation to the given node to simulate a virtual\n",
        "   trackball.\n",
		(char *)0 
	);
}



static void
Gr_patchCMD_Usage (
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"Usage: ", argv[0], " <parentnode> <childname>",
                " <dimensions:{n m}> <vertex list> <polygon list>",
        "<scale>\n",
		"Purpose:\n",
        "   Patch yopta\n",
        "   bezier nah.\n",
		(char *)0 
	);
}


static int 
Gr_trackballCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
	char *errmsg;

	char *node_name;
	double scale;
	double old_p[2];
	double new_p[2];

    if (2 == argc && '?' == argv[1][0]) {
		Gr_trackballCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (5 != argc) {
		Gr_trackballCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

	/* obtain name of node to which transformation will be applied */
	node_name = argv[1];

	/* obtain last position */
	if (TCL_OK != gr_getTuple(ip,argv[2],old_p,2)) {
		Gr_trackballCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* obtain new position */
	if (TCL_OK != gr_getTuple(ip,argv[3],new_p,2)) {
		Gr_trackballCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* obtain scale factor */
	if (TCL_OK != Tcl_GetDouble(ip,argv[4],&scale)) { 
		Gr_trackballCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
	}

	/* call gr function */
	errmsg = gr_trackball(node_name, old_p, new_p, scale);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ERROR: ",  
			errmsg,
	   		"\n",  
	   		(char *)0 
	  	);
		Gr_trackballCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
	return TCL_OK;
}




static int
gr_getVertices(Tcl_Interp *ip, char *arg, 
	       int *nr_vertices, GrPoint3D **vertex_list)
{
  int i;
  char **indices;
  int rtn;
  double pt[3];
  
  rtn = Tcl_SplitList(ip, arg, nr_vertices, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A vertex list must be provided",
		     (char *) 0);
    Tcl_Free((char *)indices);
    return (TCL_ERROR);
  }

  *vertex_list = new GrPoint3D [*nr_vertices];
  for(i = 0; i < *nr_vertices; i++) {
    gr_getTuple(ip, indices[i], pt, 3);
    (*vertex_list)[i] = pt;
  }
  Tcl_Free((char *)indices);

  return TCL_OK;
}











static int
gr_getSinglePolygon(Tcl_Interp *ip, char *arg, 
		    int *nr_n, int **indices)
{
  int i;
  char **str_indices;
  int rtn;
  
  rtn = Tcl_SplitList(ip, arg, nr_n, &str_indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "a list of vertices must be provide for each polygon",
		     (char *) 0);
    free(str_indices);
    return (TCL_ERROR);
  }
  
  *indices = new int [*nr_n];
  
  for(i = 0; i < (*nr_n); i++)
    Tcl_GetInt(ip, str_indices[i], &(*indices)[i]);

  return TCL_OK;
}

/*
 * gets the polygon list.
 */
static int
gr_getPolygons(Tcl_Interp *ip, char *arg, 
	       int *nr_m, int *nr_n, int ***point_list)
{
  int i;
  char **indices;
  int rtn;
  int temp_nr_n;
  
  *nr_n = 1000;

  rtn = Tcl_SplitList(ip, arg, nr_m, &indices);
  if( rtn != TCL_OK )
  {
    Tcl_AppendResult(ip, 
		     "A polygon list must be provided",
		     (char *) 0);
    free(indices);
    return TCL_ERROR;
  }

  *point_list = new int* [*nr_m];
  for(i = 0; i < *nr_m; i++) {
    gr_getSinglePolygon(ip, indices[i], &temp_nr_n, &((*point_list)[i]));
    if (temp_nr_n < *nr_n) {
      *nr_n = temp_nr_n;
    }
  }

  return TCL_OK;
}








static int 
Gr_set_patch_parentCMD (
        ClientData cd, 
        Tcl_Interp *ip, 
        int argc, 
        char *argv[]
) {
  
  char *errmsg;
  char	  *patch_name;
  char	  *patch_parent_name;
  
  if (2 == argc && '?' == argv[1][0]) {
              Gr_patchCMD_Usage(ip,argc,argv);
              return TCL_OK;
  }
  
  if (3 != argc) {
              Gr_patchCMD_Usage(ip,argc,argv);
              return TCL_ERROR;
  }

  patch_name = argv[1];
  patch_parent_name = argv[2];
  
  errmsg = gr_set_patch_parent(patch_name, patch_parent_name);
  
  if (errmsg) {
    Tcl_AppendResult(ip, 
                  argv[0], 
                  " ERROR: ",  
                  errmsg,
                  "\n",  
                  (char *)0 
          );
          Gr_patchCMD_Usage(ip,argc,argv);
    return TCL_ERROR;
  }
  return TCL_OK;
}









static int 
Gr_patchCMD (
        ClientData cd, 
        Tcl_Interp *ip, 
        int argc, 
        char *argv[]
) {
  char	  *parent_name;
  char	  *patch_name;
  char *errmsg;
  GrPoint3D *vertices;
  int nr_vertices;
  /*
  double dims_d[2];
  int dims[2];
  */

  int **point_array;
  
  int nr_n, nr_m;

  if (2 == argc && '?' == argv[1][0]) {
              Gr_patchCMD_Usage(ip,argc,argv);
              return TCL_OK;
  }

  if (5 != argc) {
              Gr_patchCMD_Usage(ip,argc,argv);
              return TCL_ERROR;
  }

  parent_name = argv[1];
  patch_name = argv[2];
  

  if( gr_getVertices(ip, argv[3], &nr_vertices, &vertices) == TCL_OK ) {
    if( gr_getPolygons(ip, argv[4], &nr_m, &nr_n, &point_array) == TCL_OK ) {
      errmsg = gr_addPatch(parent_name, patch_name, 
          vertices, nr_m, nr_n, point_array);
  
      if (errmsg) {
        Tcl_AppendResult(ip, 
                      argv[0], 
                      " ERROR: ",  
                      errmsg,
                      "\n",  
                      (char *)0 
              );
              Gr_patchCMD_Usage(ip,argc,argv);
        return TCL_ERROR;
      }
      return TCL_OK;
    }
  }
  
  return TCL_ERROR;
}





static void
Gr_modeCMD_Usage(
	Tcl_Interp *ip, 
	int argc,
	char* argv[]
) {
	Tcl_AppendResult(ip, 
		"\nUsage: ",
		argv[0], 
		" <mode>",
		(char *)0 
	);
}

static int
Gr_modeCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char mode;
	char *errmsg;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_modeCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (2 != argc) {
		Gr_modeCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    /* obtain the mode identifier */
    mode = argv[1][0];
    
	/* execute gr function */
	errmsg = gr_mode(mode);

	/* report error, if any */
    if (errmsg) {
      	Tcl_AppendResult(ip, 
	  		argv[0], 
	   		" ",  
			errmsg,
	   		(char *)0 
	  	);
		Gr_modeCMD_Usage(ip,argc,argv);
      	return TCL_ERROR;
    }
  
    return TCL_OK;
}



static int
Gr_approxCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
  GrBPatch::toggleApprox();
  return TCL_OK;
}


static int
Gr_recordCMD (
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
    char* filename;
	char* errmsg;

    if (2 == argc && '?' == argv[1][0]) {
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_OK;
    }

    if (2 != argc) {
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }

    /* get the filename to save */
	filename = argv[1];

    /* execute gr function */
	errmsg = gr_record(filename);

    if (errmsg) {
		Tcl_AppendResult(ip, 
			argv[0], 
			" error: ",  
			errmsg,
			(char *)0 
		);
		Gr_saveCMD_Usage(ip,argc,argv);
		return TCL_ERROR;
    }
  
    return TCL_OK;
  
}



static int
Gr_toggleCMD(
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {
  GrBPatch::toggleVE((GrBPatch::views) (atoi(argv[1])));
  return TCL_OK;
}

static int
Gr_cpSchemeCMD(
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {

  char* errmsg;
  
  errmsg = gr_cp_scheme(argv[1], atoi(argv[2]), (GrBPatch::CPGroup) atoi(argv[3]));
  
  if (errmsg) {
    Tcl_AppendResult(ip, 
    	argv[0], 
    	" error: ",  
    	errmsg,
    	(char *)0 
    );
    Gr_saveCMD_Usage(ip,argc,argv);
    return TCL_ERROR;
  }
  
  return TCL_OK;
}

static int
Gr_select_CP_CMD(
	ClientData cd, 
	Tcl_Interp *ip, 
	int argc, 
	char *argv[]
) {

  char* errmsg;
  
  errmsg = gr_select_cp(argv[1], atoi(argv[2]));
  
  if (errmsg) {
    Tcl_AppendResult(ip, 
    	argv[0], 
    	" error: ",  
    	errmsg,
    	(char *)0 
    );
    Gr_saveCMD_Usage(ip,argc,argv);
    return TCL_ERROR;
  }
  
  return TCL_OK;
}


/*************************************************************************
 * Associate all the Tcl commands with the functions above...
 *************************************************************************/
static int 
Gr_Init (
	Tcl_Interp *ip
) {
    Tcl_CreateCommand (ip,"gr_mode",            Gr_modeCMD,             0,0);
    Tcl_CreateCommand (ip,"gr_approx",          Gr_approxCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_identity",        Gr_identityCMD,         0,0);
    Tcl_CreateCommand (ip,"gr_rotate",          Gr_rotateCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_translate",       Gr_translateCMD,        0,0);
    Tcl_CreateCommand (ip,"gr_mv_ctrl_pts",     Gr_mv_ctrl_ptsCMD,      0,0);
    Tcl_CreateCommand (ip,"gr_scale",           Gr_scaleCMD,            0,0);
    Tcl_CreateCommand (ip,"gr_render",          Gr_renderCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_pick",            Gr_pickCMD,             0,0);
    Tcl_CreateCommand (ip,"gr_reset",           Gr_resetCMD,            0,0);
    Tcl_CreateCommand (ip,"gr_sphere",          Gr_sphereCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_transform",       Gr_transformCMD,        0,0);
    Tcl_CreateCommand (ip,"gr_material",        Gr_materialCMD,         0,0);
    Tcl_CreateCommand (ip,"gr_surfaceproperty", Gr_surfacepropertyCMD,  0,0);
    Tcl_CreateCommand (ip,"gr_trackball",       Gr_trackballCMD,        0,0);
    Tcl_CreateCommand (ip,"gr_save",            Gr_saveCMD,             0,0);
                                                                        
    Tcl_CreateCommand (ip,"gr_patch",           Gr_patchCMD,            0,0);
    Tcl_CreateCommand (ip,"gr_cp_scheme",       Gr_cpSchemeCMD,         0,0);

    Tcl_CreateCommand (ip,"gr_set_patch_parent",Gr_set_patch_parentCMD, 0,0);
    Tcl_CreateCommand (ip,"gr_record",          Gr_recordCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_toggle",          Gr_toggleCMD,           0,0);
    Tcl_CreateCommand (ip,"gr_select_cp",       Gr_select_CP_CMD,       0,0);
    
    /* The following hash tables are used to map strings to actual pointers. 
     */
    Tcl_InitHashTable(&gGR_ModelTable,TCL_STRING_KEYS);
    Tcl_InitHashTable(&gGR_MaterialTable,TCL_STRING_KEYS);

	/* Register Gr Togl callbacks with Togl
     */
	Togl_CreateFunc(gr_initialize);
	Togl_DisplayFunc(gr_display);
	Togl_ReshapeFunc(gr_reshape);

	/* Boy, we're optimistic today! */
	return TCL_OK;
}

int 
Tcl_AppInit (
	Tcl_Interp *ip
) {
	/* Initialize various packages */
    if (Tcl_Init(ip)  == TCL_ERROR) return TCL_ERROR;
    if (Tk_Init(ip)   == TCL_ERROR) return TCL_ERROR;
    if (Togl_Init(ip) == TCL_ERROR) return TCL_ERROR;
    if (Gr_Init(ip)   == TCL_ERROR) return TCL_ERROR;

	/* Set up to read from a3 script file by default */
    Tcl_SetVar(ip, "tcl_rcFileName", "./puppet.gr", TCL_GLOBAL_ONLY);

    return TCL_OK;
}

int
main (
	int argc, 
	char **argv
) {
  	Tk_Main(argc, argv, Tcl_AppInit);
  	return 0;           /* Needed only to prevent compiler warning. */
}

