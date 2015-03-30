//------------------------------------------------------------------------------
//  stategraphicsproperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/stategraphicsproperty.h"
#include "game/entity.h"
#include "graphicsutil/segmentedgfxutil.h"
#include "graphics/stage.h"
#include "graphicsfeatureunit.h"
#include "graphics/graphicsserver.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "stateobjectfeature/stateobjectprotocol.h"

namespace StateObjectFeature
{
__ImplementClass(StateObjectFeature::StateGraphicsProperty, 'STGP', GraphicsFeature::GraphicsProperty);

//------------------------------------------------------------------------------
/** 
*/
StateGraphicsProperty::StateGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
*/
StateGraphicsProperty::~StateGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Attach the property to a game entity. This will create and setup
    the required graphics entities.
*/
void
StateGraphicsProperty::OnActivate()
{
    Game::Property::OnActivate();
}

//------------------------------------------------------------------------------
/**    
*/
void
StateGraphicsProperty::OnDeactivate()
{
    // cleanup
    this->curState.Clear();

    GraphicsFeature::GraphicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
StateGraphicsProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(StateObjectFeature::InternalSwitchActiveState::Id);
    this->RegisterMessage(BaseGameFeature::UpdateTransform::Id);
    this->RegisterMessage(GraphicsFeature::SetGraphicsVisible::Id);
    this->RegisterMessage(StateObjectFeature::Resources::Id);

    GraphicsFeature::GraphicsProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
StateGraphicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(0 != msg);
    if (msg->CheckId(StateObjectFeature::InternalSwitchActiveState::Id))
    {
        this->OnSwitchActiveState(msg.cast<StateObjectFeature::InternalSwitchActiveState>()->GetStateName());
    }
    else if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
    {
        this->UpdateTransforms();
    }
    else if (msg->CheckId(StateObjectFeature::Resources::Id))
    {
        this->OnLoadResources(msg.cast<StateObjectFeature::Resources>()->GetResources());
    }
    else
    {
        GraphicsFeature::GraphicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateGraphicsProperty::OnSwitchActiveState(const Util::String& newState)
{    
    if (newState != this->curState)
    {            
        // first deactivate the previous state
        if (this->curState.IsValid())
        {
            // TODO: send setvisible msg to graphics entities
            this->curState = "";
            Util::String fullResourceName("mdl:");
            fullResourceName.Append(newState);
            fullResourceName.Append(".n3");

            for (int i = 0; i < this->graphicsEntities.Size(); i++)
            {
                if(this->graphicsEntities[i]->GetResourceId().Value() != fullResourceName)
                {
                    this->graphicsEntities[i]->SetVisible(false);
                #if NEBULA3_DEBUG
                    n_printf("StateObject: Deactivate state %s \n", this->graphicsEntities[i]->GetResourceId().Value());
                #endif
                }
            }
        }

        // activate new state
        if (newState.IsValid())
        {
            // TODO: set node of graphics entity visible
            this->curState = newState;
            Util::String fullResourceName("mdl:");
            fullResourceName.Append(this->curState);
            fullResourceName.Append(".n3");
            
            for (int i = 0; i < this->graphicsEntities.Size(); i++)
            {
                if(this->graphicsEntities[i]->GetResourceId().Value() == fullResourceName)
                {
                    this->graphicsEntities[i]->SetVisible(true);
                #if NEBULA3_DEBUG
                    n_printf("StateObject: Activate state %s \n", this->graphicsEntities[i]->GetResourceId().Value());
                #endif
                }
            }
        }
    }

    // in any way, update the graphics entity transforms
    this->UpdateTransforms();    
}

//------------------------------------------------------------------------------
/**
    This transfers the physics entity transforms into the graphics entity
    transforms.
*/
void
StateGraphicsProperty::UpdateTransforms()
{
    if (this->curState.IsValid())
    {
        // TODO?
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateGraphicsProperty::OnLoadResources(const Util::Dictionary<Util::String, StateInfo>& resources)
{
    if (resources.Size() > 0)
    {
        const Ptr<Graphics::Stage>& stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();

        // fallthrough: setup physics-less graphics entity
        const Math::matrix44& worldMatrix = GetEntity()->GetMatrix44(Attr::Transform);
        GraphicsFeature::SegmentedGfxUtil segGfxUtil;
        //in forschleife
        IndexT i;
        for (i = 0; i < resources.KeysAsArray().Size(); i++)
        {
            this->graphicsEntities.AppendArray(segGfxUtil.CreateAndSetupGraphicsEntities(resources.KeysAsArray()[i], worldMatrix, this->GetEntity()->GetUniqueId()));            
        }
        for (i = 0; i < this->graphicsEntities.Size(); i++)
        {
            this->graphicsEntities[i]->SetVisible(false);
            stage->AttachEntity(this->graphicsEntities[i].cast<Graphics::GraphicsEntity>());
        }
    }
}

} // namespace StateObjectFeature