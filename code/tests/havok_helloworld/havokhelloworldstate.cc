//------------------------------------------------------------------------------
//  havokhelloworldstate.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokhelloworldstate.h"
#include "havokhelloworldapplication.h"
#include "physics/physicsbody.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/collider.h"
#include "debugrender/debugtextrenderer.h"

using namespace BaseGameFeature;
using namespace Physics;
using namespace Havok;
using namespace Math;

namespace Tests
{
__ImplementClass(Tests::HavokHelloWorldState, 'HKSE',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
HavokHelloWorldState::HavokHelloWorldState()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokHelloWorldState::~HavokHelloWorldState()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
HavokHelloWorldState::OnFrame()
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

	Debug::DebugTextRenderer::Instance()->DrawText("Maya controls are used.\nNote that the object can be viewed through the havok visual debugger as well", Math::float4(1, 0, 1, 1), Math::float2(0.05f, 0.01f));

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
HavokHelloWorldState::OnStateEnter(const Util::String& prevState)
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
HavokHelloWorldState::OnStateLeave(const Util::String& nextState)
{
	this->ClosePhysicsStuff();

	this->camera = 0;

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHelloWorldState::InitPhysicsStuff()
{
	n_assert(PhysicsServer::HasInstance());
	this->physicsServer = PhysicsServer::Instance();

	this->physicsWorld = this->physicsServer->GetScene().get();

	// set zero gravity, we want the box to spin in place
	this->physicsWorld->SetGravity(vector(0, 0, 0));

	matrix44 ident = matrix44::identity();

	Ptr<HavokCollider> collider = HavokCollider::Create();
	collider->AddBox(vector(1, 2, 3), ident);

	// set up box template
	PhysicsCommon boxTemplate;
	boxTemplate.name = "box";
	boxTemplate.category = Default;
	boxTemplate.collider = collider.downcast<Collider>();
	boxTemplate.type = Physics::PhysicsBody::RTTI.GetFourCC();
	boxTemplate.mass = 1.0f;

	matrix44 positionMatrix = ident;
	positionMatrix.set_position(float4(0, 5, 0, 0));

	boxTemplate.startTransform = positionMatrix;

	this->testPhysicsBody = PhysicsBody::CreateFromTemplate(boxTemplate).downcast<PhysicsBody>();

	this->physicsWorld->Attach(this->testPhysicsBody.upcast<PhysicsObject>());

	// add a diagonal torque to the body
	this->testPhysicsBody->SetAngularVelocity(vector(1, 2, 0));
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHelloWorldState::ClosePhysicsStuff()
{
	this->testPhysicsBody = 0;
	this->physicsServer = 0;
}

}