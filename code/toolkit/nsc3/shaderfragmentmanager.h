#pragma once
#ifndef TOOLS_SHADERFRAGMENTMANAGER_H
#define TOOLS_SHADERFRAGMENTMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderFragmentManager
    
    Load and parse shader fragments in the shader database and, and offer
    information about shader fragments.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "shaderfragment.h"
#include "shaderfragmentgroup.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderFragmentManager : public Core::RefCounted
{
    __DeclareClass(ShaderFragmentManager);
    __DeclareSingleton(ShaderFragmentManager);
public:
    /// constructor
    ShaderFragmentManager();
    /// destructor
    virtual ~ShaderFragmentManager();
    /// load fragments and fragment groups from the provided path
    bool LoadFragmentsAndGroups(const IO::URI& path);
    /// return true if a fragment exists
    bool HasFragment(const Util::String& name) const;
    /// return true if a fragment group exists
    bool HasFragmentGroup(const Util::String& name) const;
    /// get access to a fragment
    const Ptr<ShaderFragment>& GetFragment(const Util::String& name) const;
    /// begin a group fragment iteration
    bool BeginIterateGroupFragments();
    /// continue group fragment iteration, return false when iteration finished
    bool ContinueIterateGroupFragments();
    /// get fragment groups
    const Util::Dictionary<Util::String, Ptr<ShaderFragmentGroup>>& GetFragmentGroups() const;
    /// get active group fragment names (sorted alphabetically)
    const Util::Array<Util::String>& GetActiveGroupFragments() const;

private:
    /// activate the current iteration group fragment combination
    void ActivateCurrentGroupFragments();
    /// activate a group fragment
    void ActivateGroupFragment(const Util::String& fragmentGroup, const Util::String& groupFragment);
    /// deactivate a group fragment
    void DeactivateGroupFragment(const Util::String& fragmentGroup);

    Util::Dictionary<Util::String, Ptr<ShaderFragment>> fragments;
    Util::Dictionary<Util::String, Ptr<ShaderFragmentGroup>> fragmentGroups;
    IndexT innerIndex;
    IndexT outerIndex;
    SizeT numGroups;
    Util::FixedArray<IndexT> fragIndices;
    Util::Array<Util::String> activeGroupFragments;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Util::String>&
ShaderFragmentManager::GetActiveGroupFragments() const
{
    return this->activeGroupFragments;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragmentManager::HasFragment(const Util::String& name) const
{
    n_assert(name.IsValid());
    return this->fragments.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragmentManager::HasFragmentGroup(const Util::String& name) const
{
    n_assert(name.IsValid());
    return this->fragmentGroups.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ShaderFragment>&
ShaderFragmentManager::GetFragment(const Util::String& name) const
{
    return this->fragments[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, Ptr<ShaderFragmentGroup>>&
ShaderFragmentManager::GetFragmentGroups() const
{
    return this->fragmentGroups;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif