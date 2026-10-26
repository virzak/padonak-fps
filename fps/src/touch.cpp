//*****************************************************************************
//
//  Description:
//      On-screen controls for touch screens: a move stick that appears
//      where the left thumb goes down, dragging elsewhere to look, and
//      buttons for the actions a phone has no keys for.
//
//*****************************************************************************

#include <math.h>
#include <string.h>
#include "touch.h"
#include "render.h"

static const char * const s_labels[CTouchControls::B_TOTAL] = {
  "Fire", "Jump", "View", "Fly", "Light", "Mirror", "Log"
};

/* the keys the stick holds down, one bit each */
static const SDL_Scancode s_stick_codes[4] = {
  SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D
};

/* how much of the stick radius a direction needs before it counts */
static const float STICK_DEAD = 0.35f;

/* look speed: mouse units per screen height of drag (0.1 degree each) */
static const float LOOK_PER_HEIGHT = 1500.0f;

CTouchControls & touchControls() {
  static CTouchControls s_controls;
  return s_controls;
}

CTouchControls::CTouchControls() :
  m_view_on(false), m_stick_keys(0), m_rest_x(0), m_rest_y(0),
  m_visible(false), m_cb(NULL) {
  memset(m_fingers, 0, sizeof(m_fingers));
  memset(m_down, 0, sizeof(m_down));
  memset(m_pending_release, 0, sizeof(m_pending_release));
}

/* Sizes follow the short side of the screen, so they suit any density. */
static float unit(int width, int height) {
  return (float) (width < height ? width : height);
}

void CTouchControls::buttonCentre(int b, int width, int height,
    float & x, float & y, float & r) const {
  float u = unit(width, height);
  float m = 0.04f * u;           /* margin */
  float big = 0.085f * u;
  float small = 0.065f * u;      /* wide enough for "Mirror" */
  float bar = 20;                /* the help bar along the bottom */

  float fire_r = 1.25f * big;
  float fire_x = width - m - fire_r;
  float fire_y = bar + m + fire_r;

  switch (b) {
    case B_FIRE: x = fire_x; y = fire_y; r = fire_r; break;
    case B_JUMP: x = fire_x - 2.7f * big; y = bar + m + big; r = big; break;
    case B_VIEW: x = fire_x; y = fire_y + 2.6f * big; r = big; break;
    default:
      /* the toggles sit in a row along the top right */
      r = small;
      x = width - m - small - (b - B_FLY) * 2.3f * small;
      y = height - m - small;
      break;
  }
}

int CTouchControls::hitButton(float x, float y, int width, int height) const {
  for (int b = 0; b < B_TOTAL; ++b) {
    float bx, by, r;
    buttonCentre(b, width, height, bx, by, r);
    r *= 1.15f;                  /* a little slack for thumbs */
    if ((x - bx) * (x - bx) + (y - by) * (y - by) < r * r) return b;
  }
  return -1;
}

CTouchControls::Finger * CTouchControls::find(SDL_FingerID id) {
  for (int i = 0; i < MAX_FINGERS; ++i) {
    if (m_fingers[i].role != Finger::NONE && m_fingers[i].id == id)
      return &m_fingers[i];
  }
  return NULL;
}

void CTouchControls::press(int b) {
  m_pending_release[b] = false;
  m_down[b] = true;
  switch (b) {
    case B_FIRE:   m_cb->buttonPress(1); break;
    case B_JUMP:   m_cb->buttonPress(3); break;
    case B_VIEW:
      /* each press and release of Tab switches the view */
      m_view_on = !m_view_on;
      m_cb->keyPress(SDL_SCANCODE_TAB);
      break;
    case B_FLY:    m_cb->keyPress(SDL_SCANCODE_O); break;
    case B_LIGHT:  m_cb->keyPress(SDL_SCANCODE_L); break;
    case B_MIRROR: m_cb->keyPress(SDL_SCANCODE_M); break;
    case B_LOG:    m_cb->keyPress(SDL_SCANCODE_GRAVE); break;
  }
}

