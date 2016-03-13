#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::AnimPathProperty
	
	This property uses two anim paths to animate the rotation and translation of the object.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/property.h"
#include "math/float4.h"
#include "graphicsfeature/animpath/pathanimation.h"
#include "graphicsfeature/animpath/managedpathanimation.h"

namespace GraphicsFeature
{
class AnimPathProperty : public Game::Property
{
	__DeclareClass(AnimPathProperty);
	__SetupExternalAttributes();
public:
	/// constructor
	AnimPathProperty();
	/// destructor
	virtual ~AnimPathProperty();
	
	/// setup callbacks
	void SetupCallbacks();
	/// setup messages
	void SetupAcceptedMessages();

	/// handle property activation
	void OnActivate();
	/// handle property deactivation
	void OnDeactivate();
	/// handle begin-frame events
	void OnBeginFrame();
	/// handle messages
	void HandleMessage(const Ptr<Messaging::Message>& msg);
private:

	/// update transform using current sample
	void Update();

	bool paused;
	float lastSample;
	Math::float4 pos;
	Math::matrix44 baseRotation;
	Ptr<PathAnimationTrack> currentTrack;
	Ptr<ManagedPathAnimation> animPath;
};
__RegisterClass(AnimPathProperty);
} // namespace GraphicsFeature