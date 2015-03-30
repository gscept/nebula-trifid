#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::GraphicsProperty

    This is the standard graphics property which adds visibility to a game 
    entity. 

    NOTE: There are cases where the graphics property may depend on a 
    physics property (for complex physics entities which require several 
    graphics entities to render themselves). Thus it is recommended that 
    physics properties are attached before graphics properties.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "game/entity.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphics/graphicsprotocol.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

//------------------------------------------------------------------------------
namespace Graphics
{
    class ModelEntity;
};

namespace GraphicsFeature
{
class GraphicsProperty : public Game::Property
{
	__DeclareClass(GraphicsProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    GraphicsProperty();
    /// destructor
    virtual ~GraphicsProperty();
    
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
	/// called before the frame
	virtual void OnMoveAfter();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);   
    /// override to provide a self managed graphics resource (default is Attr::Graphics)
    virtual const Util::String& GetGraphicsResource() const;
    /// called when game debug visualization is on
    virtual void OnRenderDebug();
	/// gets the node names
	const Util::Array<Util::String>& GetNodeNames() const;

protected:

    /// setup graphics
    virtual void SetupGraphics();
    /// discard graphics
    virtual void DiscardGraphics();

    /// setup anim event handling
    virtual void SetupAnimEvents(const Ptr<Graphics::ModelEntity>& entity);
    /// shutdown anim event handling
    virtual void RemoveAnimEvents(const Ptr<Graphics::ModelEntity>& entity);
    /// update the graphics entity's transform
    virtual void UpdateTransform(const Math::matrix44& m, bool setDirectly=false);
    // shows or hides all attached graphics entities
    void SetVisible(bool visible);
    /// on set overwrite color
    void OnSetOverwriteColor(const Ptr<SetOverwriteColor>& msg);
    /// set shader variable
    void OnSetShaderVariable(const Ptr<SetShaderVariable>& msg);
	/// set material variable
	void OnSetMaterialVariable(const Ptr<SetMaterialVariable>& msg);
	/// handles receving the model node list
	void OnReceiveModelNodes(const Ptr<Graphics::GetModelNodes>& msg);
	
    Ptr<Graphics::ModelEntity> modelEntity;
	Util::Array<Util::String> modelNodeNames;

    bool loadSync;
	bool nodesDirty;

};
__RegisterClass(GraphicsProperty);
//------------------------------------------------------------------------------
/**    
    Get the default graphics resource, which is Attr::Graphics.
    subclasses may override this to provide a self managed resource.
*/
inline const Util::String&
GraphicsProperty::GetGraphicsResource() const
{
    return (GetEntity()->GetString(Attr::Graphics));    
}


//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Util::String>& 
GraphicsProperty::GetNodeNames() const
{
	return this->modelNodeNames;
}

}; // namespace Property
//------------------------------------------------------------------------------

