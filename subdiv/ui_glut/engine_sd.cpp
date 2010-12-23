#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "trackball.h"
#include "polyhedron.h"
#include "subdiv.h"



static float front_shininess[] =   {60.0};
static float front_specular[]  =   { 0.7, 0.7, 0.7, 1.0 };
static float ambient[]         =   { 0.0, 0.0, 0.0, 1.0 };
static float diffuse[]         =   { 1.0, 1.0, 1.0, 1.0 };
static float position0[]       =   { 1.0, 1.0, 1.0, 0.0 };
static float position1[]       =   {-1.0,-1.0, 1.0, 0.0 };
static float lmodel_ambient[]  =   { 0.5, 0.5, 0.5, 1.0 };
static float lmodel_twoside[]  =   {GL_TRUE};

/*
static float MaterialRed[]     =   { 0.7, 0.0, 0.0, 1.0 };
static float MaterialGreen[]   =   { 0.1, 0.5, 0.2, 1.0 };
static float MaterialBlue[]    =   { 0.0, 0.0, 0.7, 1.0 };
static float MaterialCyan[]    =   { 0.2, 0.5, 0.7, 1.0 };
static float MaterialYellow[]  =   { 0.7, 0.7, 0.0, 1.0 };
static float MaterialMagenta[] =   { 0.6, 0.2, 0.5, 1.0 };
static float MaterialWhite[]   =   { 0.7, 0.7, 0.7, 1.0 };
static float MaterialGray[]    =   { 0.2, 0.2, 0.2, 1.0 };
*/

static int       mono=0;
static int       smooth=1;

static GLint     WindH, WindW;


static bool b1;
static bool b2;
static bool b3;


#ifdef _Modifiers_
static bool bCTRL; 
static bool bSHIFT;
static bool bALT;
#endif

static CPolyhedron * box[20];

static unsigned int subdiv_ind = 0;

static unsigned lastx, lasty;


/*
static GrMatrix4x4 mat_trackball = {
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};
*/

static GrMatrix4x4 mat_trackball;


static void pinit(void)
{
  if (mono) {
    /*
    int loop;
    for (loop=0; loop<20; loop++) MaterialColor[loop]=MaterialGray;
    */
  }
  if (smooth) {
    glShadeModel( GL_SMOOTH );
  } else {
    glShadeModel( GL_FLAT );
  }

}

static void
gr_trackball (
        GrMatrix4x4 & trans,
	GrPoint2D old_p, 
	GrPoint2D new_p, 
	double scale
) {
	double x, y, z;
	GrMatrix4x4 rot;


	
	old_p = GrPoint2D(WindW - old_p.x(), WindH - old_p.y());
	new_p = GrPoint2D(WindW - new_p.x(), WindH - new_p.y());

	
	vGenerateRotVec((double) (old_p.x()/scale),(double) (old_p.y()/scale),&x,&y,&z,  WindW,  WindH);
	vGenerateRotVec((double) (new_p.x()/scale),(double) (new_p.y()/scale),&x,&y,&z,  WindW,  WindH);
	vRotateScene(x,y,z,rot);
	//mulMatrix(trans,rot,trans);
        trans = rot*trans;
	


        return;
}




static void draw ( void ) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glPushMatrix();

  
    glTranslatef( 0.0, 0.0, -5.0 );
    glMultMatrixd((GLdouble *)mat_trackball.elemArray());
    /*
    glScalef( Scale*WindH/WindW, Scale, Scale );
    glTranslatef(2.5*WindW/WindH*sin(step*1.11),2.5*cos(step*1.25*1.11),0);
    glRotatef(step*100,1,0,0);
    glRotatef(step*95,0,1,0);
    glRotatef(step*90,0,0,1);
    */

  box[subdiv_ind]->drawObject();

  /*
  seno=(sin(step)+1.0/3.0)*(4.0/5.0)*Magnitude;

  draw_object();

  */
  glPopMatrix();

  glFlush();

  glutSwapBuffers();

  /*step+=0.05;*/
}

static void idle_( void )
{
   glutPostRedisplay();
}

