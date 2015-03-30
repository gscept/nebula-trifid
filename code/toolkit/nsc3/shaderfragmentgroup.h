#pragma once
#ifndef TOOLS_SHADERFRAGMENTGROUP_H
#define TOOLS_SHADERFRAGMENTGROUP_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderFragmentGroup
    
    Defines a group of mutually-exclusive fragments which results in 
    multiple variations of the same shader. This is normally used
    for shader variations which are selected automatically at runtime
    by the shader (e.g. for different light types).
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "shaderfragment.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderFragmentGroup : public Core::RefCounted
{
    __DeclareClass(ShaderFragmentGroup);
public:
    /// constructor
    ShaderFragmentGroup();
    /// destructor
    virtual ~ShaderFragmentGroup();

    /// set the file URI this fragment group has been defined in
    void SetFileURI(const IO::URI& uri);
    /// get the file URI this fragment group has been defined in
    const IO::URI& GetFileURI() const;
    /// parse the fragment group from an open XmlReader
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);

    /// get the fragment group's name
    const Util::String& GetName() const;
    /// get the fragment group's category
    const Util::String& GetCategory() const;
    /// return whether this fragment group is exclusive (may only show up once in a shader)
    bool IsExclusive() const;
    /// return the description string
    const Util::String& GetDesc() const;
    /// get the shader fragments in the group
    const Util::Dictionary<Util::String, Ptr<ShaderFragment>>& GetFragments() const;

private:
    IO::URI fileUri;
    Util::String name;
    Util::String category;
    Util::String desc;
    bool isExclusive;
    Util::Dictionary<Util::String, Ptr<ShaderFragment>> fragments;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderFragmentGroup::SetFileURI(const IO::URI& uri)
{
    this->fileUri = uri;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShaderFragmentGroup::GetFileURI() const
{
    return this->fileUri;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragmentGroup::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragmentGroup::GetCategory() const
{
    return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragmentGroup::GetDesc() const
{
    return this->desc;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragmentGroup::IsExclusive() const
{
    return this->isExclusive;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, Ptr<ShaderFragment>>&
ShaderFragmentGroup::GetFragments() const
{
    return this->fragments;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif
