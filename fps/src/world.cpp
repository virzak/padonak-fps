//****************************************************************************
// @(#) $Id: world.cpp,v 1.52 2004/01/10 01:03:16 virzak Exp $ 
// 
//  Description: 
//      Contains the methods for world information
//      
//
//****************************************************************************

#include "world.h"
#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iomanip>
#include "background.h"
#include "camera.h"
#include "light.h"
#include "objgroups.h"
#include "gamelevel_poly_01.h"

CWorld * CWorld::m_instance = NULL;



CSector::CSector() {
  *output << "CSector::CSector()" << endl;
  m_stain_container = new CStainContainer();
}



CSector::~CSector() {
  *output << "CSector::~CSector()" << endl;
  delete m_stain_container;
}


bool CWorld::readStr(ifstream & file, string & type) {
  bool again;
  do {
    again = false;
    file >> type;

    if (file.eof()) {return false;}
    
    if (type[0] == '/') {
      string oneline;
      getline(file, oneline);
      if (file.eof()) {return false;}

      again = true;
    }
  } while (again);
  
  return true;

}




bool CSector::addPoly(ifstream & file, bool quad_double, unsigned int n,
    CMovement * movement, CTexture * tex) {
  
  // Allocate space for polygon
  CWall *wall = movement?new CMovingWall(n, movement->axis, 
      movement->dist, movement->speed):new CWall(n);
      
  if (tex) wall->texture(tex);

  // Try reading it
  if (!wall->load(file)) {
    // If unsuccessful delete and break;
    delete wall;
    return false;
  }
  

  // Otherwise add it to the collection.
  m_walls.push_back(wall);
  
  if (quad_double) {

    // Create reversed polygon
    CWall *wall2 = movement ? 
      new CMovingWall(*((CMovingWall *)wall)):new CWall(*wall);

    wall2->reverse();

    // Add to the collection
    m_walls.push_back(wall2);
  }
  
  return true; 
}


CCamera * cam;

void CWorld::load(ifstream & filein) {

  string word;

  CBackground * bgd = new CBackground();
  attach(bgd);
  
  C3DObjects * objects_3d = new C3DObjects();
  attach(objects_3d);
  

  CGUI2D * gui_2d = new CGUI2D();
  attach(gui_2d);
  
  bool street_level = false;

  cp = new CPlayer(!street_level);
  cam = new CCamera(cp);
  cam->attachTo(objects_3d);
  
  CSector * sector1 = new CSector();

  if (!street_level) {

  CMirror * mirror = new CMirror();
  mirror->attachTo(objects_3d);
  
  sector1->attachTo(mirror);
  
  

    
  
  string type;
  CTexture * tex = NULL;
  
  for (;;)
  {

    if (!readStr(filein, type)) break;
    if (type == "triangle" || type == "quad" ) {
      
      
      bool poly_double = false;
      
      CMovement * movement = NULL;
      

      for (;;) {
        string option;
        if (!readStr(filein, option)) break;
        
        if (option == "--") break;
        if (option == "double") poly_double = true;
        if (option == "moving") {
          movement = new CMovement();
          
          // Read axis
          char c;
          filein >> c;
          movement->axis = getAxis(c);
          if (filein.eof()) break;

          
          // Read distance
          filein >> movement->dist;
          if (filein.eof()) break;

          // Read speed
          filein >> movement->speed;
          if (filein.eof()) break;
        }

        if (option == "-texture") {
          string tex_str;
          filein >> tex_str;
          tex = tex_container->find(tex_str);
        }
        
      }
      
      if (!sector1->addPoly(filein, poly_double, (type == "triangle"?3:4), 
            movement, tex)) break;

      if (movement) delete movement;
      
    } else if (type == "texture") {
      string tex_name;

      string file_name;
      
      if (!readStr(filein, tex_name)) break;
      if (!readStr(filein, file_name)) break;

      /*
      pair<string, CTexturePathPair> new_tex;

      new_tex.first = tex_name;
      new_tex.second.path = file_name;
      tex_container->insert(new_tex);
      */
      tex_container->addTexture(tex_name, file_name);

      
    } else if (type == "light") {
      
      CLight * light = new CLight();
      light->attachTo(objects_3d);
       
    } else {
      *output << "Scripting error: Unknown option at top level " << type << endl; 
    }
    
  }

  } else {
    sector1->attachTo(objects_3d);
    tex_container->addTexture("bg_texture", "data/12a.jpg");
    tex_container->addTexture("skel_texture", "data/xray.tga");

    sector1->loadStreet();
  }

  cp->attachTo(sector1);

}
    
