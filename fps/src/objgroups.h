//*****************************************************************************
// @(#) $Id: objgroups.h,v 1.1 2003/09/17 07:35:38 virzak Exp $ 
// 
//  Description: 
//      Initialisations for 3d or 2d drawing
//
//  =
//
//*****************************************************************************




#ifndef __OBJGROUPS_H__
#define __OBJGROUPS_H__

#include "object.h"

class C3DObjects : public CObject {
  public:
    void onDraw() const;
};

 
class CGUI2D : public CObject {
    void drawHelpBar() const;
  public:
    void onDraw() const;
};


#endif /*__OBJGROUPS_H__*/

//
// @(#) $Log: objgroups.h,v $
// @(#) Revision 1.1  2003/09/17 07:35:38  virzak
// @(#) Moved classes C3DObjects and CGUI2D from world.cpp world.h to
// @(#) objgroups.cpp objgroups.h
// @(#)
//

