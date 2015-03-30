//------------------------------------------------------------------------------
//  havokplayerteststate.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokplayerteststate.h"
#include "havokplayertestapplication.h"
#include "physics/physicsbody.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/collider.h"
#include "physics/staticobject.h"
#include "physics/character.h"
#include "debugrender/debugtextrenderer.h"

using namespace BaseGameFeature;
using namespace Physics;
using namespace Havok;
using namespace Math;

namespace Tests
{
__ImplementClass(Tests::HavokPlayerTestState, 'HKSE',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
HavokPlayerTestState::HavokPlayerTestState()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokPlayerTestState::~HavokPlayerTestState()
{
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
HavokPlayerTestState::OnFrame()
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

	//Debug::DebugTextRenderer::Instance()->DrawText("Maya controls are used.\nNote that the object can be viewed through the havok visual debugger as well", Math::float4(1, 0, 1, 1), Math::float2(0.05f, 0.01f));

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
HavokPlayerTestState::OnStateEnter(const Util::String& prevState)
{
	// enter state, this creats am empty level (including physicsscene)
	GameStateHandler::OnStateEnter(prevState);

	// intialize camera
	this->camera = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Camera", "Camera");
	BaseGameFeature::EntityManager::Instance()->AttachEntity(this->camera);

	this->InitPhysicsStuff();

	// init player
	this->player = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Player", "demochar_havoktest");
	BaseGameFeature::EntityManager::Instance()->AttachEntity(this->player);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestState::OnStateLeave(const Util::String& nextState)
{
	this->ClosePhysicsStuff();

	this->camera = 0;

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestState::InitPhysicsStuff()
{
	n_assert(PhysicsServer::HasInstance());
	this->physicsServer = PhysicsServer::Instance();

	this->physicsWorld = this->physicsServer->GetScene().get();
	this->physicsWorld->SetGravity(vector(0, -18, 0));

	// set up the static environment
	Ptr<HavokCollider> collider = HavokCollider::Create();
	collider->AddBox(vector(50, 2, 50), matrix44::identity());
	collider->AddBox(vector(4, 4, 4), matrix44::translation(vector(-8, 2.1f+4, 8)));
	collider->AddBox(vector(3, 2, 3), matrix44::translation(vector(8, 2.1f+2, -8)));
	collider->AddBox(vector(4, 1.0f, 4), matrix44::translation(vector(-10, 2.1f+1.0f, -8)));

	PhysicsCommon groundTemplate;
	groundTemplate.name = "environment";
	groundTemplate.category = Default;
	groundTemplate.collider = collider.downcast<Collider>();
	groundTemplate.type = Physics::StaticObject::RTTI.GetFourCC();
	groundTemplate.mass = 1.0f;

	groundTemplate.startTransform = matrix44::translation(0, -2, 0);
	this->groundBox = PhysicsBody::CreateFromTemplate(groundTemplate).downcast<StaticObject>();
	this->physicsWorld->Attach(this->groundBox.upcast<PhysicsObject>());

	// make a dummy shape
	collider = HavokCollider::Create();
	collider->AddCapsule(vector(0, 2, 0), vector(0, -2, 0), 1.0f, matrix44::translation(vector(0, 5, 0)));
	collider->AddBox(vector(2.5f, 0.2f, 0.2f), matrix44::translation(0, 5+2, 0));
	collider->AddBox(vector(0.2f, 0.2f, 2.5f), matrix44::translation(0, 5+2, 0));

	PhysicsCommon dummyTemplate;
	dummyTemplate.category = Default;
	dummyTemplate.collider = collider.downcast<Collider>();
	dummyTemplate.type = Physics::PhysicsBody::RTTI.GetFourCC();

	IndexT i;
	for (i = 0; i < 3; i++)
	{
		dummyTemplate.startTransform = matrix44::translation(20, 5, (scalar)((i-1) * 10));
		dummyTemplate.name.Format("Dummy %d", i + 1);
		const float weightMul = 100.0f;
		dummyTemplate.mass = 1.0f + weightMul*i;

		Ptr<PhysicsBody> dummy = PhysicsBody::CreateFromTemplate(dummyTemplate).downcast<PhysicsBody>();	
		this->physicsWorld->Attach(dummy.upcast<PhysicsObject>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestState::ClosePhysicsStuff()
{
	this->groundBox = 0;
	this->player = 0;
	this->physicsServer = 0;
}

}