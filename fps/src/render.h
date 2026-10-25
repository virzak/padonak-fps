//*****************************************************************************
//
//  Description:
//      A small renderer on modern OpenGL (3.3 core, OpenGL ES 3.0 / WebGL 2).
//
//      It keeps the shape of the fixed-function API the game was written
//      against - a matrix stack, begin/vertex/end, pushed and popped state,
//      GL_LIGHT0 with glColorMaterial - and draws all of it with one shader.
//      Display lists become recorded meshes in a static vertex buffer.
//
//*****************************************************************************

#ifndef __RENDER_H__
#define __RENDER_H__

#include "glapi.h"

namespace rnd {

enum Primitive {
  POINTS, LINES, LINE_LOOP, LINE_STRIP,
  TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS
};

enum MatrixMode { MODELVIEW, PROJECTION };

/* Capabilities for enable/disable: the OpenGL ones (GL_BLEND, GL_CULL_FACE,
 * GL_DEPTH_TEST, GL_STENCIL_TEST) plus the two the shader implements. */
const unsigned int TEXTURE_2D = 0x10000;
const unsigned int LIGHTING   = 0x10001;

/* pushAttrib groups, named after their glPushAttrib counterparts */
const unsigned int CURRENT_BIT      = 0x01;  /* colour, normal, tex coord */
const unsigned int DEPTH_BUFFER_BIT = 0x02;  /* depth test, func, mask */
const unsigned int ENABLE_BIT       = 0x04;  /* every capability */
const unsigned int COLOR_BUFFER_BIT = 0x08;  /* blending */
const unsigned int LIGHTING_BIT     = 0x10;  /* lighting, light position */
const unsigned int TEXTURE_BIT      = 0x20;  /* bound texture */


/* Set up after the OpenGL context is current; shut down before it goes. */
bool init();
void shutdown();


/* Matrices */
void matrixMode(MatrixMode mode);
void loadIdentity();
void pushMatrix();
void popMatrix();
void translate(double x, double y, double z);
void rotate(double angle, double x, double y, double z);  /* degrees */
void scale(double x, double y, double z);
void perspective(double fovy, double aspect, double z_near, double z_far);
void ortho(double left, double right, double bottom, double top,
           double z_near, double z_far);
void lookAt(double eye_x, double eye_y, double eye_z,
            double center_x, double center_y, double center_z,
            double up_x, double up_y, double up_z);


/* State */
void enable(unsigned int cap);
void disable(unsigned int cap);
bool isEnabled(unsigned int cap);
void depthFunc(GLenum func);
void depthMask(bool write);
void blendFunc(GLenum src, GLenum dst);
void frontFace(GLenum mode);
void pushAttrib(unsigned int mask);
void popAttrib();
void bindTexture(GLuint texture);
/* Eye-space light position: transformed by the current modelview matrix,
 * like glLightfv(GL_LIGHT0, GL_POSITION, pos). */
void lightPosition(const float pos[4]);

/* Uploads an 8-bit RGB (channels 3) or RGBA (4) image with mipmaps into the
 * bound texture, replacing gluBuild2DMipmaps. */
void texImageMipmapped(int width, int height, int channels,
                       const unsigned char * data);


/* Immediate mode */
void color(double r, double g, double b, double a = 1.0);
void color3v(const double * c);
void normal(double x, double y, double z);
void normal3v(const double * n);
void texCoord(double s, double t);
void texCoord2v(const double * t);
void begin(Primitive prim);
void vertex(double x, double y, double z = 0.0);
void vertex3v(const double * v);
void end();

void rect(double x1, double y1, double x2, double y2);   /* glRect */
void sphere(double radius, int slices, int stacks);      /* gluSphere */

/* Text in the built-in 8x8 font, lower left corner at (x, y) in the current
 * coordinates; HUD code passes window pixels. */
const int FONT_WIDTH = 8;
void text(double x, double y, const char * s);


/* Recorded geometry, replacing display lists: matrix operations, texture
 * binds and begin/end between beginMesh and endMesh are baked into a static
 * vertex buffer. As with a display list, colour comes from drawing time
 * unless the recording set one. */
class Mesh;
Mesh * beginMesh();
void endMesh();
void drawMesh(const Mesh * mesh);
void deleteMesh(Mesh * mesh);

}

#endif /*__RENDER_H__*/
