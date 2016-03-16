//------------------------------------------------------------------------------
//  game/property.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "game/property.h"
#include "game/entity.h"

namespace Game
{
__ImplementClass(Game::Property, 'GAPR' ,Messaging::Port);

//------------------------------------------------------------------------------
/**
*/
Property::Property() : active(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Property::~Property()
{
    n_assert(!this->entity.isvalid());
}

//------------------------------------------------------------------------------
/**
    If a property adds attributes to an entity, override this method to setup
    their default state. This method is called before the entity is even 
    initialized from the database. After this method, entity attributes may
    be overwritten from the database, and after that from a stream.
*/
void
Property::SetupDefaultAttributes()
{
    // empty, override in subclass as necessary
}

//------------------------------------------------------------------------------
/**
*/
void
Property::SetupExternalAttributes()
{
	// empty, override in subclass as necessary
}

//------------------------------------------------------------------------------
/**
    Tells the entity what per-frame callback methods should be called
    for this property. The method is called after SetupDefaultAttributes()
    by the entity, and the property is expected to call the 
    Entity::RegisterPropertyCallback() once for every callback method
    (OnBeginFrame(), OnMoveBefore(), ...) that should be called per-frame.
*/
void
Property::SetupCallbacks()
{
    // empty, derive this method in a subclass
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::Entity::ActivateProperties(). 
    Use this method for one-time initializations of the property.
*/
void
Property::OnActivate()
{
	n_assert(!this->IsActive());
	this->active = true;
}

//------------------------------------------------------------------------------
/**
    This method is called by Game::Entity::DeactivateProperties(). Use this 
    method to cleanup stuff which has been initialized in OnActivate().
*/
void
Property::OnDeactivate()
{
    n_assert(this->IsActive());
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::Load() after the
    entity attributes have been loaded from the database. You can
    override this method in a subclass if further initialization is needed
    for the property after attributes have been loaded from the database, but
    please be aware that this method may not be called if the entity is created
    directly.
*/
void
Property::OnLoad()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::OnStart(). This is the moment
    when the world is complete and the entity can establish connections to other
    entitys.
*/
void
Property::OnStart()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from within Game::Entity::Save() before the
    entity attributes will be saved to the database. You can override
    this method in a subclass if actions are needed before a save happens
    (this is usually the case if entity attributes need to be updated 
    by the property before saving).
*/
void
Property::OnSave()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnBeginFrame() on all 
    properties attached to an entity in the order of attachment. Override this 
    method if your property has to do any work at the beginning of the frame.
*/
void
Property::OnBeginFrame()
{
    n_error("Property::OnBeginFrame() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnMoveBefore() on all 
    properties attached to an entity in the order of attachment. Override this
    method if your property has any work to do before the physics subsystem
    is triggered.
*/
void
Property::OnMoveBefore()
{
    n_error("Property::OnMoveBefore() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnMoveAfter() on all
    properties attached to an entity in the order of attachment. Override this
    method if your property has any work to do after the physics subsystem
    has been triggered.
*/
void
Property::OnMoveAfter()
{
    n_error("Property::OnMoveAfter() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnRender() on all
    properties attached to an entity in the order of attachment. Override
    this method if your property has any work to do before rendering happens.
*/
void
Property::OnRender()
{
    n_error("Property::OnRender() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnRenderDebug() on all
    properties attached to an entity in the order of attachment. It's meant for debug
    issues. It will be called when debug mode is enabled.
*/
void
Property::OnRenderDebug()
{
    n_error("Property::OnRenderDebug() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnLoseActivity() on all
    properties attached to an entity in the order of attachment.
    It indicates that the entity will no longer be trigger, due to leaving
    the "Activity Bubble", i.e. the area of interest (most probably around the active camera).
*/
void
Property::OnLoseActivity()
{
    n_error("Property::OnLoseActivity() called (callback setup error?)");
}

//------------------------------------------------------------------------------
/**
    This method is called from Game::Entity::OnRenderDebug() on all
    properties attached to an entity in the order of attachment.
    It indicates that the entity will be trigger from now on, due to entering
    the "Activity Bubble", i.e. the area of interest (most probably around the active camera).
*/
void
Property::OnGainActivity()
{
    n_error("Property::OnGainActivity() called (callback setup error?)");
}


//------------------------------------------------------------------------------
/**
	Calls the property to check whenever it has deferred messages waiting to be handled
*/
void 
Property::OnHandleDeferred()
{
	if (!this->deferredMessages.IsEmpty())
	{
		this->HandleDeferredMessages();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
Property::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
Property::SetEntity(const Ptr<Entity>& e)
{
	n_assert(e != 0);
	n_assert(!this->HasEntity());
	this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
void
Property::ClearEntity()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
Property::HandleDeferredMessages()
{
	IndexT messageIndex;
	for (messageIndex = 0; messageIndex < this->deferredMessages.Size(); messageIndex++)
	{
		if (this->deferredMessages[messageIndex]->Handled())
		{
			this->HandleMessage(this->deferredMessages[messageIndex]);
			this->deferredMessages.EraseIndex(messageIndex);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Property::AddDeferredMessage( const Ptr<Messaging::Message>& msg )
{
	this->deferredMessages.Append(msg);
}
}; // namespace Game
