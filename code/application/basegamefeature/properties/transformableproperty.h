#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::TransformableProperty

    Entites with this property can be transformed.
  
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class TransformableProperty : public Game::Property
{
	__DeclareClass(TransformableProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    TransformableProperty();
    /// destructor
    virtual ~TransformableProperty();
    
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
};
__RegisterClass(TransformableProperty);
}; // namespace Properties
//------------------------------------------------------------------------------
