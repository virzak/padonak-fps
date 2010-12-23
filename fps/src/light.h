//*****************************************************************************
// @(#) $Id: light.h,v 1.4 2003/10/18 08:00:10 virzak Exp $ 
// 
//  Description: 
//      Contains lighting code
//
//  =
//
//*****************************************************************************


#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "object.h"

class CLight : public CObject {
  void onDraw() const;
  void onUpdate(unsigned int msecs=0);
};



#endif /*__LIGHT_H__*/


//
// @(#) $Log: light.h,v $
// @(#) Revision 1.4  2003/10/18 08:00:10  virzak
// @(#) Fixed onUpdate routine
// @(#)
// @(#) Revision 1.3  2003/09/17 06:58:40  virzak
// @(#) changed an include from node.h to object.h
// @(#)
// @(#) Revision 1.2  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.1  2003/08/29 08:36:07  virzak
// @(#) Fisrt draft
// @(#)
//

