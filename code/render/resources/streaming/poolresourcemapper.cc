//------------------------------------------------------------------------------
//  poolresourcemapper.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "io/xmlreader.h"
#include "io/xmlwriter.h"
#include "loadingresource.h"
#include "coregraphics/pixelformat.h"
#include "poolresourcemapper.h"
#include "streamingresourcemapper.h"
#include "resourceinfo.h"
#include "resourcepool.h"
#include "resourcerequestinfo.h"
#include "resources/resourceloader.h"
#include "resources/managedresource.h"
#include "resources/resourceid.h"
#include "resourceslot.h"
#include "textureinfo.h"
#include "timing/timer.h"
#include "resources/resourcemanager.h"
#include "resourcescheduler.h"
#include "util/stringatom.h"

namespace Resources
{
    using namespace IO;
    using namespace Util;
    __ImplementClass(Resources::PoolResourceMapper, 'PRSM', Resources::StreamingResourceMapper);

//------------------------------------------------------------------------------
/**
*/
PoolResourceMapper::PoolResourceMapper(void)
{
}

//------------------------------------------------------------------------------
/**
*/
PoolResourceMapper::~PoolResourceMapper(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::SetDefaultPoolScheduler(const Ptr<PoolScheduler>& scheduler)
{
    this->defaultPoolScheduler = scheduler;
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::SetupPools(const Util::Dictionary<Util::StringAtom, PoolSetupInfo>& poolSetupData)
{
    n_assert(poolSetupData.Size() > 0);
    n_assert(0 != this->defaultLoaderClass);
    IndexT i;
    Ptr<ResourcePool> newPool;
    for (i = 0; i < poolSetupData.Size(); i++)
    {
        newPool = ResourcePool::Create();
        newPool->SetId(poolSetupData.KeyAtIndex(i));
        newPool->SetInfo(poolSetupData.ValueAtIndex(i).info);
        newPool->SetNumSlots(poolSetupData.ValueAtIndex(i).numSlots);
        newPool->SetLoader(this->defaultLoaderClass);
        this->pools.Append(newPool);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::OnAttachToResourceManager()
{
    this->resourceCreator = (ResourceCreator*)this->creatorType->Create();

    StreamingResourceMapper::OnAttachToResourceManager();

    // initialize pools
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        this->pools[i]->Initialize(this->resourceCreator, this->defaultPoolScheduler);
    }
}

//------------------------------------------------------------------------------
/**
    unloads all pools, placeholder-resource, ... (shutting down)
*/
void
PoolResourceMapper::OnRemoveFromResourceManager()
{
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        this->pools[i]->Unload();
    }
    this->defaultPoolScheduler = 0;

    StreamingResourceMapper::OnRemoveFromResourceManager();
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::OnPrepare(bool waiting)
{
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        this->pools[i]->SetFrameIndex(this->frameIdx);
#if !PUBLIC_BUILD
        this->pools[i]->ResetDebugStats();
#endif
    }
    StreamingResourceMapper::OnPrepare(waiting);
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::Reset()
{
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        this->pools[i]->Reset();
    }
    StreamingResourceMapper::Reset();
}

//------------------------------------------------------------------------------
/**
*/
uint
PoolResourceMapper::GetAllocatedMemory()
{
    IndexT i;
    uint result = 0;
    for (i = 0; i < this->pools.Size(); i++)
    {
        result += this->pools[i]->GetPoolSize();
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
uint
PoolResourceMapper::GetUsedMemory()
{
    uint result = 0;
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        result += this->pools[i]->GetUsedSize();
    }
    return result;
}

//------------------------------------------------------------------------------
/**
Writes infos of all pools to given XML-file.
*/
void
PoolResourceMapper::WritePoolsToXML(const URI& fileName)
{
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(fileName);
    n_assert(stream.isvalid());
    stream->SetAccessMode(Stream::WriteAccess);
    Ptr<XmlWriter> xmlWriter = XmlWriter::Create();
    xmlWriter->SetStream(stream);
    n_assert(stream->Open());
    n_assert(xmlWriter->Open());
    xmlWriter->BeginNode("TextureStreaming");
    xmlWriter->WriteComment("Test for NebulaT Texture-Contentstreaming. (C) 2010 Radon Labs GmbH");
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        const TextureInfo* info = (const TextureInfo*)(this->pools[i]->GetInfo());
        xmlWriter->BeginNode("Pool");
        xmlWriter->SetInt("Type", info->GetType());
        xmlWriter->SetInt("Width", info->GetWidth());
        xmlWriter->SetInt("Height", info->GetHeight());
        xmlWriter->SetInt("Depth", info->GetDepth());
        xmlWriter->SetInt("MipLevels", info->GetMipLevels());
        xmlWriter->SetString("PixelFormat", CoreGraphics::PixelFormat::ToString(info->GetPixelFormat()));
        xmlWriter->SetInt("Size", int(info->GetSize()));
        xmlWriter->SetInt("Slots", this->pools[i]->GetNumSlots());
        xmlWriter->SetInt("SlotsUsed", this->pools[i]->GetSlotsUsed());
        xmlWriter->EndNode();
    }
    xmlWriter->EndNode();
    xmlWriter->Close();
    stream->Close();
}

//------------------------------------------------------------------------------
/**
*/
IndexT
PoolResourceMapper::GetFittingPoolIndex(const ResourceInfo* resInfo) const
{
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        if (resInfo->IsEqual(this->pools[i]->GetInfo()))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    If no free slot is found we may want to do something like trying to find next lower or
    higher level of detail for the requested resource.
    Overload this method in subclasses to do things like this.
*/
void
PoolResourceMapper::NoSlotFound(const Ptr<ManagedResource>& resource, IndexT frameIdx)
{
    n_printf("no free slot found for resource '%s'\n", resource->GetResourceId().AsString().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourcePool>&
PoolResourceMapper::GetPoolForDebug(const Util::StringAtom& poolId) const
{
    IndexT i;
    for (i = 0; i < this->pools.Size(); i++)
    {
        if (this->pools[i]->GetId() == poolId)
        {
            return this->pools[i];
        }
    }
    n_error("Error: pool %s does not exist", poolId.Value());
    return this->pools[0];
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::ReadTexturePoolFromXML(const IO::URI& fileName, Util::Dictionary<Util::StringAtom, PoolSetupInfo>& setupData)
{
    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(fileName);
    n_assert(stream.isvalid());
    stream->SetAccessMode(IO::Stream::ReadAccess);
    Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
    xmlReader->SetStream(stream);
    n_assert(stream->Open());
    n_assert(xmlReader->Open());
    if (xmlReader->SetToFirstChild("Pool"))
    {
        do
        {
            TextureInfo* texInfo = n_new(TextureInfo);
            PoolSetupInfo setupInfo;
            StringAtom poolId;

            texInfo->SetType(CoreGraphics::Texture::Type(xmlReader->GetInt("Type")));
            texInfo->SetWidth(xmlReader->GetInt("Width"));
            texInfo->SetHeight(xmlReader->GetInt("Height"));
            texInfo->SetDepth(xmlReader->GetInt("Depth"));
            texInfo->SetMipLevels(xmlReader->GetInt("MipLevels"));
            texInfo->SetPixelFormat(CoreGraphics::PixelFormat::FromString(xmlReader->GetString("PixelFormat")));
            texInfo->SetSize(xmlReader->GetInt("Size"));
            setupInfo.numSlots = xmlReader->GetInt("Slots");
            poolId = xmlReader->GetString("PoolID");
            setupInfo.info = texInfo;
            setupData.Add(poolId, setupInfo);
        }
        while (xmlReader->SetToNextChild("Pool"));
    }
    xmlReader->Close();
    stream->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
PoolResourceMapper::WriteTexturePoolToXML(const URI& fileName, const Dictionary<StringAtom, PoolSetupInfo>& setupData)
{
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(fileName);
    n_assert(stream.isvalid());
    stream->SetAccessMode(Stream::WriteAccess);
    Ptr<XmlWriter> xmlWriter = XmlWriter::Create();
    xmlWriter->SetStream(stream);
    n_assert(stream->Open());
    n_assert(xmlWriter->Open());
    xmlWriter->BeginNode("TextureStreaming");
    xmlWriter->WriteComment("NebulaT Texture-Contentstreaming -- (C) 2010 Radon Labs GmbH");
    IndexT poolIdx;
    for (poolIdx = 0; poolIdx < setupData.Size(); poolIdx++)
    {
        const TextureInfo* info = (TextureInfo*)setupData.ValueAtIndex(poolIdx).info;
        xmlWriter->BeginNode("Pool");
        xmlWriter->SetInt("Type", info->GetType());
        xmlWriter->SetInt("Width", info->GetWidth());
        xmlWriter->SetInt("Height", info->GetHeight());
        xmlWriter->SetInt("Depth", info->GetDepth());
        xmlWriter->SetInt("MipLevels", info->GetMipLevels());
        xmlWriter->SetString("PixelFormat", CoreGraphics::PixelFormat::ToString(info->GetPixelFormat()));
        xmlWriter->SetInt("Size", int(info->GetSize()));
        xmlWriter->SetInt("Slots", setupData.ValueAtIndex(poolIdx).numSlots);
        // do ONLY change order of poolID elements if you want to display another order of pools in web browser as this is used for easy sorting of pools

        xmlWriter->SetString("PoolID", setupData.KeyAtIndex(poolIdx).AsString());
        xmlWriter->EndNode();
    }
    xmlWriter->EndNode();
    xmlWriter->Close();
    stream->Close();
}
} // namespace Resources
//------------------------------------------------------------------------------