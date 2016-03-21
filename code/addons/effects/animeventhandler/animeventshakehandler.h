#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AnimEventShakeHandler
    
    This is an animevent handler that handles camera shake events
    
    (C) 2009 Radon Labs GmbH
	(C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "animation/animeventhandlerbase.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
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
} // namespace EffectsFeature
//------------------------------------------------------------------------------