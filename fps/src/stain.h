//*****************************************************************************
// @(#) $Id: stain.h,v 1.16 2003/12/08 11:08:19 virzak Exp $ 
// 
//  Description: 
//      
//
//  =
//
//*****************************************************************************


#ifndef __STAINS_H__
#define __STAINS_H__

#include "util.h"
#include "object.h"
#include "textures.h"
#include "container.h"
#include "timer.h"
#include "polygon.h"

class CStain : public CQuad {
    GrVector3D m_fromBase;
  public:
    CStain(const GrPoint3D & pos, const GrVector3D &, const GrPoint3D &,
        GrVector3D up = GrVector3D(0,1,0));
    friend class CStainContainer;
};


class CStainContainer : public CObjectContainer<CStain> {
    CTexture * m_tex;
  public:
    CStainContainer(CTexture * tex = NULL) : m_tex(tex) {}

    void bindTexture(CTexture * tex) {m_tex=tex;}
    virtual void onDraw() const;
    virtual void onKeepIntegrity();
};







#endif /*__STAINS_H__*/

//
// @(#) $Log: stain.h,v $
// @(#) Revision 1.16  2003/12/08 11:08:19  virzak
// @(#) Changed CContainer to CObjectContainer as base class
// @(#)
// @(#) Revision 1.15  2003/10/18 04:27:00  virzak
// @(#) CStain is now derived from CQuad
// @(#)
// @(#) Revision 1.14  2003/10/18 03:00:52  virzak
// @(#) Changed onUpdate to onKeepIntegrity
// @(#)
// @(#) Revision 1.13  2003/10/18 00:20:50  virzak
// @(#) Moved the application wide times to timer.cpp and the extern to timer.h
// @(#)
// @(#) Revision 1.12  2003/09/30 06:58:51  virzak
// @(#) Reverted CStainContainer::onUpdate to previous version to preserve
// @(#) virtual behaviour
// @(#) Added CStain::m_fromBase member that is used to calculate vertices
// @(#) from the position of the container and changed CStainContainer::onUpdate
// @(#) to have that.
// @(#) Removed m_owner because of inheritance
// @(#)
// @(#) Revision 1.11  2003/09/29 03:44:35  virzak
// @(#) Added owner to CStainContainer class, maybe should be interited
// @(#)
// @(#) Revision 1.10  2003/09/21 06:39:19  virzak
// @(#) onDraw is now virtual
// @(#)
// @(#) Revision 1.9  2003/09/17 06:58:40  virzak
// @(#) changed an include from node.h to object.h
// @(#)
// @(#) Revision 1.8  2003/09/09 23:40:56  virzak
// @(#) Rewrote onDraw as const function
// @(#)
// @(#) Revision 1.7  2003/08/25 20:00:19  virzak
// @(#) Stain container keeps track of stain textures
// @(#) Stain keeps texture coordinates
// @(#)
// @(#) Revision 1.6  2003/08/24 20:02:19  virzak
// @(#) Changed CStainContainer to be a derived class of CContainer
// @(#) and changed all functions accordingly
// @(#)
// @(#) Revision 1.5  2003/08/24 06:19:49  virzak
// @(#) Added CNode interface to stains. Added drawing capabilities.
// @(#) Added class CStainContainer which is responsible for collecting stains
// @(#) and seting drawing state on stain drawing.
// @(#)
// @(#) Revision 1.4  2003/08/02 20:21:09  virzak
// @(#) Stain now also stores the normal
// @(#)
// @(#) Revision 1.3  2003/07/04 19:31:10  virzak
// @(#) Moved stains from world to triangle in order to be able to use
// @(#) GL_ALWAYS as depth function and not have these stains going to
// @(#) other polygons. GL_EQUAL doesn't work well.
// @(#)
// @(#) Revision 1.2  2003/07/04 18:57:02  virzak
// @(#) Make the stain more realistic
// @(#)
// @(#) Revision 1.1  2003/07/02 22:21:20  virzak
// @(#) Added stains from shooting.
// @(#) So far only points in the air.
// @(#)
//


