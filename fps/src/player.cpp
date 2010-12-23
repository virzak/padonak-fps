//*****************************************************************************
// @(#) $Id: player.cpp,v 1.36 2003/12/08 07:01:41 virzak Exp $ 
// 
//  Description: 
//      Handles individual players and first person in particular
//
//  =
//
//*****************************************************************************

#include "player.h"
#include "engine.h"
#include "math.h"

const GrVector3D CPlayer::starting_vector=GrVector3D(0, 0, -1);
const GrMatrix4x4 CPlayer::right90 = GrMatrix4x4(-90, 'y');
const GrVector3D CPlayer::starting_right_vector = 
        CPlayer::right90 * CPlayer::starting_vector;



/* Speeds in meters/second */
const double speeds[5] = {
  8, 
  6, 
  6,
  6,
  5
};


const double ground_acceleration = 
    wld->getFrincionCoeff(GrPoint3D(0,0,0))*speeds[D_forward];
const double air_acceleration = ground_acceleration/48;


CPlayer::CPlayer(bool checking_collisions) : m_rotY(90), m_lookup(90),
      m_look_dir(starting_vector), m_move_dir(starting_vector),
      m_walkbias(0), m_walkbiasangle(0), m_eye_height(1.7f), m_height(1.8f),
      m_eye_height_vector(0, m_eye_height, 0), m_user_acceleration_dir(0),
      m_flyingMode(false), m_checkingCollisions(checking_collisions)
{
  if (!m_checkingCollisions) m_flyingMode = true;
  m_displayList = new CDisplayList(this);
  m_sphereSize = 0.3f;
  changeLook(0, 0, true);
  updateEye();

  load();

}


CPlayer::~CPlayer() {
  delete m_displayList;
}

void CPlayer::fire() {
  static long int next_fire_at = timer.getTime();
  
  if (timer.getTime()>=next_fire_at) {
    ((CSector *)parent())->shot(CGrRay3D(m_eye_pos, m_look_dir));
    //output->printf("fire\n");
    next_fire_at = timer.getTime() + 200;    
  }
}

void CPlayer::changeLook(double right_angle, double up_angle, bool force) {
  if (right_angle || force) {
    m_rotY -= right_angle;
    if (m_rotY<0.f) m_rotY += 360.f;
    if (m_rotY>=360.f) m_rotY -= 360.f;
    m_left_right_rot = GrMatrix4x4(m_rotY, 'y');
    
  }
  
  
  if (up_angle || force) {  
    m_lookup += up_angle;
    if (m_lookup>179.5) m_lookup = 179.5;
    if (m_lookup<.5) m_lookup = .5; 
    m_up_down_rot = GrMatrix4x4(m_lookup, 'x');  
  }
  m_look_dir = m_left_right_rot*(m_up_down_rot*GrVector3D(0,-1,0));
  
  genMoveVector();
  
  accel();
}


void CPlayer::genMoveVector() {
  
  // Find Hoprizontal movement directions
  m_move_dir        = m_left_right_rot * starting_vector;
  m_move_right_dir  = m_left_right_rot * starting_right_vector;
  
  GrVector3D norm;
  if (m_floor && (norm = m_floor->norm(m_pos)).y() != 1.f ) {
    m_move_dir.planeProject(norm);
    m_move_dir.normalize();
    m_move_right_dir.planeProject(norm);
    m_move_right_dir.normalize();
  }
}


void CPlayer::jump() {
    if (m_floor && !m_flyingMode) {
      m_velocity += GrVector3D(0,speeds[D_jumping],0);
      m_floor = NULL;
      CPlayer::accel();
    }        
}



void CPlayer::accel() {
  GrVector3D accel_dir = m_flyingMode?m_look_dir:m_move_dir;
  
  m_user_acceleration.zero();
  double accel_factor = (!m_floor&&!m_flyingMode)?
    air_acceleration:ground_acceleration;
  if (m_user_acceleration_dir & (1<<D_forward))
    m_user_acceleration += accel_dir;
  if (m_user_acceleration_dir & (1<<D_backward))
    m_user_acceleration -= accel_dir;
  if (m_user_acceleration_dir & (1<<D_left))
    m_user_acceleration -= m_move_right_dir;
  if (m_user_acceleration_dir & (1<<D_right))
    m_user_acceleration += m_move_right_dir;
  
  m_user_acceleration.normalize();
  m_user_acceleration *= accel_factor;
}



