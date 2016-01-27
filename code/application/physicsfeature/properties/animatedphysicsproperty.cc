//------------------------------------------------------------------------------
//  physicsfeature/properties/physicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeature/properties/animatedphysicsproperty.h"
#include "game/entity.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/physicsbody.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "core/factory.h"
#include "basegamefeature/basegameprotocol.h"
#include "physicsfeature/physicsprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "resources/resourcemanager.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "physics/resource/managedphysicsmodel.h"
#include "graphics/modelentity.h"
#include "characters/characterskeleton.h"
#include "characters/character.h"

namespace PhysicsFeature
{
using namespace Game;
using namespace Math;
using namespace BaseGameFeature;
using namespace Physics;
using namespace Util;

__ImplementClass(PhysicsFeature::AnimatedPhysicsProperty, 'APPR', TransformableProperty);

//------------------------------------------------------------------------------
/**
*/
AnimatedPhysicsProperty::AnimatedPhysicsProperty():
enabled(false),linked(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnimatedPhysicsProperty::~AnimatedPhysicsProperty()
{
	n_assert(this->physicsEntities.IsEmpty());
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will create and setup
    the required physics entities.
*/
void
AnimatedPhysicsProperty::OnActivate()
{
	TransformableProperty::OnActivate();
    
    // activate physics by default
    this->EnablePhysics();
}

//------------------------------------------------------------------------------
/** 
    Called when property is going to be removed from its game entity.
    This will release the physics entity owned by the game entity.
*/
void
AnimatedPhysicsProperty::OnDeactivate()
{
    if (this->IsEnabled())
    {
        this->DisablePhysics();
    }
    this->physicsEntities.Clear();
    TransformableProperty::OnDeactivate();
}


//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::SetupCallbacks()
{	
	this->entity->RegisterPropertyCallback(this, Render);
}


//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(SetTransform::Id);	
	this->RegisterMessage(EnableCollisionCallback::Id);	
	this->RegisterMessage(SetCollideCategory::Id);
	this->RegisterMessage(SetCollideCategoryMask::Id);
	this->RegisterMessage(Collision::Id);	
	this->RegisterMessage(ConnectToAnimation::Id);
	TransformableProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	n_assert(0 != msg);
	if (this->IsEnabled())
	{
		if (msg->CheckId(SetTransform::Id))
		{
			// set transform of physics entity
			SetTransform* transformMsg = (SetTransform*) msg.get();
			this->UpdateTransform(transformMsg->GetMatrix());
		}		
		else if (msg->CheckId(EnableCollisionCallback::Id))
		{
			Ptr<PhysicsFeature::EnableCollisionCallback> velMsg = msg.downcast<PhysicsFeature::EnableCollisionCallback>(); 			
			for(SizeT i=0;i<this->physicsEntities.Size();i++)
			{
				this->physicsEntities[i]->SetEnableCollisionCallback(velMsg->GetEnableCallback()); 			
			}
			
		}
		else if (msg->CheckId(ConnectToAnimation::Id))
		{
			Ptr<ConnectToAnimation> cmsg = msg.cast<ConnectToAnimation>();
			AttachToJoints(cmsg->GetJointArray(),cmsg->GetBodyArray());			
		}
		else if (msg->CheckId(Collision::Id))
		{

		}		
		else if (msg->CheckId(SetCollideCategory::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategory> velMsg = msg.downcast<PhysicsFeature::SetCollideCategory>();
			for(SizeT i=0;i<this->physicsEntities.Size();i++)
			{
				this->physicsEntities[i]->SetCollideCategory(velMsg->GetCategory());
			}			
		}
		else if (msg->CheckId(SetCollideCategoryMask::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategoryMask> velMsg = msg.downcast<PhysicsFeature::SetCollideCategoryMask>();
			for(SizeT i=0;i<this->physicsEntities.Size();i++)
			{
				this->physicsEntities[i]->SetCollideFilter(velMsg->GetCategoryMask());		
			}			
		}
	}

	// need to forward to parent
	TransformableProperty::HandleMessage(msg);
}


//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::DisablePhysics()
{
	n_assert(this->IsEnabled());

	for(SizeT i=0;i<this->physicsEntities.Size();i++)
	{
		PhysicsServer::Instance()->GetScene()->Detach(this->physicsEntities[i].cast<PhysicsObject>());		
	}
	this->physicsEntities.Clear();
	this->enabled = false;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::SetEnabled(bool setEnabled)
{
	if (this->enabled != setEnabled)
	{
		if (setEnabled)
		{
			this->EnablePhysics();
		}
		else
		{
			this->DisablePhysics();
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::OnRender()
{
	if(!this->linked)
	{
		if(this->entity->GetBool(Attr::JointNameLinking))
		{
			AttachByNames();
		}		
	}
	else
	{
		IndexT i;
		for(i=0;i<trackedJoints.Size();i++)
		{
			const Math::matrix44 &trans = this->entity->GetMatrix44(Attr::Transform);
			const Characters::CharJointInfo* joint = this->trackedModel->GetTrackedCharJointInfo(trackedJoints[i]);
			if(joint)
			{
				this->jointHash[trackedJoints[i]]->SetTransform(matrix44::multiply(this->jointOffsets[trackedJoints[i]], matrix44::multiply(joint->GetGlobalMatrix(), trans)));
			}
		}
	}	
	
}

//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::EnablePhysics()
{
	n_assert(!this->IsEnabled());
	if (this->entity->HasAttr(Attr::Graphics)
		&& this->entity->GetString(Attr::Graphics).IsValid())
	{
		Util::String path;
		path.Format("physics:%s.np3",this->entity->GetString(Attr::Graphics).AsCharPtr());
		Ptr<ManagedPhysicsModel> model = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI,path).cast<ManagedPhysicsModel>();
		Ptr<PhysicsObject> object;
	
		const Math::matrix44 &trans = this->entity->GetMatrix44(Attr::Transform);

		const Util::Array<Util::KeyValuePair<Util::String, Ptr<Physics::Collider>>> & colliders = model->GetModel()->GetColliders().Content();
		
		bool enableFeedback = false;
		if(this->entity->HasAttr(Attr::CollisionFeedback))
		{
			enableFeedback = this->entity->GetBool(Attr::CollisionFeedback);			
		}
		IndexT i;
		for(i=0;i<colliders.Size();i++)
		{
			PhysicsCommon c;							
			c.name = colliders[i].Key();
			c.collider = colliders[i].Value();
			c.type = PhysicsBody::RTTI.GetFourCC();
			c.mass = 1;
			c.bodyFlags = Physics::Kinematic;
			c.startTransform = Math::matrix44::identity();
			Ptr<PhysicsBody> body = PhysicsObject::CreateFromTemplate(c).cast<PhysicsBody>();	
			this->physicsEntities.Append(body);
			body->SetUserData(this->entity.cast<Core::RefCounted>());
			PhysicsServer::Instance()->GetScene()->Attach(body.cast<PhysicsObject>());
			this->bodyHash.Add(c.name,body);
			body->SetTransform(trans);
			body->SetEnableCollisionCallback(enableFeedback);
			
		}										
		this->enabled = true;		
	}					
	if(this->entity->GetBool(Attr::JointNameLinking))
	{
		Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();		
		this->GetEntity()->SendSync(msg.cast<Messaging::Message>());
		this->trackedModel = msg->GetEntity();

		this->AttachByNames();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::AttachToJoints(const Util::Array<Util::String>& joints, const Util::Array<Util::String>& bodies)
{
	n_assert2(!this->linked, "Already linked to joints");

	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();

	n_assert2(joints.Size() == bodies.Size(),"amount of joints and bodies does not match");
	this->GetEntity()->SendSync(msg.cast<Messaging::Message>());

	const Ptr<Graphics::ModelEntity>& model = msg->GetEntity();
    n_assert2(model.isvalid(), "No valid model found");
	this->trackedModel = model;
	IndexT i;
	for(i = 0; i<joints.Size();i++)
	{
		model->AddTrackedCharJoint(joints[i]);
		this->trackedJoints.Append(joints[i]);
		this->jointHash.Add(joints[i],bodyHash[bodies[i]]);
	}
	linked = true;
}
//------------------------------------------------------------------------------
/**
*/
void
AnimatedPhysicsProperty::AttachByNames()
{
	if(this->linked)
	{
		return;
	}
	if(this->trackedModel->IsValid())
	{
		Ptr<Graphics::ModelEntity> imodel = this->trackedModel;
		if(imodel->GetModelInstance().isvalid())
		{
			n_assert2(imodel->HasCharacter(), (Util::String("AnimatedPhysics model ") + imodel->GetResourceId().Value() + "has no skeleton!").AsCharPtr());
			const Characters::CharacterSkeleton & skeleton = imodel->GetCharacter()->Skeleton();			
			IndexT i;
			for(i=0;i<skeleton.GetNumJoints();i++)
			{
				const Characters::CharacterJoint& joint = skeleton.GetJoint(i);
				Util::StringAtom jointname = joint.GetName();
				if(this->bodyHash.Contains(jointname))
				{
					this->trackedModel->AddTrackedCharJoint(jointname);
					this->trackedJoints.Append(jointname);
					this->jointHash.Add(jointname,bodyHash[jointname]);
					this->jointOffsets.Add(jointname, joint.GetInvPoseMatrix());
				}
			}
			this->linked = true;		
		}		
	}	
}
}