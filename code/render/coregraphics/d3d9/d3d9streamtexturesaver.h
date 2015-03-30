#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9StreamTextureSaver
    
    D3D9/Xbox360 implementation of StreamTextureSaver.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/streamtexturesaverbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9StreamTextureSaver : public Base::StreamTextureSaverBase
{
    __DeclareClass(D3D9StreamTextureSaver);
public:
    /// called by resource when a save is requested
    virtual bool OnSave();
};

} // namespace Win360
//------------------------------------------------------------------------------
    