#pragma once
#ifndef VEGETATION_GRASSENTITYEVENTHANDLER_H
#define VEGETATION_GRASSENTITYEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
@class Vegetation::GrassEntityEventHandler


(C) 2008 Radon Labs GmbH
*/
#include "internalgraphics/internalgraphicsentityhandler.h"

namespace Vegetation
{
class GrassEntityEventHandler : public InternalGraphics::InternalGraphicsEntityHandler
{
    __DeclareClass(GrassEntityEventHandler)
public:
	/// constructor
	GrassEntityEventHandler();
	/// destructor
	virtual ~GrassEntityEventHandler();
    /// handle events
    virtual bool HandleEvent(const Ptr<InternalGraphics::InternalGraphicsEntityEvent>& event);
};
}
#endif // VEGETATION_GRASSENTITYEVENTHANDLER_H