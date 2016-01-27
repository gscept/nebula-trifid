#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::LightProbeProperty
	
	A light probe property adds a reflection and irradiance projection decal to the entity.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/property.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "graphics/modelentity.h"
#include "graphics/lightprobeentity.h"

namespace GraphicsFeature
{
class LightProbeProperty : public Game::Property
{
	__DeclareClass(LightProbeProperty);
	__SetupExternalAttributes();
public:
	/// constructor
	LightProbeProperty();
	/// destructor
	virtual ~LightProbeProperty();

	/// called from when property gets set up
	virtual void OnActivate();
	/// called from when property gets set up
	virtual void OnDeactivate();
	/// override to register accepted messages
	virtual void SetupAcceptedMessages();
	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
	Ptr<Graphics::LightProbeEntity> lightProbeEntity;
	Ptr<Lighting::EnvironmentProbe> environmentProbe;
};
__RegisterClass(LightProbeProperty);
} // namespace GraphicsFeature