#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorLightProperty

    Based on the normal LightProperty, 
	additionally it renders a wireframe representation
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "graphicsfeature/properties/lightproperty.h"

//------------------------------------------------------------------------------
namespace Resources
{
    class ManagedMesh;
}

namespace LevelEditor2
{
class EditorLightProperty : public GraphicsFeature::LightProperty
{
	__DeclareClass(EditorLightProperty);
public:    
	EditorLightProperty();
    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
	/// called from Entity::DeactivateProperties()
	virtual void OnDeactivate();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called before rendering happens
    virtual void OnRender();
protected:
	virtual void UpdateLightTransform(const Math::matrix44& transform);

private:	
	Ptr<Graphics::BillboardEntity> billboard;
    Ptr<Resources::ManagedMesh> lightMesh;
};
__RegisterClass(EditorLightProperty);

}; // namespace EditorLightProperty
//------------------------------------------------------------------------------