/* The game polls its keys once a frame, so releases wait for endFrame. */
void CTouchControls::release(int b) {
  m_pending_release[b] = true;
}

void CTouchControls::endFrame() {
  if (!m_cb) return;
  for (int b = 0; b < B_TOTAL; ++b) {
    if (!m_pending_release[b]) continue;
    m_pending_release[b] = false;
    m_down[b] = false;
    switch (b) {
      case B_FIRE:   m_cb->buttonRelease(1); break;
      case B_JUMP:   m_cb->buttonRelease(3); break;
      case B_VIEW:   m_cb->keyRelease(SDL_SCANCODE_TAB); break;
      case B_FLY:    m_cb->keyRelease(SDL_SCANCODE_O); break;
      case B_LIGHT:  m_cb->keyRelease(SDL_SCANCODE_L); break;
      case B_MIRROR: m_cb->keyRelease(SDL_SCANCODE_M); break;
      case B_LOG:    m_cb->keyRelease(SDL_SCANCODE_GRAVE); break;
    }
  }
}

void CTouchControls::setStick(float dx, float dy, float radius) {
  unsigned int keys = 0;
  if (dy > STICK_DEAD * radius) keys |= 1;
  if (dx < -STICK_DEAD * radius) keys |= 2;
  if (dy < -STICK_DEAD * radius) keys |= 4;
  if (dx > STICK_DEAD * radius) keys |= 8;

  for (int i = 0; i < 4; ++i) {
    unsigned int bit = 1u << i;
    if ((keys & bit) && !(m_stick_keys & bit)) m_cb->keyPress(s_stick_codes[i]);
    if (!(keys & bit) && (m_stick_keys & bit)) m_cb->keyRelease(s_stick_codes[i]);
  }
  m_stick_keys = keys;
}

void CTouchControls::look(float dx, float dy, int height) {
  /* the game takes mouse movement, with y growing downwards */
  m_rest_x += dx * LOOK_PER_HEIGHT / height;
  m_rest_y -= dy * LOOK_PER_HEIGHT / height;
  int mx = (int) m_rest_x, my = (int) m_rest_y;
  m_rest_x -= mx;
  m_rest_y -= my;
  if (mx || my) m_cb->motion(mx, my);
}

bool CTouchControls::event(const SDL_Event & e, CCallbacks * cb,
    int width, int height) {
  if (e.type != SDL_EVENT_FINGER_DOWN && e.type != SDL_EVENT_FINGER_MOTION &&
      e.type != SDL_EVENT_FINGER_UP && e.type != SDL_EVENT_FINGER_CANCELED)
    return false;

  m_cb = cb;
  m_visible = true;

  /* SDL gives 0..1 from the top left; the HUD has y going up */
  float x = e.tfinger.x * width;
  float y = (1 - e.tfinger.y) * height;
  float stick_r = 0.12f * unit(width, height);

  if (e.type == SDL_EVENT_FINGER_DOWN) {
    Finger * f = NULL;
    for (int i = 0; i < MAX_FINGERS && !f; ++i) {
      if (m_fingers[i].role == Finger::NONE) f = &m_fingers[i];
    }
    if (!f) return true;

    f->id = e.tfinger.fingerID;
    f->x0 = f->x = x;
    f->y0 = f->y = y;
    f->button = hitButton(x, y, width, height);

    bool stick_taken = false;
    for (int i = 0; i < MAX_FINGERS; ++i) {
      if (m_fingers[i].role == Finger::STICK) stick_taken = true;
    }

    if (f->button >= 0) {
      f->role = Finger::BUTTON;
      press(f->button);
    } else if (x < 0.4f * width && !stick_taken) {
      f->role = Finger::STICK;
    } else {
      f->role = Finger::LOOK;
    }
    return true;
  }

  Finger * f = find(e.tfinger.fingerID);
  if (!f) return true;

  if (e.type == SDL_EVENT_FINGER_MOTION) {
    float dx = e.tfinger.dx * width, dy = -e.tfinger.dy * height;
    f->x = x;
    f->y = y;
    if (f->role == Finger::STICK) {
      setStick(x - f->x0, y - f->y0, stick_r);
    } else if (f->role == Finger::LOOK ||
        (f->role == Finger::BUTTON && f->button == B_FIRE)) {
      /* the fire thumb can aim while it shoots */
      look(dx, dy, height);
    }
    return true;
  }

  /* up or cancelled */
  if (f->role == Finger::STICK) setStick(0, 0, stick_r);
  if (f->role == Finger::BUTTON) release(f->button);
  f->role = Finger::NONE;
  return true;
}


