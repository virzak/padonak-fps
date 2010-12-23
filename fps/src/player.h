//*****************************************************************************
// @(#) $Id: player.h,v 1.27 2003/12/08 07:01:41 virzak Exp $ 
// 
//  Description: 
//      Handles individual players and first person in particular
//
//  =
//
//*****************************************************************************


#ifndef __PLAYER_H__
#define __PLAYER_H__



#include "util.h"
#include "kobject.h"
#include "app.h"
#include "displist.h"


extern const double speeds[];

enum Direction {
  D_forward,
  D_backward,
  D_left,
  D_right,
  D_jumping
};


class CPlayer : public CKineticObject/*, public CWindowDependent */ {

    const static GrVector3D starting_vector;
    const static GrMatrix4x4 right90;
    const static GrVector3D starting_right_vector;

    GrMatrix4x4 m_left_right_rot;
    GrMatrix4x4 m_up_down_rot;



    double m_rotY;  /* Y Rotation */

    double m_lookup;

    GrPoint3D m_eye_pos;
    GrVector3D m_look_dir;
    GrVector3D m_move_dir;
    GrVector3D m_move_right_dir;
    double m_walkbias;
    double m_walkbiasangle;  
    const double m_eye_height;
    const double m_height;
    const GrVector3D m_eye_height_vector;
    GrVector3D m_user_acceleration;
    unsigned int m_user_acceleration_dir;

    double m_highest;
    double m_lowest;
    CDisplayList * m_displayList;

    const bool m_checkingCollisions;

    virtual void displayListInstructions(unsigned int) const {
      drawingInstructions();
    }
    
    void drawingInstructions() const;
    inline void updateEye() {m_eye_pos = m_pos + m_eye_height_vector;}

  public:  
    CPlayer(bool checking_collisions = true);
    ~CPlayer();
    void changeLook(double lr, double ud, bool force=false);
    void genMoveVector();
    void jump();
    void fire();
    void load();
    void moveForward(double disp);
    void moveRight(double disp); 
    void onUpdate(unsigned int msecs=0);
    
    inline void accel(unsigned int dir) {
      m_user_acceleration_dir = dir;
      accel();
    }
    void accel();

    bool m_flyingMode;
    void onDraw() const;


    friend class CCamera; /* To position the camera */
    friend class CBackground; /* To draw the background */
    friend class CGUI2D;    /* To draw stats */
    friend class CLight;    /* To light */
    friend class CUI;       /* To allow user command */
};



#endif

//
// @(#) $Log: player.h,v $
// @(#) Revision 1.27  2003/12/08 07:01:41  virzak
// @(#) Player is no longer CWindowDependent, instead it is using
// @(#) CDisplayList member for display list
// @(#)
// @(#) Revision 1.26  2003/12/05 17:07:04  virzak
// @(#) Changed from CModule to CWindowDependent
// @(#)
// @(#) Revision 1.25  2003/10/21 04:27:57  virzak
// @(#) Mow calculating m_move_dir in accordance with the floor
// @(#)
// @(#) Revision 1.24  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.23  2003/10/09 00:22:12  virzak
// @(#) Removed an unnecessary member
// @(#)
// @(#) Revision 1.22  2003/10/05 23:33:44  virzak
// @(#) Better indentation
// @(#)
// @(#) Revision 1.21  2003/10/05 23:32:44  virzak
// @(#) Player now uses display lists.
// @(#) It is now a module that has to generate the display list every time
// @(#) window is created.
// @(#)
// @(#) Revision 1.20  2003/09/30 10:54:43  virzak
// @(#) CUI is a friend to allow position changing from a user command
// @(#)
// @(#) Revision 1.19  2003/09/21 06:12:24  virzak
// @(#) CPlayer::m_jumping was replaced by CKineticObject::m_inTheAir
// @(#) Changed m_fly to m_flyingMode
// @(#)
// @(#) Revision 1.18  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.17  2003/09/06 03:52:18  virzak
// @(#) CLight is a friend now
// @(#)
// @(#) Revision 1.16  2003/08/28 09:10:44  virzak
// @(#) Added friend CGUI2D
// @(#)
// @(#) Revision 1.15  2003/08/27 22:00:30  virzak
// @(#) CBackground is now a friend
// @(#)
// @(#) Revision 1.14  2003/08/20 02:46:24  virzak
// @(#) Contains time related functions
// @(#)
// @(#) Revision 1.13  2003/08/05 08:08:15  virzak
// @(#) Changed update call without argument (as now needed)
// @(#)
// @(#) Revision 1.12  2003/08/02 20:19:39  virzak
// @(#) Introduced new friend CCamera. CCamera is responsible for viewing
// @(#) what player sees.
// @(#)
// @(#) Revision 1.11  2003/07/30 08:11:33  virzak
// @(#) Added onDraw method
// @(#)
// @(#) Revision 1.10  2003/07/28 03:46:57  virzak
// @(#) Added update and onupdate method in object ans overrode it in
// @(#) player. Update calculates position, velocity, accel for an object
// @(#) every iteration
// @(#)
// @(#) Revision 1.9  2003/07/27 03:02:22  virzak
// @(#) First step to nodes. Player class extends node. Still no hierarchy
// @(#)
// @(#) Revision 1.8  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.7  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.6  2003/07/04 06:17:53  virzak
// @(#) Fixed flying mode and got rid of useless variables and routines
// @(#) that were used in the simple kinetics mode
// @(#)
// @(#) Revision 1.5  2003/07/03 22:22:45  virzak
// @(#) Added capabilities for acceleration, velocity.
// @(#) Tied the positioning to them in order to have more realistic movement
// @(#)
// @(#) Revision 1.4  2003/07/03 05:00:08  virzak
// @(#) Fixed jump-shooting
// @(#) Moved gravity from engine.h to world.h
// @(#) Player calculates its own jump instead of gr
// @(#)
// @(#) Revision 1.3  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
//


