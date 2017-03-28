#pragma once
//------------------------------------------------------------------------------
/**
	@class LevelEditor2::EditorLightProbeProperty
	
	Level editor version of a light probe property. Works as a placeholder from which we render light probes which are then used in game.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/property.h"
#include "graphicsfeature/properties/lightprobeproperty.h"
#include "environmentprobecapturer.h"
namespace LevelEditor2
{
class EditorLightProbeProperty : public GraphicsFeature::LightProbeProperty
{
	__DeclareClass(EditorLightProbeProperty);
	__SetupExternalAttributes();
public:
	/// constructor
	EditorLightProbeProperty();
	/// destructor
	virtual ~EditorLightProbeProperty();

	/// override callback registering
	virtual void SetupCallbacks();
	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
	/// called from Entity::DeactivateProperties()
	virtual void OnDeactivate();
	/// override to register accepted messages
	virtual void SetupAcceptedMessages();
	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
	/// called before rendering happens
	virtual void OnRender();

private:

	Ptr<Graphics::ModelEntity> reflectionMapPreview;
	Ptr<EnvironmentProbeCapturer> environmentCapturer;
};
__RegisterClass(EditorLightProbeProperty);

} // namespace LevelEditor2