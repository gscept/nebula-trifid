#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AnimEventAttachmentHandler

    This is the an animevent handler for handling attachment events

    (C) 2009 Radon Labs GmbH
	(C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "animation/animeventhandlerbase.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class AnimEventAttachmentHandler : public Animation::AnimEventHandlerBase
{
    __DeclareClass(AnimEventAttachmentHandler);

public:
    /// constructor
    AnimEventAttachmentHandler();
    /// destructor
    virtual ~AnimEventAttachmentHandler();  

    /// handle a event
    virtual bool HandleEvent(const Animation::AnimEventInfo& event);    
};
} // namespace EffectsFeature
//------------------------------------------------------------------------------