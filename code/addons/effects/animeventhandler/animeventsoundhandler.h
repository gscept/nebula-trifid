#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AnimEventSoundHandler
    
    This is the sound animevent handler
    
    (C) 2009 Radon Labs GmbH
	(C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "animation/animeventhandlerbase.h"
#include "io/uri.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class AnimEventSoundHandler : public Animation::AnimEventHandlerBase
{
    __DeclareClass(AnimEventSoundHandler);
public:
    /// constructor
    AnimEventSoundHandler();
    /// destructor
    virtual ~AnimEventSoundHandler();  

    /// handle a event
    virtual bool HandleEvent(const Animation::AnimEventInfo& event);
};
} // namespace EffectsFeature
//------------------------------------------------------------------------------