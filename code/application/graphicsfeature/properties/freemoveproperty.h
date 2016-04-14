#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::FreeMoveProperty

    Allows free fly movement with use of the MoveRotate and MoveDirection messages
      
    (C) 2016 Individual contributors, see AUTHORS file
*/

#include "basegamefeature/properties/transformableproperty.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class FreeMoveProperty: public BaseGameFeature::TransformableProperty
{
	__DeclareClass(FreeMoveProperty);	
	__SetupExternalAttributes();
public:
    /// constructor
	FreeMoveProperty();
    /// destructor
    virtual ~FreeMoveProperty();
    
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
};
__RegisterClass(FreeMoveProperty);
}; // namespace GraphicsFeature
//------------------------------------------------------------------------------
