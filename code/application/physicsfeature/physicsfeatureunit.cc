//------------------------------------------------------------------------------
//  game/PhysicsFeatureUnit.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeatureunit.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "game/gameserver.h"
#include "basegametiming/systemtimesource.h"
#include "physics/scene.h"
#include "physics/physicsbody.h"
#include "physics/physicsserver.h"
#include "io/ioserver.h"
#include "physics/visualdebuggerserver.h"

// include all properties for known by managers::factorymanager
#include "physicsfeature/properties/physicsproperty.h"
#include "physicsfeature/properties/animatedphysicsproperty.h"
#include "properties/actorphysicsproperty.h"
#include "properties/pointnclickinputproperty.h"
#include "properties/mousegripperproperty.h"
#include "properties/environmentcollideproperty.h"
#include "properties/triggerproperty.h"
#include "physicsfeature/physicsprotocol.h"
#include "scripting/scriptserver.h"

namespace PhysicsFeature
{
__ImplementClass(PhysicsFeatureUnit, 'FPH2' , Game::FeatureUnit);
__ImplementSingleton(PhysicsFeatureUnit);

using namespace Physics;
using namespace Game;

//------------------------------------------------------------------------------
/**
*/
PhysicsFeatureUnit::PhysicsFeatureUnit():
	syncVDBCamera(false),
	initVisualDebugger(true)
{
    __ConstructSingleton;
    _setup_timer(PhysicsFeatureUpdatePhysics);
}

//------------------------------------------------------------------------------
/**
*/
PhysicsFeatureUnit::~PhysicsFeatureUnit()
{
    _discard_timer(PhysicsFeatureUpdatePhysics);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();

    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("physics", "export:physics"));

	// setup physics server
    this->physicsServer = PhysicsServer::Create();
	this->physicsServer->SetInitVisualDebuggerFlag(this->initVisualDebugger);
    this->physicsServer->Open();
	this->physicsServer->AddCollisionReceiver(this);

	if(Scripting::ScriptServer::HasInstance())
	{
		Scripting::ScriptServer::Instance()->CreateFunctionTable("triggerentry_table");
		Scripting::ScriptServer::Instance()->CreateFunctionTable("triggerexit_table");
		Scripting::ScriptServer::Instance()->CreateFunctionTable("triggerinside_table");
		Scripting::ScriptServer::Instance()->CreateFunctionTable("onmouseenter_table");
		Scripting::ScriptServer::Instance()->CreateFunctionTable("onmouseleave_table");
		Scripting::ScriptServer::Instance()->CreateFunctionTable("onmousebutton_table");
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnDeactivate()
{
	// make sure graphics is initialized first!
	this->physicsServer->RemoveCollisionReceiver(this);
	this->physicsServer->Close();
    this->physicsServer = 0;

    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnLoad()
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnFrame()
{    
    _start_timer(PhysicsFeatureUpdatePhysics);

    // update time in physics system
    Timing::Time time = BaseGameFeature::SystemTimeSource::Instance()->GetTime();
    this->physicsServer->SetTime(time);
    // trigger physics system, steps physics world etc.
    this->physicsServer->Trigger();

	if (GraphicsFeature::GraphicsFeatureUnit::HasInstance() && this->IsSyncingVisualDebuggerCamera())
	{
		const Ptr<Graphics::View>& curView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
		Ptr<Graphics::CameraEntity> camera = curView->GetCameraEntity();
		
		if (camera.isvalid())
		{
			Math::matrix44 camTransform = camera->GetTransform();
			Physics::VisualDebuggerServer::Instance()->SetCameraView(camTransform);
		}
	}

    _stop_timer(PhysicsFeatureUpdatePhysics);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnRenderDebug()
{
    Physics::PhysicsServer::Instance()->RenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::CreateDefaultPhysicsWorld()
{   
    Ptr<Scene> physicsLevel = Scene::Create();
    PhysicsServer::Instance()->SetScene(physicsLevel);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::CleanupPhysicsWorld()
{	
    PhysicsServer::Instance()->SetScene(0);    
}

//------------------------------------------------------------------------------
/**
*/
bool 
PhysicsFeatureUnit::OnCollision(const Ptr<Physics::PhysicsObject> & receiver, const Ptr<Physics::PhysicsObject> & collidedWith, const Ptr<Physics::Contact> & c)
{
	Ptr<Collision> collmsg = Collision::Create();
	collmsg->SetOtherEntity(collidedWith->GetUserData()->object.cast<Game::Entity>());
	collmsg->SetContact(c);
	receiver->GetUserData()->object.cast<Game::Entity>()->SendSync(collmsg.cast<Messaging::Message>());
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsFeatureUnit::SetInitVisualDebuggerFlag(bool flag)
{
	// this must be done before OnActivate is called!
	n_assert(!this->IsActive());

	this->initVisualDebugger = flag;
}

//------------------------------------------------------------------------------
/**
*/
bool 
PhysicsFeatureUnit::GetInitVisualDebuggerFlag() const
{
	return this->initVisualDebugger;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnBeforeLoad()
{
	this->CreateDefaultPhysicsWorld();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnBeforeCleanup()
{
	this->CleanupPhysicsWorld();
}

}; // namespace PhysicsFeature
