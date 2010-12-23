//*****************************************************************************
// @(#) $Id: background.h,v 1.4 2003/10/18 05:07:28 virzak Exp $ 
// 
//  Description: 
//      Handles Background drawing
//
//  =
//
//*****************************************************************************


#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "kobject.h"

class CBackground : public CKineticObject {
    GrPoint2D m_tl;
    GrPoint2D m_br;
    GrPoint2D m_tr;
    GrPoint2D m_bl;
    
  public:
    void onDraw() const;
    void onUpdate(unsigned int msecs=0);
};


#endif //__BACKGROUND_H__


//
// @(#) $Log: background.h,v $
// @(#) Revision 1.4  2003/10/18 05:07:28  virzak
// @(#) onUpdate now accepts unsigned int parameter (time change in milliseconds)
// @(#)
// @(#) Revision 1.3  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.2  2003/08/29 08:09:02  virzak
// @(#) Divided the calculations between update and draw evenly
// @(#)
// @(#) Revision 1.1  2003/08/27 22:16:48  virzak
// @(#) First try
// @(#)
//


