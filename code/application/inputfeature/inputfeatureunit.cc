//------------------------------------------------------------------------------
// inputfeatureunit.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "inputfeatureunit.h"
#include "game/gameserver.h"

using namespace Input;
namespace InputFeature
{

__ImplementClass(InputFeatureUnit, 'FINP', Game::FeatureUnit);
__ImplementSingleton(InputFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
InputFeatureUnit::InputFeatureUnit()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
InputFeatureUnit::~InputFeatureUnit()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnActivate()
{
	n_assert(!this->IsActive());
	this->inputServer = InputServer::Create();
	this->inputServer->Open();
	FeatureUnit::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnDeactivate()
{
	n_assert(this->IsActive());
	this->inputServer->Close();
	this->inputServer = 0;
	FeatureUnit::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnBeginFrame()
{
	FeatureUnit::OnBeginFrame();
	this->inputServer->BeginFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnFrame()
{
	FeatureUnit::OnFrame();
	this->inputServer->OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnEndFrame()
{
	if (this->inputServer->IsQuitRequested())
	{
		Game::GameServer::Instance()->SetQuitRequested();
	}
	this->inputServer->EndFrame();
	FeatureUnit::OnEndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
InputFeatureUnit::OnStart()
{
	FeatureUnit::OnStart();
	this->inputServer->Reset();
}

} // namespace InputFeature