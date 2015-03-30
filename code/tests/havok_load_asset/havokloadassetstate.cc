//------------------------------------------------------------------------------
//  havokloadassetstate.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokloadassetstate.h"
#include "havokloadassetapplication.h"
#include "physics/physicsbody.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/collider.h"
#include "debugrender/debugtextrenderer.h"
#include "resources/resourcemanager.h"
#include "physicsfeature/physicsprotocol.h"

using namespace BaseGameFeature;
using namespace Physics;
using namespace Havok;
using namespace Math;

namespace Tests
{
__ImplementClass(Tests::HavokLoadAssetState, 'HKSE',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
HavokLoadAssetState::HavokLoadAssetState()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokLoadAssetState::~HavokLoadAssetState()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
HavokLoadAssetState::OnFrame()
{
	// draw some lines at origo for reference
	{
		const Math::matrix44 m = Math::matrix44::identity();
		const Math::float4 color(0.9f,0.9f,0.9f,1.0f);
		Math::point midCross[4];
		const float lineLength = 10;

		midCross[0] = Math::point(-lineLength, 0.0f, 0.0f);
		midCross[1] = Math::point(lineLength, 0.0f, 0.0f);
		midCross[2] = Math::point(0.0f, 0.0f, -lineLength);
		midCross[3] = Math::point(0.0f, 0.0f, lineLength);

		Debug::DebugShapeRenderer::Instance()->DrawPrimitives(m, CoreGraphics::PrimitiveTopology::LineList, 2, midCross, 4, color, CoreGraphics::RenderShape::CheckDepth);
	}

	Debug::DebugTextRenderer::Instance()->DrawText("Currently physics-meshes are not debug-rendered in n3 apps! Use the havok visual debugger instead.", Math::float4(1, 0, 1, 1), Math::float2(0.05f, 0.01f));

	if (this->physicsServer->GetScene().isvalid())
	{
		this->physicsServer->GetScene()->RenderDebug();
	}

	return GameStateHandler::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokLoadAssetState::OnStateEnter(const Util::String& prevState)
{
	// enter state, this creats am empty level (including physicsscene)
	GameStateHandler::OnStateEnter(prevState);

	// intialize camera
	this->camera = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Camera", "Camera");
	BaseGameFeature::EntityManager::Instance()->AttachEntity(this->camera);

	this->InitPhysicsStuff();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokLoadAssetState::OnStateLeave(const Util::String& nextState)
{
	this->camera = 0;
	this->sphere = 0;

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokLoadAssetState::InitPhysicsStuff()
{
	n_assert(PhysicsServer::HasInstance());
	this->physicsServer = PhysicsServer::Instance();

	this->physicsWorld = this->physicsServer->GetScene().get();

	this->physicsWorld->SetGravity(vector(0, 0, 0));

	// NOTE: you need to reconfigure the model, also need to create a havok_mopp/<folder> directory in the export dir
	this->sphere = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Simple", "systemsphere");
	BaseGameFeature::EntityManager::Instance()->AttachEntity(this->sphere);

	// set linear velocity on the object
	Ptr<PhysicsFeature::SetLinearVelocity> msg = PhysicsFeature::SetLinearVelocity::Create();
	msg->SetVelocity(Math::vector(-0.33f, -0.66f, -1.00f));
	this->sphere->SendSync(msg.upcast<Messaging::Message>());
}

}