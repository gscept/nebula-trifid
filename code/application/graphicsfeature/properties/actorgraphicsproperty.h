#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::ActorGraphicsProperty
    
    Graphics property for animated characters.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphicsfeature/properties/graphicsproperty.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class ActorGraphicsProperty : public GraphicsProperty
{
    __DeclareClass(ActorGraphicsProperty);
	__SetupExternalAttributes();
public:
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    /// setup graphics entities
    virtual void SetupGraphics();
};
__RegisterClass(ActorGraphicsProperty);

} // namespace GraphicsFeature
//------------------------------------------------------------------------------
    