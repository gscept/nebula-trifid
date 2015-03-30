#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceDictionary
    
    The resource dictionary singleton keeps information about file resources
    so that the actual resource doesn't have to be loaded first to
    check its attributes. Resource dictionaries are created by asset tools
    during asset export.

    The resource dictionary file format is as follows:

    Header:
    FourCC 'RDIC'       - 4 bytes magic number
    uint version        - 4 byte version
    uint numEntries     - 4 byte number of entries

    Version 0001 entry (every entry is 128 bytes):
        uint dataSize       - size of the data block of the resource
        uchar[124] name     - 120 bytes block with the resource id, MUST BE 0-TERMINATED!!

    @todo: ResourceDictionary should be the base to switch to numerical resource 
    id's later on. During asset export, an offline-dictionary is kept which
    associated actual filenames with numerical id's, and the asset export tools
    write those numerical ids in place where a string resource name would be
    used. For now it's just a lookup-table for the resource size, which
    is only really useful for console-platforms (since on those platforms
    the application has more control over the resource loading process
    as compared to PC-APIs).
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "resources/resourceid.h"
#include "system/byteorder.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceDictionary : public Core::RefCounted
{
    __DeclareClass(ResourceDictionary);
    __DeclareSingleton(ResourceDictionary);
public:
    /// a resource dictionary entry
    class Entry
    {
    public:
        /// constructor
        Entry() : size(0) { };
        /// set the resource data size
        void SetSize(SizeT s) { this->size = s; };
        /// get the resource data size
        SizeT GetSize() const { return this->size; };
    private:
        SizeT size;
    };

    /// constructor
    ResourceDictionary();
    /// destructor
    virtual ~ResourceDictionary();
    
    /// set the dictionary file URI
    void SetURI(const IO::URI& uri);
    /// get the dictionary file URI
    const IO::URI& GetURI() const;
    /// load resource dictionary from URI
    bool Load();
    /// unload the current resource dictionary
    void Unload();
    /// return true if currently loaded (dictionary contains entries)
    bool IsValid() const;
    
    /// test if a resource is in the dictionary
    bool HasEntry(const ResourceId& resId) const;
    /// get the resource dictionary entry of a resource
    const Entry& GetEntry(const ResourceId& resId) const;

private:
    static const SizeT MaxResIdLength = 124;
    IO::URI uri;    
    Util::Dictionary<ResourceId, Entry> dict;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceDictionary::IsValid() const
{
    return this->dict.Size() > 0;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceDictionary::SetURI(const IO::URI& u)
{
    n_assert(!this->IsValid());
    this->uri = u;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ResourceDictionary::GetURI() const
{
    return this->uri;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceDictionary::HasEntry(const ResourceId& resId) const
{
    return this->dict.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const ResourceDictionary::Entry&
ResourceDictionary::GetEntry(const ResourceId& resId) const
{
    return this->dict[resId];
}

} // namespace Resources
//------------------------------------------------------------------------------