void CSector::loadStreet() {
    for (unsigned int i=0; 
      i<sizeof(level_face_indicies)/sizeof(level_face_indicies[0]); 
      ++i) 
    {
      // Allocate space for polygon
      CWall *wall = new CWall(3);

      // Try reading it
      for(unsigned int j=0; j<3; j++) {
        wall->m_vertex[j].p = 
          GrPoint3D( level_vertices[level_face_indicies[i][j]]);
      }
      wall->norm(level_normals[level_face_indicies[i][3]]);
    
      // Otherwise add it to the collection.
      m_walls.push_back(wall);

      // Create reversed polygon
      CWall *wall2 = new CWall(*wall);

      wall2->reverse();

      // Add to the collection
      m_walls.push_back(wall2);
    }
}



CWorld::CWorld(const char * worldfile) : m_gravity(-9.81)
{
  
  ifstream filein(worldfile);


  load(filein);
  
  filein.close();  
  return;
}



void CSector::checkCollisions(CKineticObject * kobj) const {

  // Iterate through the elements of the sector
  for (deque<CWall *>::const_iterator i = m_walls.begin(); 
      i < m_walls.end(); ++i) {
    (*i)->checkCollision(kobj);
  }
  
}



double CSector::checkMovementCollisions(CKineticObject * kobj, const GrVector3D & v) const {

  CIResult closest_res;
  double length = v.length();
  closest_res.dist = length;
  CGrRay3D ray(kobj->pos() + GrVector3D(0, kobj->sphereSize(), 0), v);
  onRayIntercection(ray, &closest_res);
  
  if (closest_res.obj) {
    //cout << "Collision with " << *closest_res.poly << '\n';
    kobj->onCollision(closest_res.pt, closest_res.obj->norm(closest_res.pt));
    return closest_res.dist/length;
  }

  return 1;
}


void CSector::onRayIntercection(const CGrRay3D & ray, CIResult* closest_res) const {
  CIResult cur_res;

  // Iterate through the walls of the sector
  for (deque<CWall *>::const_iterator i = m_walls.begin(); 
      i < m_walls.end(); ++i) {

    // Find intersection and make sure res contains closest intersection info
    (*i)->intersect(&cur_res, ray, closest_res->dist);
    if (cur_res.obj != NULL && 
        ( cur_res.dist < closest_res->dist || closest_res->obj == NULL)) {
      *closest_res = cur_res;
    }
  }
}


void CSector::shot(const CGrRay3D & ray) {

  CIResult closest_res;
  
  //onRayIntercection(ray, &closest_res);
  rayIntercection(ray, &closest_res);

  // Create a stain
  if (closest_res.obj != NULL) {
    closest_res.obj->onBulletHit(closest_res.pt);
  }
  
}


void CSector::onDraw() const {

  tex_container->glBind("texture");
  glColor4d(1,1,1,1);

        
  m_walls.onDraw();
  
  // Draw Stains
  m_stain_container->onDraw();
}




void CMirror::onDraw () const {

  if (!mirror) return;
  

  const double major_axis_disp = -9.;
  
  if (mirror_stencil) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 1);
    
    glBegin (GL_QUADS);
      glVertex3d (major_axis_disp, 4., 3.);
      glVertex3d (major_axis_disp, 0., 3.);
      glVertex3d (major_axis_disp, 0., -2.5);
      glVertex3d (major_axis_disp, 4., -2.5);
    glEnd ();
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 1, 1);
    glEnable(GL_DEPTH_TEST);
  }

  
  glPushMatrix ();
      /* Mirrors lies in YZ plane, so scale by -1.0 in X axis */
      glScaled (-1., 1., 1.);
      /* MirrorGL_TEXTURE_2D is 2.0 units from origin, so translate by 4.0 */
      glTranslated (-2*major_axis_disp, 0., 0.);
        glFrontFace (GL_CW);
        
        child()->draw();

  glPopMatrix ();


  if (mirror_stencil) {
    glDisable(GL_STENCIL_TEST);
  } else {
    glClear (GL_DEPTH_BUFFER_BIT);
  }
  glPushAttrib (GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
    glDisable (GL_LIGHTING);
    /* Create imperfect reflector effect by blending a black
       mirror over the reflected scene with alpha of 0.05 */
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i=0; i<2; ++i) {
    
      glFrontFace (!i?GL_CW:GL_CCW);
      glDisable(GL_TEXTURE_2D);
      glColor4d (0., .1, 0., 0.45);
      glBegin (GL_QUADS);
        glVertex3d (major_axis_disp, 4., 3.);
        glVertex3d (major_axis_disp, 0., 3.);
        glVertex3d (major_axis_disp, 0., -2.5);
        glVertex3d (major_axis_disp, 4., -2.5);
      glEnd ();
      
      glEnable(GL_TEXTURE_2D);
      
      tex_container->glBind("pad_texture");
      
      glDepthFunc(GL_LEQUAL);
      glColor4d (1, 1, 1, .7);
      glBegin (GL_QUADS);
        glTexCoord2d(0.0, 1.0);
        glVertex3d (major_axis_disp+(i?0.001:-0.001), 2.5, 2.);
        glTexCoord2d(0.0, 0.0);
        glVertex3d (major_axis_disp+(i?0.001:-0.001), 1.5, 2.);
        glTexCoord2d(1.0, 0.0);
        glVertex3d (major_axis_disp+(i?0.001:-0.001), 1.5, -1.5);
        glTexCoord2d(1.0, 1.0);
        glVertex3d (major_axis_disp+(i?0.001:-0.001), 2.5, -1.5);
      glEnd ();
      glDepthFunc(GL_LESS);
    }
  
  glPopAttrib();

  


  /* Draw the real scene */
  
  glFrontFace (GL_CCW);
  
}



