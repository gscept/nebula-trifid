#pragma once
//------------------------------------------------------------------------------
/**
    @class Minimap::MinimapProperty    
      
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "messaging/staticmessagehandler.h"

//------------------------------------------------------------------------------
namespace Minimap
{
class MinimapProperty : public Game::Property
{
	__DeclareClass(MinimapProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    MinimapProperty();
    /// destructor
    virtual ~MinimapProperty();

    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    
private:

    friend class Messaging::StaticMessageHandler;   	   
	Util::String texture;
};
__RegisterClass(MinimapProperty);
}; // namespace Minimap
//------------------------------------------------------------------------------