void CPlayer::moveRight(double disp) {
  m_pos = m_pos + disp*m_move_right_dir;
  updateEye();
}


void CPlayer::moveForward(double disp) {
    if (!m_flyingMode) {
      m_pos = m_pos + disp*m_move_dir;
    } else {
      m_pos = m_pos + disp*m_look_dir;
    }
    m_walkbiasangle += disp;
    m_walkbias = sin(m_walkbiasangle * piover180) / 20.0f;
    updateEye();
}



void CPlayer::onUpdate(unsigned int msecs) {

  static const CObject * last_floor_obj;
  
  m_acceleration.zero();
  m_acceleration = m_user_acceleration;

  // Gravity
  if (!m_flyingMode) {
    m_acceleration += GrVector3D(0, wld->getGravity(), 0);
  }

  // Dynamic drag - mainly friction and other forces opposing to the movement
  
  GrVector3D frictional_velocity(m_velocity);
  if (m_floor) {
     frictional_velocity-= m_floor->velocity();
  }


  // Apply only if we are moving
  if (frictional_velocity != (double) 0.0f) {

    // Apply dynamic drag (friction) if flying or on the ground
    if (m_floor || m_flyingMode) {

      // Apply friction if the speed is significant, otherwise set it to 0.
      if (frictional_velocity.sqmag() > 0.01) {
        
        
        //if (m_floor) frictional_velocity.y(0);
        GrVector3D friction= -wld->getFrincionCoeff(m_pos)*frictional_velocity;
        
        m_acceleration += friction;
      } else {
        // Reseting velocity
        if (m_floor) {
          m_velocity = m_floor->velocity();
        } else {
          m_velocity.zero();          
        }
      }
    }
  }
  
  //cout << timer.getChange() << "\n";
  //Have zeroes on pausing!
  
  if (msecs==0) msecs = timer.getChange();
  //CKineticObject::onUpdate();
  m_velocity += (double)msecs/1000.f*m_acceleration;
  
  GrVector3D changeInPos = (double)msecs/1000.f*m_velocity;
  
  
  double fraction = 1;
  if (changeInPos.length()>=m_sphereSize && m_checkingCollisions) {
    fraction = ((CSector *)parent())->checkMovementCollisions(this, 
                                                   changeInPos + m_sphereSize);
/*
    cout << "Checking\n";
  } else {
    cout << "Not checking\n";
*/
  }

  if (fraction == 1) {
    m_pos += changeInPos;
  }

  last_floor_obj = m_floor;
  m_floor = NULL;
  
  if (m_checkingCollisions)
    ((CSector *)parent())->checkCollisions(this);

  if (last_floor_obj != m_floor) {
    genMoveVector();
    accel();
  }
  
#if 0
  // Control velocity - We let the drag take care of this
  if (m_velocity.sqmag() > speeds[D_forward]*speeds[D_forward]) {
    output->printf("vel = %lf   speed =%lf\n", m_velocity.sqmag(), speeds[D_forward]);
    m_velocity.normalize();
    m_velocity *= speeds[D_forward];
  }
#endif
  
  updateEye();
}


    
//
// @(#) $Log: player.cpp,v $
// @(#) Revision 1.36  2003/12/08 07:01:41  virzak
// @(#) Player is no longer CWindowDependent, instead it is using
// @(#) CDisplayList member for display list
// @(#)
// @(#) Revision 1.35  2003/12/05 17:06:41  virzak
// @(#) small change
// @(#)
// @(#) Revision 1.34  2003/10/21 05:52:13  virzak
// @(#) Right movement vector is also determined from the floor
// @(#)
// @(#) Revision 1.33  2003/10/21 04:27:57  virzak
// @(#) Mow calculating m_move_dir in accordance with the floor
// @(#)
// @(#) Revision 1.32  2003/10/18 18:21:29  virzak
// @(#) Do movement checks if the velocity is big
// @(#)
// @(#) Revision 1.31  2003/10/18 11:20:12  virzak
// @(#) Now checking movement collisions
// @(#)
// @(#) Revision 1.30  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.29  2003/10/09 00:21:41  virzak
// @(#) Changed onUpdate to allow friction with horizontal elevator
// @(#)
// @(#) Revision 1.28  2003/10/05 23:32:44  virzak
// @(#) Player now uses display lists.
// @(#) It is now a module that has to generate the display list every time
// @(#) window is created.
// @(#)
// @(#) Revision 1.27  2003/10/04 23:03:39  virzak
// @(#) Fixed flying bug
// @(#) Rewrote update funciton
// @(#)
// @(#) Revision 1.26  2003/09/30 22:45:06  virzak
// @(#) Reseting velocity again.
// @(#) Dynamic frag only influences horizontal components of the velocity x,z
// @(#)
// @(#) Revision 1.25  2003/09/29 03:39:02  virzak
// @(#) Small potential bug
// @(#)
// @(#) Revision 1.24  2003/09/21 06:29:56  virzak
// @(#) CPlayer::m_jumping was replaced by CKineticObject::m_inTheAir
// @(#) Changed m_fly to m_flyingMode
// @(#) All update functions don't depend on whether the player is in the air -
// @(#) collision detection handles gravity
// @(#)
// @(#) Revision 1.23  2003/09/17 06:34:35  virzak
// @(#) Now checking collisions
// @(#)
// @(#) Revision 1.22  2003/09/17 03:44:22  virzak
// @(#) CSector::shot now takes CGrRay3D instead of a point and a Vector
// @(#)
// @(#) Revision 1.21  2003/09/10 03:40:41  virzak
// @(#) Small changes
// @(#)
// @(#) Revision 1.20  2003/08/20 02:40:31  virzak
// @(#) Inherits grom CKineticObject and uses its update routine
// @(#)
// @(#) Revision 1.19  2003/08/14 01:11:06  virzak
// @(#) Redefined m_lookup from looking down as 0 and up as 180, ranges 0.5-179.5
// @(#)
// @(#) Revision 1.18  2003/08/05 08:08:15  virzak
// @(#) Changed update call without argument (as now needed)
// @(#)
// @(#) Revision 1.17  2003/08/02 20:33:10  virzak
// @(#) Shot function calls shot of a higher level.
// @(#)
// @(#) Revision 1.16  2003/07/28 03:46:57  virzak
// @(#) Added update and onupdate method in object ans overrode it in
// @(#) player. Update calculates position, velocity, accel for an object
// @(#) every iteration
// @(#)
// @(#) Revision 1.15  2003/07/27 03:02:22  virzak
// @(#) First step to nodes. Player class extends node. Still no hierarchy
// @(#)
// @(#) Revision 1.14  2003/07/20 00:43:13  virzak
// @(#) Added tagra textures
// @(#)
// @(#) Revision 1.13  2003/07/19 22:05:14  virzak
// @(#) Different changes
// @(#)
// @(#) Revision 1.12  2003/07/13 06:10:29  virzak
// @(#) Fixed windows bug
// @(#)
// @(#) Revision 1.11  2003/07/11 23:20:59  virzak
// @(#) Added double * conversion to GrTuple and used it
// @(#)
// @(#) Revision 1.10  2003/07/11 04:03:23  virzak
// @(#) Got rid of getComponent where is made mode sence to put x(), y(), or z()
// @(#)
// @(#) Revision 1.9  2003/07/09 05:01:45  virzak
// @(#) fixed stains, removed old kinematics, inlined vector functions
// @(#) Made output support << operations as iostream does. needs fixes
// @(#)
// @(#) Revision 1.8  2003/07/04 08:25:05  virzak
// @(#) Made gcc compatible
// @(#)
// @(#) Revision 1.7  2003/07/04 07:56:09  virzak
// @(#) small fix
// @(#)
// @(#) Revision 1.6  2003/07/04 06:17:52  virzak
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


