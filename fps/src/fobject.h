//*****************************************************************************
// @(#) $Id: fobject.h,v 1.2 2003/12/05 16:57:36 virzak Exp $ 
// 
//  Description: 
//      Interface to an object that serves as ground or floor
//
//  =
//
//*****************************************************************************


#ifndef __FOBJECT_H__
#define __FOBJECT_H__

#include "object.h"

class CFloorObject : virtual public CObject {
  public:
    //virtual GrVector3D getNorm() const = 0;
};



#endif /*__FOBJECT_H__*/



//
// @(#) $Log: fobject.h,v $
// @(#) Revision 1.2  2003/12/05 16:57:36  virzak
// @(#) Removed getNorm from fobject.h
// @(#)
// @(#) Revision 1.1  2003/10/21 04:28:41  virzak
// @(#) This class contains floor information
// @(#)
//


