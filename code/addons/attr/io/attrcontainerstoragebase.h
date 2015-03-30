#pragma once
//------------------------------------------------------------------------------
/**
    @class Attr::AttrContainerStorageBase
  
    Provides read/write access to an attribute container 
        
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "util/guid.h"
#include "util/blob.h"
#include "attr/attributecontainer.h"
#include "io/stream.h"


//------------------------------------------------------------------------------
namespace Attr
{
class AttrContainerStorageBase : public Core::RefCounted
{
    __DeclareAbstractClass(AttrContainerStorageBase);

public:
    /// constructor
    AttrContainerStorageBase();
    /// destructor
    virtual ~AttrContainerStorageBase();

    /// save attributes 
    virtual void Save() = 0;
    /// load attributes 
    virtual void Load() = 0;

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

protected:
    Attr::AttributeContainer attrs;    
    Util::String table;
};

/**
*/
inline bool
AttrContainerStorageBase::HasAttr(const Attr::AttrId& attrId) const
{
    return this->attrs.HasAttr(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetString(const Attr::StringAttrId& attrId, const Util::String& str)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, str));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
AttrContainerStorageBase::GetString(const Attr::StringAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetString();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetInt(const Attr::IntAttrId& attrId, int val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline int
AttrContainerStorageBase::GetInt(const Attr::IntAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetFloat(const Attr::FloatAttrId& attrId, float val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline float
AttrContainerStorageBase::GetFloat(const Attr::FloatAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetBool(const Attr::BoolAttrId& attrId, bool val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline bool
AttrContainerStorageBase::GetBool(const Attr::BoolAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetFloat4(const Attr::Float4AttrId& attrId, const Math::float4& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4
AttrContainerStorageBase::GetFloat4(const Attr::Float4AttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetFloat4();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetMatrix44(const Attr::Matrix44AttrId& attrId, const Math::matrix44& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44
AttrContainerStorageBase::GetMatrix44(const Attr::Matrix44AttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetMatrix44();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetGuid(const Attr::GuidAttrId& attrId, const Util::Guid& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Guid&
AttrContainerStorageBase::GetGuid(const Attr::GuidAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetGuid();
}

//------------------------------------------------------------------------------
/**
*/
inline void
AttrContainerStorageBase::SetBlob(const Attr::BlobAttrId& attrId, const Util::Blob& val)
{
    this->attrs.SetAttr(Attr::Attribute(attrId, val));
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
AttrContainerStorageBase::GetBlob(const Attr::BlobAttrId& attrId) const
{
    return this->attrs.GetAttr(attrId).GetBlob();
}

}