//
// @(#) $Log: world.cpp,v $
// @(#) Revision 1.52  2004/01/10 01:03:16  virzak
// @(#) Commented out surface subdivision for now
// @(#)
// @(#) Revision 1.51  2003/12/10 10:20:01  virzak
// @(#) Fixed g++ bug
// @(#)
// @(#) Revision 1.50  2003/12/10 10:16:23  virzak
// @(#) Subdivision doesn't occur in sync anymore
// @(#)
// @(#) Revision 1.49  2003/12/10 09:32:33  virzak
// @(#) Added Loop subdivision
// @(#)
// @(#) Revision 1.48  2003/12/08 11:31:47  virzak
// @(#) Commented out creation of patches
// @(#)
// @(#) Revision 1.47  2003/12/08 11:18:10  virzak
// @(#) CSubdivBox now extends COneDisplayList
// @(#)
// @(#) Revision 1.46  2003/12/08 07:06:38  virzak
// @(#) changes as a result of CContainer changes
// @(#) CBasicSubdivBoxes now has member array of CSubdivBox
// @(#)
// @(#) Revision 1.45  2003/12/05 17:12:17  virzak
// @(#) Came up with classes for patches and subdivisions
// @(#) CSubdivBox, CBasicSubdivBoxes, CSubdivBoxesAnim, CSubdivBoxesShot
// @(#) Instanciated their objects
// @(#) other small changes
// @(#)
// @(#) Revision 1.44  2003/10/21 08:42:53  virzak
// @(#) Now allowing for a polygon to specify its texture
// @(#)
// @(#) Revision 1.43  2003/10/18 11:19:34  virzak
// @(#) Added another type of collision detection for movement
// @(#) Divided shot routine into shot and closestIntersection
// @(#)
// @(#) Revision 1.42  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.41  2003/10/18 00:26:09  virzak
// @(#) Modified the shot routine.
// @(#)
// @(#) Revision 1.40  2003/09/30 07:22:47  virzak
// @(#) Changed all the sector polygon references to wall
// @(#)
// @(#) Revision 1.39  2003/09/29 03:43:39  virzak
// @(#) Stain container is now a pointer member
// @(#) Keeping all polygons in the same container
// @(#)
// @(#) Revision 1.38  2003/09/21 06:41:34  virzak
// @(#) Changed CSector::addPoly to allow loading of moving polygons
// @(#)
// @(#) Revision 1.37  2003/09/18 03:55:49  virzak
// @(#) Improved collision detection
// @(#)
// @(#) Revision 1.36  2003/09/17 07:35:38  virzak
// @(#) Moved classes C3DObjects and CGUI2D from world.cpp world.h to
// @(#) objgroups.cpp objgroups.h
// @(#)
// @(#) Revision 1.35  2003/09/17 06:37:07  virzak
// @(#) Implemented CSector::checkCollisions
// @(#)
// @(#) Revision 1.34  2003/09/17 03:56:10  virzak
// @(#) CSector::shot now takes CGrRay3D instead of a point and a Vector
// @(#)
// @(#) Revision 1.33  2003/09/11 20:29:14  virzak
// @(#) Improved mirror
// @(#)
// @(#) Revision 1.32  2003/09/10 06:52:19  virzak
// @(#) Changed to preincrement from postincrement where possible
// @(#)
// @(#) Revision 1.31  2003/09/10 03:40:41  virzak
// @(#) Small changes
// @(#)
// @(#) Revision 1.30  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.29  2003/09/09 22:51:01  virzak
// @(#) Rewrote CSector::shot as a result changes in polygon.h
// @(#)
// @(#) Revision 1.28  2003/09/04 02:05:40  virzak
// @(#) Changed all output calls to use << operator
// @(#)
// @(#) Revision 1.27  2003/08/29 08:33:02  virzak
// @(#) Changed texture handling
// @(#) Moved most of the constructor code into CWorld::load
// @(#)
// @(#) Revision 1.26  2003/08/28 09:07:51  virzak
// @(#) Added instance of CGUI2D class.
// @(#) Moved all 2D drawing to world.cpp from gr.cpp
// @(#)
// @(#) Revision 1.25  2003/08/27 21:49:16  virzak
// @(#) Added instances of Camera, Background, moved CSector::onDraw and
// @(#) CMirror::onDraw from gr.cpp
// @(#)
// @(#) Revision 1.24  2003/08/26 07:39:14  virzak
// @(#) Fixed singleton bug
// @(#)
// @(#) Revision 1.23  2003/08/25 20:07:04  virzak
// @(#) Renamed struct Vertex to class CVertex and moved it to util.h and .cpp
// @(#) Moved class CIResult to the header
// @(#) Moved all polygon related functions to polygon.h and .cpp
// @(#) Made CSector::addPoly be generic for all polygons
// @(#) Moved getVertex function to polygon.h
// @(#) readStr is now a static function of world class
// @(#)
// @(#) Revision 1.22  2003/08/24 22:10:13  virzak
// @(#) Added CPolygon::load function to load polygon info
// @(#) Added CPolygon::reverse that returns polygon looking back
// @(#) Added CPolygon::intersect that checks for intersection
// @(#) Moved definition of class CIResult from world.cpp to world.h
// @(#) Added CPolygon::genStain that retuns a new stain
// @(#)
// @(#) Revision 1.21  2003/08/24 20:06:44  virzak
// @(#) Changed all n in triangle to norm. Now using n for number of vertices
// @(#) CContainer related changes.
// @(#) Added CSector::addTriangle
// @(#)
// @(#) Revision 1.20  2003/08/24 06:26:13  virzak
// @(#) Changed file input code.
// @(#) Using streams now.
// @(#) Changed CSector::shot to use stain container.
// @(#)
// @(#) Revision 1.19  2003/08/02 20:31:35  virzak
// @(#) CSector is not a member of CWorld any longer, it is now an independant
// @(#) instance in the tree and world's child. It gets initialised in CWorld
// @(#) constructor and destroyed with the tree.
// @(#) CTriangle is also a derived class from CObject, however it is rendered
// @(#) as a part of CSector.
// @(#)
// @(#) Revision 1.18  2003/07/30 08:17:20  virzak
// @(#) CWorld constructor loads mirror as well as players
// @(#)
// @(#) Revision 1.17  2003/07/30 04:28:53  virzak
// @(#) more node changes
// @(#)
// @(#) Revision 1.16  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.15  2003/07/12 01:09:58  virzak
// @(#) put texture into into GrPoint2D
// @(#)
// @(#) Revision 1.14  2003/07/11 19:19:44  virzak
// @(#) Fixed g++ warning
// @(#)
// @(#) Revision 1.13  2003/07/11 04:03:23  virzak
// @(#) Got rid of getComponent where is made mode sence to put x(), y(), or z()
// @(#)
// @(#) Revision 1.12  2003/07/11 01:31:07  virzak
// @(#) Fixed stains with vertical normal
// @(#)
// @(#) Revision 1.11  2003/07/10 19:23:08  virzak
// @(#) changed triangle array into deque
// @(#)
// @(#) Revision 1.10  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.9  2003/07/04 19:31:10  virzak
// @(#) Moved stains from world to triangle in order to be able to use
// @(#) GL_ALWAYS as depth function and not have these stains going to
// @(#) other polygons. GL_EQUAL doesn't work well.
// @(#)
// @(#) Revision 1.8  2003/07/04 18:57:02  virzak
// @(#) Make the stain more realistic
// @(#)
// @(#) Revision 1.7  2003/07/04 07:56:09  virzak
// @(#) small fix
// @(#)
// @(#) Revision 1.6  2003/07/04 07:49:18  virzak
// @(#) Calculated stain position (took the code from the ray tracer)
// @(#)
// @(#) Revision 1.5  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
// @(#) Revision 1.4  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
//


