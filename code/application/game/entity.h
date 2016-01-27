#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::Entity

    The game entity class. A game entity represents one game object,
    such as an actor, an item, etc...

    The entity class itself doesn't have any game specific attributes or
    functionality. Instead it's configured by attaching properties to the
    entity. Properties add functionality to a game entity, for instance,
    if an entity should be visible, add a GraphicsProperty, if it should
    be audible, add an AudioProperty, and so forth...
    
    Any data used for initilizing any property or holding any information about 
    states etc. is stored in the attributeTable.

    (C) 2007 RadonLabs GmbH
	(C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "messaging/message.h"
#include "messaging/dispatcher.h"
#include "game/property.h"
#include "util/dictionary.h"
#include "appgame/appconfig.h"
#include "db/valuetable.h"
#include "basegamefeature/managers/categorymanager.h"
#include "debug/debugtimer.h"

namespace Messaging
{
    class Port;
}

namespace BaseGameFeature
{
    class FactoryManager;	
    class EntityManager;    
}

namespace GraphicsFeature
{
	class EnvEntityManager;
}

// sends message to game entity
#define __SendSync(OBJ, MSG) OBJ->SendSync(MSG.downcast<Messaging::Message>())

//------------------------------------------------------------------------------
namespace Game
{

class Entity : public Core::RefCounted
{
	__DeclareClass(Entity);
public:
    /// an Id type, used to identify entities
    typedef unsigned int EntityId;
    
    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    
    //=== basics ===

    /// get the entity's category
    const Util::String& GetCategory() const;
    /// get unique id of entity
    EntityId GetUniqueId() const;
    /// return true if any property accepts/processes message
    bool AcceptsMessage(const Messaging::Id& msgId) const;
    /// send a synchronous message to the entity
    void SendSync(const Ptr<Messaging::Message>& msg);
    /// return true if the entity is currently active (between OnActivate/OnDeactivate)
    bool IsActive() const;
    /// get the instance attribute table for the entity
    const Ptr<Db::ValueTable>& GetAttrTable() const;
    /// get the instance attribute table row index for the entity
    IndexT GetAttrTableRowIndex() const;
    /// find property by RTTI (performs IsA() check), returns invalid pointer if not exists, slow!
    Ptr<Property> FindProperty(const Core::Rtti& rtti) const;
	/// returns true if entity has a property with the given RTTI
	bool HasProperty(const Core::Rtti& rtti) const;
	/// Returns the entity's property list.
	const Util::Array<Ptr<Property>>& GetAllProperties() const {return properties;}

    //=== callbacks ===

    /// register a property callback, called by Property::SetupCallback() method
    void RegisterPropertyCallback(const Ptr<Property>& prop, Property::CallbackType callback);
    /// called when attached to world
    virtual void OnActivate();
    /// called when removed from world
    virtual void OnDeactivate();
    /// called at the beginning of the frame
    void OnBeginFrame();
    /// called before movement
    void OnMoveBefore();
    /// called after movement
    void OnMoveAfter();
    /// called before rendering
    void OnRender();
    /// debug rendering called before rendering 
    void OnRenderDebug();
    /// called if entity loses activity
    void OnLoseActivity();
    /// called if entity gains activity
    void OnGainActivity();
	/// called if entity has deferred messages
	void OnHandleDeferred();
    /// called after loading from database has happened
    void OnLoad();
    /// called when the entity starts to live in the complete world
    void OnStart();
    /// called before saving from database happens
    void OnSave();

    //=== attributes ===

    /// return true if entity has an attribute
    bool HasAttr(const Attr::AttrId& attrId) const;
    /// generic attribut setter (slow!)
    void SetAttr(const Attr::Attribute& attr);
    /// generic attribute getter (slow!)
    Attr::Attribute GetAttr(const Attr::AttrId& attr) const;
    /// generic attribut set with variant
    void SetAttrValue(const Attr::AttrId& attrId, const Util::Variant& val);
    /// set string attribute on the entity
    void SetString(const Attr::StringAttrId& attrId, const Util::String& s);
    /// get string attribute from the entity
    const Util::String& GetString(const Attr::StringAttrId& attrId) const;
    /// set int attribute on the entity
    void SetInt(const Attr::IntAttrId& attrId, int i);
    /// get int attribute from the entity
    int GetInt(const Attr::IntAttrId& attrId) const;
    /// set float attribute on the entity
    void SetFloat(const Attr::FloatAttrId& attrId, float f);
    /// get float attribute from the entity
    float GetFloat(const Attr::FloatAttrId& attrId) const;
    /// set bool attribute on the entity
    void SetBool(const Attr::BoolAttrId& attrId, bool b);
    /// get bool attribute from the entity
    bool GetBool(const Attr::BoolAttrId& attrId) const;
    /// set float4 attribute on the entity
    void SetFloat4(const Attr::Float4AttrId& attrId, const Math::float4& v);
    /// get float4 attribute from the entity
    const Math::float4 GetFloat4(const Attr::Float4AttrId& attrId) const;
    /// set matrix44 attribute on the entity
    void SetMatrix44(const Attr::Matrix44AttrId& attrId, const Math::matrix44& m);
    /// get matrix44 attribute from the entity
    const Math::matrix44 GetMatrix44(const Attr::Matrix44AttrId& attrid) const;
    /// set guid attribute on the entity
    void SetGuid(const Attr::GuidAttrId& attrId, const Util::Guid& guid);
    /// get guid attribute on the entity
    const Util::Guid& GetGuid(const Attr::GuidAttrId& attrId) const;
    /// set blob attribute on the entity
    void SetBlob(const Attr::BlobAttrId& attrId, const Util::Blob& blob);
    /// get blob attribute on the entity
    const Util::Blob& GetBlob(const Attr::BlobAttrId& attrId) const;

    /// add string attribute if not exists
    void AddString(const Attr::StringAttrId& attrId);
    /// add int attribute if not exists
    void AddInt(const Attr::IntAttrId& attrId);
    /// add float attribute if not exists
    void AddFloat(const Attr::FloatAttrId& attrId);
    /// add bool attribute if not exists
    void AddBool(const Attr::BoolAttrId& attrId);
    /// add float4 attribute if not exists
    void AddFloat4(const Attr::Float4AttrId& attrId);
    /// add matrix44 attribute if not exists
    void AddMatrix44(const Attr::Matrix44AttrId& attrId);
    /// add guid attribute if not exists
    void AddGuid(const Attr::GuidAttrId& attrId);
    /// add blob attribute if not exists
    void AddBlob(const Attr::BlobAttrId& attrId);

    /// these can only be called when the entity is not active
    /// mainly public to make custom factory managers easier
    /// attach a property to the entity
    void AttachProperty(const Ptr<Property>& prop);
    /// remove a property from the entity
    void RemoveProperty(const Ptr<Property>& prop);

private:
    friend class BaseGameFeature::FactoryManager;		
    friend class BaseGameFeature::EntityManager;    
    friend class GraphicsFeature::EnvEntityManager;

    /// set entity category
    void SetCategory(const Util::String& cat);    
    /// set the instance attribute table row index for the entity
    void SetAttrTableRowIndex(IndexT i);
    /// set the instance attribute table for the entity
    void SetAttrTable(const Ptr<Db::ValueTable>& t);
    /// cleanup the entity properties, called from OnDeactivate()
    void CleanupProperties();
    /// call OnActivate() on all properties
    void ActivateProperties();
    /// call OnDeactivate() on all properties
    void DeactivateProperties();

    Util::String category;
    Ptr<Messaging::Dispatcher> dispatcher;
    Ptr<Db::ValueTable> attrTable;
    IndexT attrTableRowIndex;
    Util::Array<Ptr<Property> > properties;
    Util::FixedArray<Util::Array<Ptr<Property> > > callbackProperties;
    EntityId uniqueId;

    static EntityId uniqueIdCounter;
    
    bool activated;
    bool isInOnActivate;
    bool isInOnDeactivate;

#if NEBULA3_ENABLE_PROFILING
    Util::Dictionary<Util::String, Ptr<Debug::DebugTimer> > propertyActivateDebugTimer;
    Util::Dictionary<Util::String, Ptr<Debug::DebugTimer> > propertyOnStartDebugTimer;
    Util::Dictionary<Util::String, Ptr<Debug::DebugTimer> > propertyOnBeginFrameDebugTimer;
    Util::Dictionary<Util::String, Ptr<Debug::DebugTimer> > propertyOnRenderDebugTimer;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetCategory(const Util::String& c)
{
    n_assert(c.IsValid());
    this->category = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
Entity::GetCategory() const
{
    return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetAttrTable(const Ptr<Db::ValueTable>& t)
{
    n_assert(0 != t);
    this->attrTable = t;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Db::ValueTable>&
Entity::GetAttrTable() const
{
    return this->attrTable;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetAttrTableRowIndex(IndexT rowIndex)
{
    this->attrTableRowIndex = rowIndex;
}


//------------------------------------------------------------------------------
/**
*/
inline IndexT
Entity::GetAttrTableRowIndex() const
{
    return this->attrTableRowIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Entity::IsActive() const
{
    return this->activated;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Entity::HasAttr(const Attr::AttrId& attrId) const
{
    if (this->attrTable.isvalid())
    {
        return this->attrTable->HasColumn(attrId);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetString(const Attr::StringAttrId& attrId, const Util::String& s)
{
    this->attrTable->SetString(attrId, this->attrTableRowIndex, s);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
Entity::GetString(const Attr::StringAttrId& attrId) const
{
    return this->attrTable->GetString(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetInt(const Attr::IntAttrId& attrId, int i)
{
    this->attrTable->SetInt(attrId, this->attrTableRowIndex, i);
}

//------------------------------------------------------------------------------
/**
*/
inline int
Entity::GetInt(const Attr::IntAttrId& attrId) const
{
    return this->attrTable->GetInt(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetFloat(const Attr::FloatAttrId& attrId, float f)
{
    this->attrTable->SetFloat(attrId, this->attrTableRowIndex, f);
}

//------------------------------------------------------------------------------
/**
*/
inline float
Entity::GetFloat(const Attr::FloatAttrId& attrId) const
{
    return this->attrTable->GetFloat(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetBool(const Attr::BoolAttrId& attrId, bool b)
{
    this->attrTable->SetBool(attrId, this->attrTableRowIndex, b);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Entity::GetBool(const Attr::BoolAttrId& attrId) const
{
    return this->attrTable->GetBool(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetFloat4(const Attr::Float4AttrId& attrId, const Math::float4& v)
{
    this->attrTable->SetFloat4(attrId, this->attrTableRowIndex, v);
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4
Entity::GetFloat4(const Attr::Float4AttrId& attrId) const
{
    return this->attrTable->GetFloat4(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetMatrix44(const Attr::Matrix44AttrId& attrId, const Math::matrix44& m)
{ 
    n_assert(m.getrow0().w() >= 0);

    this->attrTable->SetMatrix44(attrId, this->attrTableRowIndex, m);
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44
Entity::GetMatrix44(const Attr::Matrix44AttrId& attrId) const
{
    return this->attrTable->GetMatrix44(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetGuid(const Attr::GuidAttrId& attrId, const Util::Guid& g)
{
    this->attrTable->SetGuid(attrId, this->attrTableRowIndex, g);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Guid&
Entity::GetGuid(const Attr::GuidAttrId& attrId) const
{
    return this->attrTable->GetGuid(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SetBlob(const Attr::BlobAttrId& attrId, const Util::Blob& b)
{
    this->attrTable->SetBlob(attrId, this->attrTableRowIndex, b);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
Entity::GetBlob(const Attr::BlobAttrId& attrId) const
{
    return this->attrTable->GetBlob(attrId, this->attrTableRowIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline uint
Entity::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Entity::AcceptsMessage(const Messaging::Id& msgId) const
{
    return this->dispatcher->AcceptsMessage(msgId);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Entity::SendSync(const Ptr<Messaging::Message>& msg)
{
    this->dispatcher->HandleMessage(msg);
}

} // namespace Game
//------------------------------------------------------------------------------
