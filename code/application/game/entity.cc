//------------------------------------------------------------------------------
//  game/entity.cc
//  (C) 2003 RadonLabs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attr/attributedefinition.h"
#include "game/entity.h"
#include "math/transform44.h"
#include "core/factory.h"
#include "app/application.h"
#include "managers/categorymanager.h"
#include "managers/entitymanager.h"
#include "debug/debugserver.h"

namespace Game
{
__ImplementClass(Game::Entity, 'GAEN', Core::RefCounted);

using namespace BaseGameFeature;

uint Entity::uniqueIdCounter = 0;

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() :
    attrTableRowIndex(InvalidIndex),
    uniqueId(++uniqueIdCounter),
    activated(false),
    isInOnActivate(false),
    isInOnDeactivate(false)
{
    this->callbackProperties.SetSize(Property::NumCallbackTypes);
    this->dispatcher = Messaging::Dispatcher::Create();
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    n_assert(!this->activated);
    n_assert(!this->attrTable.isvalid());
    this->dispatcher = 0;
}

//------------------------------------------------------------------------------
/**
    This method cleans up the entity. It is called from OnDeactivate().
    Override this method to cleanup any subclass specific stuff when the
    entity is removed from the level.
*/
void
Entity::CleanupProperties()
{
    // deactivate and remove all properties
    this->DeactivateProperties();
    while (this->properties.Size() > 0)
    {
        this->RemoveProperty(this->properties.Back());
    }
	#if NEBULA3_ENABLE_PROFILING
	propertyActivateDebugTimer.Clear();
	propertyOnStartDebugTimer.Clear();
	propertyOnBeginFrameDebugTimer.Clear();
	#endif
}

//------------------------------------------------------------------------------
/**
    Called when the game entity has been attached to a game level object.
    This will attach contained subsystem entities to their respective
    subsystems.

    @param  l   pointer to a level object to which the entity was attached

    - 01-Apr-05 floh    graphics entity now created after physics entity
*/
void
Entity::OnActivate()
{
    n_assert(!this->activated);
    n_assert(!this->isInOnActivate);
    this->isInOnActivate = true;
    n_assert(!this->isInOnDeactivate);
    
    // bind the entity to our category instance
    if (this->attrTable.isvalid())
    {
        CategoryManager::Entry entry(this->category, this->attrTable, this->attrTableRowIndex);
        CategoryManager::Instance()->SetInstanceEntity(entry, this);
    }

    // activate all properties
    this->ActivateProperties();
    
    // set activated flag
    this->activated = true;
    this->isInOnActivate = false;
}

//------------------------------------------------------------------------------
/**
    Called when the game entity has been removed from the game level object.
    This will remove any contained subsystem entities from their subsystems.
*/
void
Entity::OnDeactivate()
{
    n_assert(this->activated);
    n_assert(!this->isInOnDeactivate);

    this->isInOnDeactivate = true;
    n_assert(!this->isInOnActivate);
    this->activated = false;

    // cleanup properties
    this->CleanupProperties();

    // unbind the entity to our category instance
    if (this->attrTable.isvalid())
    {
        CategoryManager::Entry entry(this->category, this->attrTable, this->attrTableRowIndex);
        CategoryManager::Instance()->SetInstanceEntity(entry, 0);
        this->attrTable = 0;
        this->attrTableRowIndex = InvalidIndex;
    }

    // clear activated flag
    this->isInOnDeactivate = false;
}

//------------------------------------------------------------------------------
/**
    Called on game entities at the begin of the frame.
*/
void
Entity::OnBeginFrame()
{    
    n_assert(this->IsActive());

    // call properties that have registered for the BeginFrame callback
    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::BeginFrame];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
    #if NEBULA3_ENABLE_PROFILING
        Util::String timerName = props[i]->GetRtti()->GetName() + ".OnBeginFrame";
		this->propertyOnBeginFrameDebugTimer[timerName]->StartAccum();        
    #endif
        props[i]->OnBeginFrame();   

    #if NEBULA3_ENABLE_PROFILING
		this->propertyOnBeginFrameDebugTimer[timerName]->StopAccum();        
    #endif
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities before movement.
*/
void
Entity::OnMoveBefore()
{
    n_assert(this->IsActive());

    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::MoveBefore];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnMoveBefore();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities after movement.
*/
void
Entity::OnMoveAfter()
{
    n_assert(this->IsActive());

    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::MoveAfter];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnMoveAfter();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities before rendering.
*/
void 
Entity::OnRender()
{
    n_assert(this->IsActive());

    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::Render];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnRender();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }

#if NEBULA3_ENABLE_PROFILING
    IndexT idx;
    for (idx = 0; idx < this->propertyOnBeginFrameDebugTimer.Size(); ++idx)
    {
        this->propertyOnBeginFrameDebugTimer.ValueAtIndex(idx)->ResetAccum();
    } 
    for (idx = 0; idx < this->propertyActivateDebugTimer.Size(); ++idx)
    {
        this->propertyActivateDebugTimer.ValueAtIndex(idx)->ResetAccum();
    } 
    for (idx = 0; idx < this->propertyOnStartDebugTimer.Size(); ++idx)
    {
        this->propertyOnStartDebugTimer.ValueAtIndex(idx)->ResetAccum();
    } 
#endif
}

//------------------------------------------------------------------------------
/**
    Called on game entities before rendering.
*/
void 
Entity::OnRenderDebug()
{
    n_assert(this->IsActive());

    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::RenderDebug];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnRenderDebug();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities at the begin of the frame.
*/
void
Entity::OnLoseActivity()
{
    n_assert(this->IsActive());

    // call properties that have registered for the LoseActivity callback
    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::LoseActivity];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnLoseActivity();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
    Called on game entities if gaining/regaining activity
*/
void
Entity::OnGainActivity()
{
    n_assert(this->IsActive());

    // call properties that have registered for the LoseActivity callback
    const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::GainActivity];
    IndexT i;
    SizeT num = props.Size();
    for (i = 0; i < num; i++)
    {
        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
        #endif

        props[i]->OnGainActivity();        

        #if __ENTITY_STATS__
        this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
        #endif
    }
}

//------------------------------------------------------------------------------
/**
	Called on game entity if it has deferred messages waiting
*/
void 
Entity::OnHandleDeferred()
{
	n_assert(this->IsActive());

	// call properties that have registered for the LoseActivity callback
	const Util::Array<Ptr<Property> >& props = this->callbackProperties[Property::HandleDeferred];
	IndexT i;
	SizeT num = props.Size();
	for (i = 0; i < num; i++)
	{
#if __ENTITY_STATS__
		this->propertyProfilersByClass[props[i]->GetRtti()].StartAccum();
#endif

		props[i]->OnHandleDeferred();        

#if __ENTITY_STATS__
		this->propertyProfilersByClass[props[i]->GetRtti()].StopAccum();
#endif
	}
}