static void reshape( int width, int height )
{
  glViewport(0, 0, WindW=(GLint)width, WindH=(GLint)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
  gluPerspective(45.0f, (GLdouble)width / (GLdouble)height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
}



static void key( unsigned char k, int x, int y )
{
  (void) x;
  (void) y;
  switch (k) {
    case ' ': mono^=1; break;
    case 13: smooth^=1; break;

    case 's':
              box[subdiv_ind+1] = new CPolyhedron(*box[subdiv_ind]);
              subdiv_ind++;
              box[subdiv_ind]->mkSubdivision();
              glutPostRedisplay();
              break;
    case 'a':
              box[subdiv_ind+1] = new CPolyhedron(*box[subdiv_ind]);
              subdiv_ind++;
              box[subdiv_ind]->mkAverage();
              glutPostRedisplay();
              break;

    case '+': 
              
              //box[subdiv_ind]->mkSubdivision(&box[subdiv_ind+1] );
              box[subdiv_ind+1] = new CPolyhedron(*box[subdiv_ind]);
              box[subdiv_ind+1]->mkSubdivision();
              subdiv_ind++;
              box[subdiv_ind]->mkAverage();
              //subdiv_ind++;
              glutPostRedisplay();
              break; 
    case 'p': CPolyhedron::togglePoints(); break;
    case 'l': CPolyhedron::toggleLines(); break;
    case 27:
    case 'q':
       exit(0);
    
  }
  pinit();
}

static void help_message() {
  printf("Press:\ns - subdivision\na - averaging\np - toggle points\nl - toggle lines\nq or <esc> - quit\n");
}


static void AssistModifiers(){
    
#ifdef _Modifiers_
    int M = glutGetModifiers();

    bCTRL = M & GLUT_ACTIVE_CTRL;
    bSHIFT = M & GLUT_ACTIVE_SHIFT; 
    bALT = M & GLUT_ACTIVE_ALT;	
#endif
    
}



static void
motion(int x, int y)
{
  bool changes = b1 | b2 | b3;
  if (changes) {
    if (b1) {
      /*
      GrPoint2D old_p(lastx, lasty);
      GrPoint2D new_p(x, y);
      
      gr_trackball(mat_trackball, old_p, new_p, 1);
      */
#ifdef __ICC
#pragma warning (disable:981)
#endif

      gr_trackball(mat_trackball, GrPoint2D(lastx, lasty), GrPoint2D(x, y), 1);
    }
  }
  lastx = x;
  lasty = y;
}

static void
mouse(int button, int state, int x, int y)
{
  AssistModifiers();
  if (button == GLUT_LEFT_BUTTON) {
    b1 = (state == GLUT_DOWN);
    if (b1) {
      motion(x, y);
      glutPostRedisplay();

    }
  }
  if (button == GLUT_MIDDLE_BUTTON) {
           b2 = (state == GLUT_DOWN);
  }
  if (button == GLUT_RIGHT_BUTTON) {
           b3 = (state == GLUT_DOWN);
  }

}



static void init_app() {

  //box[0] = new CPolyhedron(CPolyhedron::poly_box);
  box[0] = new CPolyhedron(CPolyhedron::tri_star);
  
}


static void INIT(void)
{


  glutInitWindowPosition(0,0);
  glutInitWindowSize(640,480);

  glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB );

  if (glutCreateWindow(" Subdivision 3D") <= 0) {
     exit(0);
  }

  glClearDepth(1.0);
  glClearColor( 0.0, 0.0, 0.0, 1.0 );
  glColor3f( 1.0, 1.0, 1.0 );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glFlush();
  glutSwapBuffers();

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, position0);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, position1);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable (GL_CULL_FACE);


  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);

  glHint(GL_FOG_HINT, GL_FASTEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

  pinit();
  init_app();
  help_message();

  glutReshapeFunc( reshape );
  glutKeyboardFunc( key );
  glutIdleFunc( idle_ );
  glutDisplayFunc( draw );
  glutMotionFunc( motion );
  glutPassiveMotionFunc( motion );
  glutMouseFunc( mouse );
  glutMainLoop();
  
}

int main(int argc, char **argv)
{
  INIT();
  return(0);
}
