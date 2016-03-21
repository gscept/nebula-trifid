#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::ResourceDictionaryReader

    ResourceDictionaryReader reads a certain .dic-file and fills a
    Dictionary<ResourceId, Ptr<ResourceInfo>>.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "io/stream.h"
#include "textureinfo.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceDictionaryReader : public Core::RefCounted
{
__DeclareClass(ResourceDictionaryReader);
public:
    /// constructor
    ResourceDictionaryReader(void);
    /// destructor
    ~ResourceDictionaryReader(void);

    /// reads given file in dst: and fills given dictionary with resource-informations
    virtual void CreateResourceDictionary(const IO::URI& dstPath, Util::Dictionary<ResourceId, ResourceInfo*>& dict);

protected:
    static const int MaxResIdSize = 124;
};
} // namespace Resources
//------------------------------------------------------------------------------