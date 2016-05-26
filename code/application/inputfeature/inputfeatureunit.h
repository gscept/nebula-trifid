#pragma once
//------------------------------------------------------------------------------
/**
	The input feature unit is used to refresh and clear input.

	Input should be cleared at the beginning of the frame, and get update immediately after,
	making this feature unit viable to put into the feature unit stack whenever input should be fetched (usually the very start).
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "game/featureunit.h"
#include "input/inputserver.h"

namespace InputFeature
{
class InputFeatureUnit : public Game::FeatureUnit
{
	__DeclareClass(InputFeatureUnit);
	__DeclareSingleton(InputFeatureUnit);
public:
	/// constructor
	InputFeatureUnit();
	/// destructor
	virtual ~InputFeatureUnit();

	/// called from GameServer::ActivateProperties()
	virtual void OnActivate();
	/// called from GameServer::DeactivateProperties()
	virtual void OnDeactivate();

	/// called when the feature unit should start
	virtual void OnStart();

	/// called on begin of frame
	virtual void OnBeginFrame();
	/// called during frame
	virtual void OnFrame();
	/// called on end of frame
	virtual void OnEndFrame();
private:

	Ptr<Input::InputServer> inputServer;
};
} // namespace InputFeature