/****************************************************************** drawing */

static const double TWO_PI = 6.283185307179586;

static void disc(float x, float y, float r) {
  rnd::begin(rnd::TRIANGLE_FAN);
  rnd::vertex(x, y);
  for (int i = 0; i <= 32; ++i) {
    rnd::vertex(x + r * cos(i * TWO_PI / 32), y + r * sin(i * TWO_PI / 32));
  }
  rnd::end();
}

static void ring(float x, float y, float r) {
  rnd::begin(rnd::LINE_LOOP);
  for (int i = 0; i < 32; ++i) {
    rnd::vertex(x + r * cos(i * TWO_PI / 32), y + r * sin(i * TWO_PI / 32));
  }
  rnd::end();
}

static void label(float x, float y, float scale, const char * s) {
  rnd::pushMatrix();
  rnd::translate(x - strlen(s) * rnd::FONT_WIDTH * scale / 2, y - 4 * scale, 0);
  rnd::scale(scale, scale, 1);
  rnd::text(0, 0, s);
  rnd::popMatrix();
}

/* Drawn by the 2D pass, which has set up blending and pixel coordinates. */
void CTouchControls::draw(int width, int height) const {
  if (!m_visible) return;

  float u = unit(width, height);
  float text_scale = floorf(u / 360) < 1 ? 1 : floorf(u / 360);

  for (int b = 0; b < B_TOTAL; ++b) {
    float x, y, r;
    buttonCentre(b, width, height, x, y, r);
    bool lit = m_down[b] || (b == B_VIEW && m_view_on);
    rnd::color(1, 1, 1, lit ? 0.45 : 0.18);
    disc(x, y, r);
    rnd::color(1, 1, 1, 0.6);
    ring(x, y, r);
    rnd::color(1, 1, 1, 0.9);
    label(x, y, text_scale, s_labels[b]);
  }

  float stick_r = 0.12f * u;
  const Finger * stick = NULL;
  for (int i = 0; i < MAX_FINGERS; ++i) {
    if (m_fingers[i].role == Finger::STICK) stick = &m_fingers[i];
  }

  if (stick) {
    float dx = stick->x - stick->x0, dy = stick->y - stick->y0;
    float len = sqrtf(dx * dx + dy * dy);
    if (len > stick_r) {
      dx *= stick_r / len;
      dy *= stick_r / len;
    }
    rnd::color(1, 1, 1, 0.12);
    disc(stick->x0, stick->y0, stick_r);
    rnd::color(1, 1, 1, 0.5);
    ring(stick->x0, stick->y0, stick_r);
    rnd::color(1, 1, 1, 0.45);
    disc(stick->x0 + dx, stick->y0 + dy, 0.4f * stick_r);
  } else {
    /* show where the stick is when no thumb holds it */
    float m = 0.04f * u;
    float x = m + 1.2f * stick_r, y = 20 + m + 1.2f * stick_r;
    rnd::color(1, 1, 1, 0.3);
    ring(x, y, stick_r);
    rnd::color(1, 1, 1, 0.6);
    label(x, y, text_scale, "Move");
  }
}
