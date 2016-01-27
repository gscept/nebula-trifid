#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::ResourceInfo

    ResourceInfo stores several informations for a certain Resource, i.e. height, widht, ...
    This class is just an interface and empty.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceInfo
{
public:
    /// constructor
    ResourceInfo(void);
    ResourceInfo(const ResourceInfo* info);
    /// destructor
    ~ResourceInfo(void);

    /// returns true if elements are equal (overloaded by subclasses)
    virtual bool IsEqual(const ResourceInfo* info) const;

    /// returns estimated resource-size in memory in bytes
    uint GetSize() const;
    /// sets estimated resource-size in memory in bytes
    void SetSize(uint size);

protected:
    uint estimatedSize;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceInfo::IsEqual(const ResourceInfo* info) const
{
    n_error("equality-operator of ResourceInfo called. Do not call this operator directly - overload it in subclasses");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
ResourceInfo::GetSize() const
{
    return this->estimatedSize;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceInfo::SetSize(uint size)
{
    this->estimatedSize = size;
}

} // namespace Resources
//------------------------------------------------------------------------------
