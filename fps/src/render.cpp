//*****************************************************************************
//
//  Description:
//      A small renderer on modern OpenGL; see render.h
//
//*****************************************************************************

#define _USE_MATH_DEFINES   /* M_PI on MSVC */
#include "render.h"
#include "output.h"
#include "font8x8.h"
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <vector>

using namespace std;

namespace rnd {

/******************************************************************** maths */

/* column-major 4x4 matrix, as OpenGL stores them */
struct Mat4 {
  double m[16];
};

static Mat4 identity() {
  Mat4 r;
  memset(r.m, 0, sizeof(r.m));
  r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0;
  return r;
}

static Mat4 mul(const Mat4 & a, const Mat4 & b) {
  Mat4 r;
  for (int c = 0; c < 4; ++c) {
    for (int row = 0; row < 4; ++row) {
      double sum = 0;
      for (int k = 0; k < 4; ++k) sum += a.m[k*4 + row] * b.m[c*4 + k];
      r.m[c*4 + row] = sum;
    }
  }
  return r;
}

static void transformPoint(const Mat4 & a, const float in[3], float out[3]) {
  for (int row = 0; row < 3; ++row) {
    out[row] = (float) (a.m[row] * in[0] + a.m[4 + row] * in[1] +
                        a.m[8 + row] * in[2] + a.m[12 + row]);
  }
}

/* inverse transpose of the upper 3x3, row-major, for transforming normals */
static void normalMatrix(const Mat4 & a, double n[9]) {
  double m00 = a.m[0], m01 = a.m[4], m02 = a.m[8];
  double m10 = a.m[1], m11 = a.m[5], m12 = a.m[9];
  double m20 = a.m[2], m21 = a.m[6], m22 = a.m[10];
  double c00 = m11*m22 - m12*m21, c01 = m12*m20 - m10*m22, c02 = m10*m21 - m11*m20;
  double c10 = m02*m21 - m01*m22, c11 = m00*m22 - m02*m20, c12 = m01*m20 - m00*m21;
  double c20 = m01*m12 - m02*m11, c21 = m02*m10 - m00*m12, c22 = m00*m11 - m01*m10;
  double det = m00*c00 + m01*c01 + m02*c02;
  double inv = det != 0 ? 1.0 / det : 0.0;
  /* the cofactor matrix divided by the determinant is the inverse transpose */
  n[0] = c00*inv; n[1] = c01*inv; n[2] = c02*inv;
  n[3] = c10*inv; n[4] = c11*inv; n[5] = c12*inv;
  n[6] = c20*inv; n[7] = c21*inv; n[8] = c22*inv;
}

static void transformNormal(const double n[9], const float in[3], float out[3]) {
  for (int row = 0; row < 3; ++row) {
    out[row] = (float) (n[row*3] * in[0] + n[row*3 + 1] * in[1] +
                        n[row*3 + 2] * in[2]);
  }
}


/******************************************************************** state */

struct Vertex {
  float pos[3];
  float normal[3];
  float tex[2];
  float color[4];
};

struct State {
  bool blend, cull, depth_test, stencil, texture, lighting;
  GLenum depth_func;
  bool depth_mask;
  GLenum blend_src, blend_dst;
  GLenum front_face;
  float color[4];
  float normal[3];
  float tex[2];
  GLuint texture_id;
  float light[4];
};

struct SavedState {
  State state;
  unsigned int mask;
};

struct Batch {
  GLenum mode;
  GLint first;
  GLsizei count;
  bool has_texture;          /* the recording bound a texture */
  GLuint texture_id;
  bool current_color;        /* no colour was recorded: use the one at draw */
};

class Mesh {
  public:
    vector<Vertex> vertices;
    vector<Batch> batches;
    GLuint vao, vbo;
    Mesh() : vao(0), vbo(0) {}
};


static State s;
static vector<SavedState> s_saved;

static vector<Mat4> s_modelview(1, identity());
static vector<Mat4> s_projection(1, identity());
static MatrixMode s_mode = MODELVIEW;

static Primitive s_prim = TRIANGLES;
static vector<Vertex> s_vertices;
static vector<Vertex> s_expanded;

static Mesh * s_mesh = NULL;          /* recording into this mesh */
static bool s_mesh_color_set;
static bool s_mesh_has_texture;
static GLuint s_mesh_texture;

static GLuint s_program, s_vao, s_vbo, s_font;
static GLint u_projection, u_modelview, u_normal_matrix, u_lighting,
             u_light_pos, u_texturing, u_texture, u_use_color, u_color;


static void resetState() {
  s.blend = s.cull = s.depth_test = s.stencil = false;
  s.texture = s.lighting = false;
  s.depth_func = GL_LESS;
  s.depth_mask = true;
  s.blend_src = GL_ONE;
  s.blend_dst = GL_ZERO;
  s.front_face = GL_CCW;
  s.color[0] = s.color[1] = s.color[2] = s.color[3] = 1.0f;
  s.normal[0] = s.normal[1] = 0.0f; s.normal[2] = 1.0f;
  s.tex[0] = s.tex[1] = 0.0f;
  s.texture_id = 0;
  /* GL_LIGHT0's default position: a directional light along +z */
  s.light[0] = s.light[1] = s.light[3] = 0.0f;
  s.light[2] = 1.0f;
  s_saved.clear();
}


/****************************************************************** shaders */

#ifdef __EMSCRIPTEN__
static const char * s_header = "#version 300 es\nprecision mediump float;\n";
#else
static const char * s_header = "#version 330 core\n";
#endif

static const char * s_vertex_shader =
  "layout(location = 0) in vec3 aPos;\n"
  "layout(location = 1) in vec3 aNormal;\n"
  "layout(location = 2) in vec2 aTexCoord;\n"
  "layout(location = 3) in vec4 aColor;\n"
  "uniform mat4 uProjection;\n"
  "uniform mat4 uModelView;\n"
  "uniform mat3 uNormalMatrix;\n"
  "uniform bool uLighting;\n"
  "uniform vec4 uLightPos;\n"
  "uniform bool uUseColor;\n"
  "uniform vec4 uColor;\n"
  "out vec4 vColor;\n"
  "out vec2 vTexCoord;\n"
  "void main() {\n"
  "  vec4 eye = uModelView * vec4(aPos, 1.0);\n"
  "  gl_Position = uProjection * eye;\n"
  "  vTexCoord = aTexCoord;\n"
  "  vec4 color = uUseColor ? uColor : aColor;\n"
  "  if (uLighting) {\n"
  /* GL_LIGHT0 as the game sets it up: ambient 0.5 plus the default global
   * ambient 0.2, diffuse 1, no specular, glColorMaterial on the colour */
  "    vec3 n = normalize(uNormalMatrix * aNormal);\n"
  "    vec3 l = normalize(uLightPos.xyz - eye.xyz * uLightPos.w);\n"
  "    float diffuse = max(dot(n, l), 0.0);\n"
  "    color.rgb = min(color.rgb * (0.7 + diffuse), vec3(1.0));\n"
  "  }\n"
  "  vColor = color;\n"
  "}\n";

static const char * s_fragment_shader =
  "in vec4 vColor;\n"
  "in vec2 vTexCoord;\n"
  "uniform bool uTexturing;\n"
  "uniform sampler2D uTexture;\n"
  "out vec4 fragColor;\n"
  "void main() {\n"
  "  vec4 color = vColor;\n"
  "  if (uTexturing) color *= texture(uTexture, vTexCoord);\n"   /* GL_MODULATE */
  "  fragColor = color;\n"
  "}\n";


static GLuint compile(GLenum type, const char * body) {
  const char * sources[2] = { s_header, body };
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 2, sources, NULL);
  glCompileShader(shader);
  GLint ok = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[2048];
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    COutput::instanceOf()->printf("Shader compile failed: %s\n", log);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}


