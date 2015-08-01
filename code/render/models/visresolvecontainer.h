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
#include "materials/surfacename.h"

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
    void SetResolved(const Materials::SurfaceName::Code& code, bool b);
	/// return true if the resolved flag has been set for ModelNodeMaterial
    bool IsResolved(const Materials::SurfaceName::Code& code) const;
    /// add a visible element by ModelNodeType
    void Add(IndexT frameIndex, const Materials::SurfaceName::Code& code, const Ptr<TYPE>& e);
    /// get all visible elements of given ModelNodeType
    const Util::Array<Ptr<TYPE> >& Get(const Materials::SurfaceName::Code& code) const;

    static const IndexT numEntries = Materials::SurfaceName::MaxNumSurfaceNames;

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
template<class TYPE> 
inline void
VisResolveContainer<TYPE>::SetResolved(const Materials::SurfaceName::Code& code, bool b)
{
    this->entries[code].resolved = b;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> 
inline bool
VisResolveContainer<TYPE>::IsResolved(const Materials::SurfaceName::Code& code) const
{
    return this->entries[code].resolved;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> 
inline void
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
template<class TYPE> 
inline void
VisResolveContainer<TYPE>::Add(IndexT curFrameIndex, const Materials::SurfaceName::Code& code, const Ptr<TYPE>& e)
{
    if (curFrameIndex != this->frameIndex)
    {
        this->Reset();
        this->frameIndex = curFrameIndex;
    }
    this->entries[code].nodes.Append(e);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE> 
inline const Util::Array<Ptr<TYPE> >&
VisResolveContainer<TYPE>::Get(const Materials::SurfaceName::Code& code) const
{
    return this->entries[code].nodes;
}

} // namespace Models
//------------------------------------------------------------------------------
    