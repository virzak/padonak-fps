//*****************************************************************************
//
//  Description:
//      On-screen controls for touch screens. Fingers are turned into the
//      key, button and mouse calls a keyboard and mouse would make, so the
//      game itself does not know the difference.
//
//*****************************************************************************

#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <SDL3/SDL_events.h>
#include "window.h"

class CTouchControls {
  public:
    enum { MAX_FINGERS = 10 };

    /* Buttons on the right; the left part of the screen is the move stick
     * and dragging anywhere else turns the view. */
    enum Button {
      B_FIRE, B_JUMP, B_VIEW, B_FLY, B_LIGHT, B_MIRROR, B_LOG, B_TOTAL
    };

    CTouchControls();

    /* Handles finger events; returns false for any other event. */
    bool event(const SDL_Event & e, CCallbacks * cb, int width, int height);

    /* Keyboard or mouse use hides the controls until the next touch. */
    void otherInput() { m_visible = false; }

    /* Called once a frame after the game has read its input, so a tap
     * shorter than a frame still registers. */
    void endFrame();

    void draw(int width, int height) const;

  private:
    struct Finger {
      SDL_FingerID id;
      enum { NONE, STICK, LOOK, BUTTON } role;
      int button;
      float x0, y0;     /* where it went down (stick centre), pixels, y up */
      float x, y;       /* where it is now */
    };

    Finger m_fingers[MAX_FINGERS];
    bool m_down[B_TOTAL];          /* button held by a finger */
    bool m_pending_release[B_TOTAL];
    bool m_view_on;                /* third person, toggled */
    unsigned int m_stick_keys;     /* W A S D held, as bits */
    float m_rest_x, m_rest_y;      /* sub-unit look movement */
    bool m_visible;
    CCallbacks * m_cb;

    Finger * find(SDL_FingerID id);
    int hitButton(float x, float y, int width, int height) const;
    void buttonCentre(int b, int width, int height,
        float & x, float & y, float & r) const;
    void press(int b);
    void release(int b);
    void setStick(float dx, float dy, float radius);
    void look(float dx, float dy, int height);
};

CTouchControls & touchControls();

#endif /*__TOUCH_H__*/