static void setupAttributes() {
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (void *) offsetof(Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (void *) offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (void *) offsetof(Vertex, tex));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (void *) offsetof(Vertex, color));
}


/* The 8x8 font as a 16x8 grid of glyphs, white with the glyph in alpha.
 * Glyph rows go bottom up so texture coordinates match screen y. */
static void createFont() {
  const int w = 16 * 8, h = 8 * 8;
  vector<unsigned char> pixels(w * h * 4, 255);
  for (int c = 0; c < 128; ++c) {
    int x0 = (c % 16) * 8, y0 = (c / 16) * 8;
    for (int r = 0; r < 8; ++r) {
      unsigned char bits = font8x8_basic[c][r];
      for (int b = 0; b < 8; ++b) {
        int y = y0 + 7 - r;
        pixels[(y * w + x0 + b) * 4 + 3] = (bits & (1 << b)) ? 255 : 0;
      }
    }
  }
  glGenTextures(1, &s_font);
  glBindTexture(GL_TEXTURE_2D, s_font);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &pixels[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


bool init() {
  resetState();

  GLuint vs = compile(GL_VERTEX_SHADER, s_vertex_shader);
  GLuint fs = compile(GL_FRAGMENT_SHADER, s_fragment_shader);
  if (!vs || !fs) return false;

  s_program = glCreateProgram();
  glAttachShader(s_program, vs);
  glAttachShader(s_program, fs);
  glLinkProgram(s_program);
  glDeleteShader(vs);
  glDeleteShader(fs);
  GLint ok = 0;
  glGetProgramiv(s_program, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[2048];
    glGetProgramInfoLog(s_program, sizeof(log), NULL, log);
    COutput::instanceOf()->printf("Shader link failed: %s\n", log);
    return false;
  }
  glUseProgram(s_program);

  u_projection = glGetUniformLocation(s_program, "uProjection");
  u_modelview = glGetUniformLocation(s_program, "uModelView");
  u_normal_matrix = glGetUniformLocation(s_program, "uNormalMatrix");
  u_lighting = glGetUniformLocation(s_program, "uLighting");
  u_light_pos = glGetUniformLocation(s_program, "uLightPos");
  u_texturing = glGetUniformLocation(s_program, "uTexturing");
  u_texture = glGetUniformLocation(s_program, "uTexture");
  u_use_color = glGetUniformLocation(s_program, "uUseColor");
  u_color = glGetUniformLocation(s_program, "uColor");
  glUniform1i(u_texture, 0);

  glGenVertexArrays(1, &s_vao);
  glBindVertexArray(s_vao);
  glGenBuffers(1, &s_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
  setupAttributes();

  createFont();
  return true;
}


void shutdown() {
  glDeleteTextures(1, &s_font);
  glDeleteBuffers(1, &s_vbo);
  glDeleteVertexArrays(1, &s_vao);
  glDeleteProgram(s_program);
  s_font = s_vbo = s_vao = s_program = 0;
}


/***************************************************************** matrices */

static vector<Mat4> & stack() {
  return s_mode == MODELVIEW ? s_modelview : s_projection;
}

static void multiply(const Mat4 & m) {
  stack().back() = mul(stack().back(), m);
}

void matrixMode(MatrixMode mode) { s_mode = mode; }
void loadIdentity() { stack().back() = identity(); }
void pushMatrix() { stack().push_back(stack().back()); }
void popMatrix() { if (stack().size() > 1) stack().pop_back(); }

void translate(double x, double y, double z) {
  Mat4 m = identity();
  m.m[12] = x; m.m[13] = y; m.m[14] = z;
  multiply(m);
}

void scale(double x, double y, double z) {
  Mat4 m = identity();
  m.m[0] = x; m.m[5] = y; m.m[10] = z;
  multiply(m);
}

void rotate(double angle, double x, double y, double z) {
  double len = sqrt(x*x + y*y + z*z);
  if (len == 0) return;
  x /= len; y /= len; z /= len;
  double a = angle * M_PI / 180.0, c = cos(a), sn = sin(a), t = 1 - c;
  Mat4 m = identity();
  m.m[0] = x*x*t + c;   m.m[4] = x*y*t - z*sn; m.m[8]  = x*z*t + y*sn;
  m.m[1] = y*x*t + z*sn; m.m[5] = y*y*t + c;   m.m[9]  = y*z*t - x*sn;
  m.m[2] = x*z*t - y*sn; m.m[6] = y*z*t + x*sn; m.m[10] = z*z*t + c;
  multiply(m);
}

void perspective(double fovy, double aspect, double z_near, double z_far) {
  double f = 1.0 / tan(fovy * M_PI / 360.0);
  Mat4 m;
  memset(m.m, 0, sizeof(m.m));
  m.m[0] = f / aspect;
  m.m[5] = f;
  m.m[10] = (z_far + z_near) / (z_near - z_far);
  m.m[11] = -1;
  m.m[14] = 2 * z_far * z_near / (z_near - z_far);
  multiply(m);
}

void ortho(double l, double r, double b, double t, double n, double f) {
  Mat4 m = identity();
  m.m[0] = 2 / (r - l);
  m.m[5] = 2 / (t - b);
  m.m[10] = -2 / (f - n);
  m.m[12] = -(r + l) / (r - l);
  m.m[13] = -(t + b) / (t - b);
  m.m[14] = -(f + n) / (f - n);
  multiply(m);
}

void lookAt(double ex, double ey, double ez, double cx, double cy, double cz,
            double ux, double uy, double uz) {
  double fx = cx - ex, fy = cy - ey, fz = cz - ez;
  double fl = sqrt(fx*fx + fy*fy + fz*fz);
  if (fl == 0) return;
  fx /= fl; fy /= fl; fz /= fl;
  /* side = forward x up, then up = side x forward, as gluLookAt does */
  double sx = fy*uz - fz*uy, sy = fz*ux - fx*uz, sz = fx*uy - fy*ux;
  double sl = sqrt(sx*sx + sy*sy + sz*sz);
  if (sl == 0) return;
  sx /= sl; sy /= sl; sz /= sl;
  double vx = sy*fz - sz*fy, vy = sz*fx - sx*fz, vz = sx*fy - sy*fx;
  Mat4 m = identity();
  m.m[0] = sx;  m.m[4] = sy;  m.m[8]  = sz;
  m.m[1] = vx;  m.m[5] = vy;  m.m[9]  = vz;
  m.m[2] = -fx; m.m[6] = -fy; m.m[10] = -fz;
  multiply(m);
  translate(-ex, -ey, -ez);
}


/******************************************************************** state */

static void setCap(GLenum cap, bool on) {
  if (on) glEnable(cap); else glDisable(cap);
}

static bool * capField(unsigned int cap) {
  switch (cap) {
    case GL_BLEND:        return &s.blend;
    case GL_CULL_FACE:    return &s.cull;
    case GL_DEPTH_TEST:   return &s.depth_test;
    case GL_STENCIL_TEST: return &s.stencil;
    case TEXTURE_2D:      return &s.texture;
    case LIGHTING:        return &s.lighting;
    default:              return NULL;
  }
}

static void setEnabled(unsigned int cap, bool on) {
  bool * field = capField(cap);
  if (field) *field = on;
  if (cap != TEXTURE_2D && cap != LIGHTING) setCap(cap, on);
}

void enable(unsigned int cap) { setEnabled(cap, true); }
void disable(unsigned int cap) { setEnabled(cap, false); }

bool isEnabled(unsigned int cap) {
  bool * field = capField(cap);
  return field ? *field : glIsEnabled(cap) != GL_FALSE;
}

void depthFunc(GLenum func) { s.depth_func = func; glDepthFunc(func); }
void depthMask(bool write) { s.depth_mask = write; glDepthMask(write); }
void frontFace(GLenum mode) { s.front_face = mode; glFrontFace(mode); }

void blendFunc(GLenum src, GLenum dst) {
  s.blend_src = src;
  s.blend_dst = dst;
  glBlendFunc(src, dst);
}

void pushAttrib(unsigned int mask) {
  SavedState saved;
  saved.state = s;
  saved.mask = mask;
  s_saved.push_back(saved);
}

void popAttrib() {
  if (s_saved.empty()) return;
  const State & o = s_saved.back().state;
  unsigned int mask = s_saved.back().mask;

  if (mask & CURRENT_BIT) {
    memcpy(s.color, o.color, sizeof(s.color));
    memcpy(s.normal, o.normal, sizeof(s.normal));
    memcpy(s.tex, o.tex, sizeof(s.tex));
  }
  if (mask & (DEPTH_BUFFER_BIT | ENABLE_BIT)) {
    if (s.depth_test != o.depth_test) setEnabled(GL_DEPTH_TEST, o.depth_test);
  }
  if (mask & DEPTH_BUFFER_BIT) {
    if (s.depth_func != o.depth_func) depthFunc(o.depth_func);
    if (s.depth_mask != o.depth_mask) depthMask(o.depth_mask);
  }
  if (mask & (COLOR_BUFFER_BIT | ENABLE_BIT)) {
    if (s.blend != o.blend) setEnabled(GL_BLEND, o.blend);
  }
  if (mask & COLOR_BUFFER_BIT) {
    if (s.blend_src != o.blend_src || s.blend_dst != o.blend_dst)
      blendFunc(o.blend_src, o.blend_dst);
  }
  if (mask & (LIGHTING_BIT | ENABLE_BIT)) {
    s.lighting = o.lighting;
  }
  if (mask & LIGHTING_BIT) {
    memcpy(s.light, o.light, sizeof(s.light));
  }
  if (mask & ENABLE_BIT) {
    if (s.cull != o.cull) setEnabled(GL_CULL_FACE, o.cull);
    if (s.stencil != o.stencil) setEnabled(GL_STENCIL_TEST, o.stencil);
    s.texture = o.texture;
  }
  if (mask & TEXTURE_BIT) {
    s.texture_id = o.texture_id;
  }
  s_saved.pop_back();
}

void bindTexture(GLuint texture) {
  if (s_mesh) {
    /* compiled into the mesh, like glBindTexture in a display list */
    s_mesh_has_texture = true;
    s_mesh_texture = texture;
  } else {
    s.texture_id = texture;
  }
}

void lightPosition(const float pos[4]) {
  const Mat4 & mv = s_modelview.back();
  for (int row = 0; row < 4; ++row) {
    s.light[row] = (float) (mv.m[row] * pos[0] + mv.m[4 + row] * pos[1] +
                            mv.m[8 + row] * pos[2] + mv.m[12 + row] * pos[3]);
  }
}

void texImageMipmapped(int width, int height, int channels,
                       const unsigned char * data) {
  GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA8 : GL_RGB8,
      width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glGenerateMipmap(GL_TEXTURE_2D);
}


/******************************************************************* drawing */

static void setUniforms(bool use_current_color) {
  float f[16];
  for (int i = 0; i < 16; ++i) f[i] = (float) s_projection.back().m[i];
  glUniformMatrix4fv(u_projection, 1, GL_FALSE, f);
  for (int i = 0; i < 16; ++i) f[i] = (float) s_modelview.back().m[i];
  glUniformMatrix4fv(u_modelview, 1, GL_FALSE, f);

  double n[9];
  normalMatrix(s_modelview.back(), n);
  float nf[9];   /* n is row-major; OpenGL wants columns */
  for (int row = 0; row < 3; ++row)
    for (int col = 0; col < 3; ++col) nf[col*3 + row] = (float) n[row*3 + col];
  glUniformMatrix3fv(u_normal_matrix, 1, GL_FALSE, nf);

  glUniform1i(u_lighting, s.lighting);
  glUniform4fv(u_light_pos, 1, s.light);

  /* an unbound texture switches texturing off, as an incomplete one did */
  bool texturing = s.texture && s.texture_id != 0;
  glUniform1i(u_texturing, texturing);
  if (texturing) glBindTexture(GL_TEXTURE_2D, s.texture_id);

  glUniform1i(u_use_color, use_current_color);
  glUniform4fv(u_color, 1, s.color);
}

static GLenum glMode(Primitive prim) {
  switch (prim) {
    case POINTS:         return GL_POINTS;
    case LINES:          return GL_LINES;
    case LINE_LOOP:      return GL_LINE_LOOP;
    case LINE_STRIP:     return GL_LINE_STRIP;
    case TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case TRIANGLE_FAN:   return GL_TRIANGLE_FAN;
    default:             return GL_TRIANGLES;   /* TRIANGLES and QUADS */
  }
}

void color(double r, double g, double b, double a) {
  s.color[0] = (float) r; s.color[1] = (float) g;
  s.color[2] = (float) b; s.color[3] = (float) a;
  if (s_mesh) s_mesh_color_set = true;
}
void color3v(const double * c) { color(c[0], c[1], c[2]); }

void normal(double x, double y, double z) {
  s.normal[0] = (float) x; s.normal[1] = (float) y; s.normal[2] = (float) z;
}
void normal3v(const double * n) { normal(n[0], n[1], n[2]); }

void texCoord(double u, double v) { s.tex[0] = (float) u; s.tex[1] = (float) v; }
void texCoord2v(const double * t) { texCoord(t[0], t[1]); }

void begin(Primitive prim) {
  s_prim = prim;
  s_vertices.clear();
}

void vertex(double x, double y, double z) {
  Vertex v;
  v.pos[0] = (float) x; v.pos[1] = (float) y; v.pos[2] = (float) z;
  memcpy(v.normal, s.normal, sizeof(v.normal));
  memcpy(v.tex, s.tex, sizeof(v.tex));
  memcpy(v.color, s.color, sizeof(v.color));
  s_vertices.push_back(v);
}
void vertex3v(const double * v) { vertex(v[0], v[1], v[2]); }

static void record(GLenum mode, const vector<Vertex> & vertices) {
  const Mat4 & mv = s_modelview.back();
  double n[9];
  normalMatrix(mv, n);

  bool joinable = mode == GL_TRIANGLES || mode == GL_LINES || mode == GL_POINTS;
  Batch * last = s_mesh->batches.empty() ? NULL : &s_mesh->batches.back();
  if (!(joinable && last && last->mode == mode &&
        last->has_texture == s_mesh_has_texture &&
        last->texture_id == s_mesh_texture &&
        last->current_color == !s_mesh_color_set)) {
    Batch b;
    b.mode = mode;
    b.first = (GLint) s_mesh->vertices.size();
    b.count = 0;
    b.has_texture = s_mesh_has_texture;
    b.texture_id = s_mesh_texture;
    b.current_color = !s_mesh_color_set;
    s_mesh->batches.push_back(b);
    last = &s_mesh->batches.back();
  }

  for (size_t i = 0; i < vertices.size(); ++i) {
    Vertex v = vertices[i];
    transformPoint(mv, vertices[i].pos, v.pos);
    transformNormal(n, vertices[i].normal, v.normal);
    s_mesh->vertices.push_back(v);
  }
  last->count += (GLsizei) vertices.size();
}

void end() {
  const vector<Vertex> * vertices = &s_vertices;
  if (s_prim == QUADS) {
    s_expanded.clear();
    for (size_t i = 0; i + 3 < s_vertices.size(); i += 4) {
      static const int order[6] = { 0, 1, 2, 0, 2, 3 };
      for (int k = 0; k < 6; ++k) s_expanded.push_back(s_vertices[i + order[k]]);
    }
    vertices = &s_expanded;
  }
  if (vertices->empty()) return;

  GLenum mode = glMode(s_prim);
  if (s_mesh) {
    record(mode, *vertices);
    return;
  }

  glBindVertexArray(s_vao);
  glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex),
      &(*vertices)[0], GL_STREAM_DRAW);
  setUniforms(false);
  glDrawArrays(mode, 0, (GLsizei) vertices->size());
}

void rect(double x1, double y1, double x2, double y2) {
  begin(QUADS);
    vertex(x1, y1); vertex(x2, y1); vertex(x2, y2); vertex(x1, y2);
  end();
}

void sphere(double radius, int slices, int stacks) {
  /* z is the polar axis, as with gluSphere */
  for (int i = 0; i < stacks; ++i) {
    double t0 = M_PI * i / stacks, t1 = M_PI * (i + 1) / stacks;
    begin(QUADS);
    for (int j = 0; j < slices; ++j) {
      double p0 = 2 * M_PI * j / slices, p1 = 2 * M_PI * (j + 1) / slices;
      double corners[4][2] = { {t0, p0}, {t1, p0}, {t1, p1}, {t0, p1} };
      for (int k = 0; k < 4; ++k) {
        double x = sin(corners[k][0]) * cos(corners[k][1]);
        double y = sin(corners[k][0]) * sin(corners[k][1]);
        double z = cos(corners[k][0]);
        normal(x, y, z);
        vertex(x * radius, y * radius, z * radius);
      }
    }
    end();
  }
}

void text(double x, double y, const char * str) {
  pushAttrib(ENABLE_BIT | COLOR_BUFFER_BIT | TEXTURE_BIT);
  enable(TEXTURE_2D);
  disable(LIGHTING);
  enable(GL_BLEND);
  blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  bindTexture(s_font);

  begin(QUADS);
  for (const char * c = str; *c; ++c, x += FONT_WIDTH) {
    unsigned char ch = (unsigned char) *c;
    if (ch >= 128) continue;
    double u0 = (ch % 16) / 16.0, u1 = u0 + 1 / 16.0;
    double v0 = (ch / 16) / 8.0, v1 = v0 + 1 / 8.0;
    texCoord(u0, v0); vertex(x, y);
    texCoord(u1, v0); vertex(x + 8, y);
    texCoord(u1, v1); vertex(x + 8, y + 8);
    texCoord(u0, v1); vertex(x, y + 8);
  }
  end();

  popAttrib();
}


/******************************************************************* meshes */

Mesh * beginMesh() {
  s_mesh = new Mesh();
  s_mesh_color_set = false;
  s_mesh_has_texture = false;
  s_mesh_texture = 0;
  s_modelview.push_back(identity());
  return s_mesh;
}

void endMesh() {
  Mesh * mesh = s_mesh;
  s_mesh = NULL;
  if (s_modelview.size() > 1) s_modelview.pop_back();

  glGenVertexArrays(1, &mesh->vao);
  glBindVertexArray(mesh->vao);
  glGenBuffers(1, &mesh->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  if (!mesh->vertices.empty()) {
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex),
        &mesh->vertices[0], GL_STATIC_DRAW);
  }
  setupAttributes();
  glBindVertexArray(s_vao);
  glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
}

void drawMesh(const Mesh * mesh) {
  if (!mesh) return;
  glBindVertexArray(mesh->vao);
  for (size_t i = 0; i < mesh->batches.size(); ++i) {
    const Batch & b = mesh->batches[i];
    if (b.has_texture) s.texture_id = b.texture_id;
    setUniforms(b.current_color);
    glDrawArrays(b.mode, b.first, b.count);
  }
  glBindVertexArray(s_vao);
}

void deleteMesh(Mesh * mesh) {
  if (!mesh) return;
  glDeleteBuffers(1, &mesh->vbo);
  glDeleteVertexArrays(1, &mesh->vao);
  delete mesh;
}

}
