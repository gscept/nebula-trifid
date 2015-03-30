#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::AnimEventShakeHandler
    
    This is the godsend animevent handler, to handle camera shake events
    
    (C) 2009 Radon Labs GmbH
*/
#include "animation/animeventhandlerbase.h"

//------------------------------------------------------------------------------
namespace FX
{
class AnimEventShakeHandler : public Animation::AnimEventHandlerBase
{
    __DeclareClass(AnimEventShakeHandler);

public:
    /// constructor
    AnimEventShakeHandler();
    /// destructor
    virtual ~AnimEventShakeHandler();  

    /// handle a event
    virtual bool HandleEvent(const Animation::AnimEventInfo& event);
};
} // namespace FX
//------------------------------------------------------------------------------