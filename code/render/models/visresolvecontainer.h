#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::VisResolveContainer
    
    Helper class which keeps an array of visible nodes by material type.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/ptr.h"
#include "frame/batchgroup.h"
#include "materials/materialtype.h"

//------------------------------------------------------------------------------
namespace Models
{
template<class TYPE>
class VisResolveContainer
{
public:
    /// constructor
    VisResolveContainer();
    /// reset content
    void Reset();
	/// set the resolved flag for a given ModelNodeMaterial
    void SetResolved(const Materials::MaterialType::Code& t, bool b);
	/// return true if the resolved flag has been set for ModelNodeMaterial
    bool IsResolved(const Materials::MaterialType::Code& t) const;
    /// add a visible element by ModelNodeType
    void Add(IndexT frameIndex, const Materials::MaterialType::Code& t, const Ptr<TYPE>& e);
    /// get all visible elements of given ModelNodeType
    const Util::Array<Ptr<TYPE> >& Get(const Materials::MaterialType::Code& t) const;

    static const IndexT numEntries = Materials::MaterialType::MaxNumMaterialTypes;

private:
    struct Entry
    {
        /// constructor
        Entry() : resolved(false) {};

        Util::Array<Ptr<TYPE>> nodes;
        bool resolved;
    };
    Util::FixedArray<Entry> entries;
    IndexT frameIndex;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
VisResolveContainer<TYPE>::VisResolveContainer() :
	entries(numEntries),
    frameIndex(InvalidIndex)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> void
VisResolveContainer<TYPE>::SetResolved(const Materials::MaterialType::Code& t, bool b)
{
    this->entries[t].resolved = b;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> bool
VisResolveContainer<TYPE>::IsResolved(const Materials::MaterialType::Code& t) const
{
    return this->entries[t].resolved;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> void
VisResolveContainer<TYPE>::Reset()
{
    IndexT i;
    for (i = 0; i < numEntries; i++)
    {
        this->entries[i].resolved = false;
        this->entries[i].nodes.Clear();
    }
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> void
VisResolveContainer<TYPE>::Add(IndexT curFrameIndex, const Materials::MaterialType::Code& t, const Ptr<TYPE>& e)
{
    if (curFrameIndex != this->frameIndex)
    {
        this->Reset();
        this->frameIndex = curFrameIndex;
    }
    this->entries[t].nodes.Append(e);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> const Util::Array<Ptr<TYPE> >&
VisResolveContainer<TYPE>::Get(const Materials::MaterialType::Code& t) const
{
    return this->entries[t].nodes;
}

} // namespace Models
//------------------------------------------------------------------------------
    