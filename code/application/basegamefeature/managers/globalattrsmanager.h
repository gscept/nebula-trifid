#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::GlobalAttrsManager
  
    Provides read/write access to global attributes. 
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "game/manager.h"
#include "util/guid.h"
#include "util/blob.h"
#include "core/singleton.h"
#include "attr/attributecontainer.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class GlobalAttrsManager : public Game::Manager
{
    __DeclareClass(GlobalAttrsManager);
    __DeclareSingleton(GlobalAttrsManager);
public:
    /// constructor
    GlobalAttrsManager();
    /// destructor
    virtual ~GlobalAttrsManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// explicitely save attributes to database (not through OnSave!)
    void SaveAttributes(); 
    /// explicitely load attributes from database (not through OnLoad()!)
    void LoadAttributes();

    /// return true if global attribute exists
    bool HasAttr(const Attr::AttrId& attrId) const;
    /// set a global string attribute
    void SetString(const Attr::StringAttrId& attrId, const Util::String& value);
    /// get a global string attribute
    const Util::String& GetString(const Attr::StringAttrId& attrId) const;
    /// set a global int attribute
    void SetInt(const Attr::IntAttrId& attrId, int value);
    /// get a global int attribute
    int GetInt(const Attr::IntAttrId& attrId) const;
    /// set a global float attribute
    void SetFloat(const Attr::FloatAttrId& attrId, float value);
    /// get a global float attribute
    float GetFloat(const Attr::FloatAttrId& attrId) const;
    /// set a global bool attribute
    void SetBool(const Attr::BoolAttrId& attrId, bool value);
    /// get a global bool attribute
    bool GetBool(const Attr::BoolAttrId& attrId) const;
    /// set a global float4 attribute
    void SetFloat4(const Attr::Float4AttrId& attrId, const Math::float4& value);
    /// get a global float4 attribute
    const Math::float4 GetFloat4(const Attr::Float4AttrId& attrId) const;
    /// set a global matrix44 attribute
    void SetMatrix44(const Attr::Matrix44AttrId& attrId, const Math::matrix44& value);
    /// get a global matrix44 attribute
    const Math::matrix44 GetMatrix44(const Attr::Matrix44AttrId& attrId) const;
    /// set a global guid attribute
    void SetGuid(const Attr::GuidAttrId& attrId, const Util::Guid& guid);
    /// get a global guid attribute
    const Util::Guid& GetGuid(const Attr::GuidAttrId& attrId) const;
    /// set a global blob attribute
    void SetBlob(const Attr::BlobAttrId& attrId, const Util::Blob& blob);
    /// get a global blob attribute
    const Util::Blob& GetBlob(const Attr::BlobAttrId& attrId) const;

private:
    Attr::AttributeContainer attrs;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
GlobalAttrsManager::HasAttr(const Attr::AttrId& attrId) const
{
    return this->attrs.HasAttr(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetString(const Attr::StringAttrId& attrId, const Util::String& str)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, str));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
GlobalAttrsManager::GetString(const Attr::StringAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetString();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetInt(const Attr::IntAttrId& attrId, int val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline int
GlobalAttrsManager::GetInt(const Attr::IntAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetFloat(const Attr::FloatAttrId& attrId, float val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline float
GlobalAttrsManager::GetFloat(const Attr::FloatAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetBool(const Attr::BoolAttrId& attrId, bool val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GlobalAttrsManager::GetBool(const Attr::BoolAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetFloat4(const Attr::Float4AttrId& attrId, const Math::float4& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4
GlobalAttrsManager::GetFloat4(const Attr::Float4AttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetFloat4();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetMatrix44(const Attr::Matrix44AttrId& attrId, const Math::matrix44& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44
GlobalAttrsManager::GetMatrix44(const Attr::Matrix44AttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetMatrix44();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetGuid(const Attr::GuidAttrId& attrId, const Util::Guid& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Guid&
GlobalAttrsManager::GetGuid(const Attr::GuidAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetGuid();
}

//------------------------------------------------------------------------------
/**
*/
inline void
GlobalAttrsManager::SetBlob(const Attr::BlobAttrId& attrId, const Util::Blob& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
GlobalAttrsManager::GetBlob(const Attr::BlobAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetBlob();
}

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