//------------------------------------------------------------------------------
/**
    This method is called after the game world has been loaded from the
    database. At the time when this method is called all entities
    in the world have already been created and their attributes have been
    loaded from the database.
    
    This method reads the entity attributes from the world database. A
    valid GUID attribute must exist on the entity for identification in
    the database. After the attributes are loaded from the world database,
    the Property::OnLoad() method will be called on all attached properties.
*/
void
Entity::OnLoad()
{
    // let properties do their post-loading-stuff
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        if (this->IsActive())
        {
            this->properties[i]->OnLoad();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called in 2 cases:

    When a level is loaded it is called on all entities after OnLoad when the
    complete world already exist.

    When a entity is created at ruTime (while a level is active) OnStart is
    called after the entity is attached to level.
*/
void
Entity::OnStart()
{
    // let propreties do there OnStart init stuff
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        if (this->IsActive())
        {
#if NEBULA3_ENABLE_PROFILING
            Util::String timerName(this->properties[i]->GetRtti()->GetName() + ".OnStart");
			this->propertyOnStartDebugTimer[timerName]->StartAccum();        
#endif
            this->properties[i]->OnStart();
#if NEBULA3_ENABLE_PROFILING
			this->propertyOnStartDebugTimer[timerName]->StopAccum();        
#endif
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method writes the current entity attributes back to the world database.
    At least a valid GUID attribute must exist on the entity for
    identification. If no entry exists yet in the database for the 
    entity, a new one will be created. Before the attributes are written back
    to the database, the method Property::OnSave() will be called on each
    attached property.
*/
void
Entity::OnSave()
{
    int i;
    int num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        if (this->IsActive())
        {
            this->properties[i]->OnSave();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called from within Property::SetupCallbacks() to register
    per-frame callback methods with the entity.
*/
void
Entity::RegisterPropertyCallback(const Ptr<Property>& prop, Property::CallbackType callbackType)
{
    n_assert(0 != prop);
    
    // ignore double entries, this could happen if the method
    // is called from several property subclasses
    if (InvalidIndex == this->callbackProperties[callbackType].FindIndex(prop))
    {
        this->callbackProperties[callbackType].Append(prop);
    }
}

//------------------------------------------------------------------------------
/**
    Attach a new property to the entity. This method must not be called
    while the entity is active.
*/
void
Entity::AttachProperty(const Ptr<Property>& prop)
{
    n_assert(0 != prop);
    n_assert(!this->IsActive());
    
    // only add the entity if it is compatible with our entity pool
    // and it should be added to entities in the current state.
    this->properties.Append(prop);
    prop->SetEntity(this);

    #if NEBULA3_ENABLE_PROFILING
    // create timer for activation
    Util::String timerName(prop->GetRtti()->GetName() + ".OnActivate");
    Ptr<Debug::DebugTimer> debugtimer = Debug::DebugServer::Instance()->GetDebugTimerByName(timerName);
    if (!debugtimer.isvalid())
    {
        debugtimer = Debug::DebugTimer::Create();
        debugtimer->Setup(timerName, "Entity");
    }
    this->propertyActivateDebugTimer.Add(timerName, debugtimer);    

    // create timer for on begin frame
    timerName = prop->GetRtti()->GetName() + ".OnBeginFrame";
    debugtimer = Debug::DebugServer::Instance()->GetDebugTimerByName(timerName);
    if (!debugtimer.isvalid())
    {
        debugtimer = Debug::DebugTimer::Create();
		debugtimer->Setup(timerName, "Entity");
    }
    this->propertyOnBeginFrameDebugTimer.Add(timerName, debugtimer);  

    // create timer for on start
    timerName = prop->GetRtti()->GetName() + ".OnStart";
    debugtimer = Debug::DebugServer::Instance()->GetDebugTimerByName(timerName);
    if (!debugtimer.isvalid())
    {
        debugtimer = Debug::DebugTimer::Create();
		debugtimer->Setup(timerName, "Entity");
    }
    this->propertyOnStartDebugTimer.Add(timerName, debugtimer);  
    #endif
}

//------------------------------------------------------------------------------
/**
    Remove a property from the entity. If the property is not attached to the entity, 
    a hard error will be thrown. This method must not be called while
    the entity is active.
*/
void
Entity::RemoveProperty(const Ptr<Property>& prop)
{
    n_assert(0 != prop);
    n_assert(!this->IsActive());
    IndexT propIndex = this->properties.FindIndex(prop);
    if (InvalidIndex != propIndex)
    {
        // delete from callbacks arrays first
        IndexT i;
        for (i = 0; i < Property::NumCallbackTypes; i++)
        {           
            IndexT callbackIndex = this->callbackProperties[i].FindIndex(prop);
            if (InvalidIndex != callbackIndex)
            {
                this->callbackProperties[i].EraseIndex(callbackIndex);
            }
        }

        // delete property
        n_assert(!this->properties[propIndex]->IsActive());
		this->properties[propIndex]->ClearEntity();

#if NEBULA3_ENABLE_PROFILING
        // remove timer ptr
        Util::String timerName(this->properties[propIndex]->GetRtti()->GetName() + ".OnActivate");
        if (this->propertyActivateDebugTimer[timerName]->GetRefCount() == 2)
        {
            this->propertyActivateDebugTimer[timerName]->Discard();
        }
        this->propertyActivateDebugTimer[timerName] = 0;

        timerName = this->properties[propIndex]->GetRtti()->GetName() + ".OnBeginFrame";
        if (this->propertyOnBeginFrameDebugTimer[timerName]->GetRefCount() == 2)
        {
            this->propertyOnBeginFrameDebugTimer[timerName]->Discard();
        }
        this->propertyOnBeginFrameDebugTimer[timerName] = 0;

        timerName = this->properties[propIndex]->GetRtti()->GetName() + ".OnStart";
        if (this->propertyOnStartDebugTimer[timerName]->GetRefCount() == 2)
        {
            this->propertyOnStartDebugTimer[timerName]->Discard();
        }
        this->propertyOnStartDebugTimer[timerName] = 0;
#endif

        this->properties.EraseIndex(propIndex);
    }
    else
    {
        n_error("Entity::RemoveProperty: Property '%s' does not exist on entity!", prop->GetClassName().AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
    This method calls OnActivate() on all attached properties.
*/
void
Entity::ActivateProperties()
{
    CategoryManager::Instance()->BeginAddCategoryAttrs(this->category);
    IndexT i;
    SizeT num = this->properties.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<Property>& prop = this->properties[i];
#if NEBULA3_ENABLE_PROFILING
        Util::String timerName(prop->GetRtti()->GetName() + ".OnActivate");
		this->propertyActivateDebugTimer[timerName]->StartAccum();        
#endif
        n_assert(!prop->IsActive());
        prop->SetupAcceptedMessages();

#if NEBULA3_ENABLE_PROFILING
	    this->propertyActivateDebugTimer[timerName]->StopAccum();
#endif
    }
    CategoryManager::Instance()->EndAddCategoryAttrs();
    
    // handle the other property initialisations
    for (i = 0; i < num; i++)
    {
        const Ptr<Property>& prop = this->properties[i];
        n_assert(!prop->IsActive());
        
        this->dispatcher->AttachPort(prop.upcast<Messaging::Port>());
        prop->SetupCallbacks();

    #if NEBULA3_ENABLE_PROFILING
        Util::String timerName(prop->GetRtti()->GetName() + ".OnActivate");
		this->propertyActivateDebugTimer[timerName]->StartAccum();        
    #endif
        // activate property
        prop->OnActivate();      

    #if NEBULA3_ENABLE_PROFILING
		this->propertyActivateDebugTimer[timerName]->StopAccum();
    #endif
    }
}

//------------------------------------------------------------------------------
/**
    This method calls OnDeactivate() on all attached properties.
*/
void
Entity::DeactivateProperties()
{
    // clear property callbacks
    IndexT i;
    for (i = 0; i < Property::NumCallbackTypes; i++)
    {
        this->callbackProperties[i].Clear();
    }

    // deactivate properties
    for (i = 0; i < this->properties.Size(); i++)
    {
        n_assert(this->properties[i]->IsActive());        
        this->properties[i]->OnDeactivate();
        this->dispatcher->RemovePort(this->properties[i].upcast<Messaging::Port>());
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Property>
Entity::FindProperty(const Core::Rtti& rtti) const
{
    IndexT i;
    for (i = 0; i < this->properties.Size(); i++)
    {
        if (this->properties[i]->IsA(rtti))
        {
            return this->properties[i];
        }
    }
    // fallthrough: not found, return invalid ptr
    return Ptr<Property>();
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::HasProperty(const Core::Rtti& rtti) const
{
	IndexT i;
	for (i = 0; i < this->properties.Size(); i++)
	{
		if (this->properties[i]->IsA(rtti)) return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddBool(const Attr::BoolAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddMatrix44(const Attr::Matrix44AttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddGuid(const Attr::GuidAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddBlob(const Attr::BlobAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddFloat4(const Attr::Float4AttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddFloat(const Attr::FloatAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddInt(const Attr::IntAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
		CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::AddString(const Attr::StringAttrId& attrId)
{
    if (!this->attrTable->HasColumn(attrId))
    {
        CategoryManager::Instance()->AddCategoryAttr(attrId, false);
    }
}

//------------------------------------------------------------------------------
/**
    Generic attribute setter method. This is slower then the specialied
    attribute setter methods!
*/
void
Entity::SetAttr(const Attr::Attribute& attr)
{
    switch (attr.GetValueType())
    {
        case Attr::IntType:
            this->SetInt(attr.GetAttrId(), attr.GetInt());
            break;
        case Attr::FloatType:
            this->SetFloat(attr.GetAttrId(), attr.GetFloat());
            break;
        case Attr::BoolType:
            this->SetBool(attr.GetAttrId(), attr.GetBool());
            break;
        case Attr::Float4Type:
            this->SetFloat4(attr.GetAttrId(), attr.GetFloat4());
            break;
        case Attr::StringType:
            this->SetString(attr.GetAttrId(), attr.GetString());
            break;
        case Attr::Matrix44Type:
            this->SetMatrix44(attr.GetAttrId(), attr.GetMatrix44());
            break;
        case Attr::BlobType:
            this->SetBlob(attr.GetAttrId(), attr.GetBlob());
            break;
        case Attr::GuidType:
            this->SetGuid(attr.GetAttrId(), attr.GetGuid());
            break;
        default:
            n_error("Game::Entity::SetAttr(): invalid attribute type!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Generic attribute getter method. This is slower then the specialied
    attribute getter methods!
*/
Attr::Attribute
Entity::GetAttr(const Attr::AttrId& attrId) const
{
    switch (attrId.GetValueType())
    {
        case Attr::IntType:
            return Attr::Attribute(attrId, this->GetInt(attrId));
        case Attr::FloatType:
            return Attr::Attribute(attrId, this->GetFloat(attrId));
        case Attr::BoolType:
            return Attr::Attribute(attrId, this->GetBool(attrId));
        case Attr::Float4Type:
            return Attr::Attribute(attrId, this->GetFloat4(attrId));
        case Attr::StringType:
            return Attr::Attribute(attrId, this->GetString(attrId));
        case Attr::Matrix44Type:
            return Attr::Attribute(attrId, this->GetMatrix44(attrId));
        case Attr::BlobType:
            return Attr::Attribute(attrId, this->GetBlob(attrId));
        case Attr::GuidType:
            return Attr::Attribute(attrId, this->GetGuid(attrId));
        default:
            n_error("Game::Entity::SetAttr(): invalid attribute type!");
            return Attr::Attribute();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Entity::SetAttrValue(const Attr::AttrId& attrId, const Util::Variant& val)
{
    this->attrTable->SetVariant(attrId, this->attrTableRowIndex, val);
}


} // namespace Game