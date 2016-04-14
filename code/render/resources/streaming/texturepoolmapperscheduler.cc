//------------------------------------------------------------------------------
//  texturepoolmapperscheduler.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "texturepoolmapperscheduler.h"
#include "texturerequestinfo.h"
#include "resources/managedtexture.h"
#include "poolresourcemapper.h"
#include "texturestreamer.h"
#include "resourcepool.h"
#include "textureinfo.h"
#include "coregraphics/texture.h"
#include "loadingresource.h"
#include "math/scalar.h"
#include "textureinfo.h"
#include "coregraphics/texture.h"
#include "poolloadingresource.h"

using namespace CoreGraphics;

namespace Resources
{
    __ImplementClass(Resources::TexturePoolMapperScheduler, 'STPS', Resources::ResourceScheduler);

//------------------------------------------------------------------------------
/**
*/
TexturePoolMapperScheduler::TexturePoolMapperScheduler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TexturePoolMapperScheduler::~TexturePoolMapperScheduler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This stores a downcasted copy of the Ptr as a separate PoolResourceMapper
    so we don't need to cast it frequently OnFrame.
*/
void
TexturePoolMapperScheduler::SetMapper(const Ptr<StreamingResourceMapper>& mapper)
{
    ResourceScheduler::SetMapper(mapper);
    this->poolMapper = mapper.downcast<PoolResourceMapper>();
}

//------------------------------------------------------------------------------
/**
*/
void
TexturePoolMapperScheduler::DoResourceLOD(const Ptr<ManagedResource>& managedResource)
{
    // discarded resources should not be in list of active resources and therefore not being visited here
    if (!managedResource->IsAutoManaged() || managedResource->GetResourceStreamingLevelOfDetail() < 0.0f ||
        managedResource->GetResource()->IsLocked())
    {
        // skip pinned, invisible and temporary locked resources
        return;
    }

    // determine required mip level
    const Ptr<ManagedTexture> managedTex = managedResource.downcast<ManagedTexture>();
    if (Texture::Texture2D != managedTex->GetTexture()->GetType())
    {
        // do not auto manage mips of cube or volume textures currently
        // as this is currently not supported by the loader
        return;
    }
    // required mips = total mips - factor
    IndexT requiredMipLevel = managedTex->GetTextureUnloaded()->GetNumMipLevels() + managedTex->GetTextureUnloaded()->GetSkippedMips() - (IndexT)managedResource->GetResourceStreamingLevelOfDetail();
    if (requiredMipLevel < 0)
    {
        requiredMipLevel = 0;
    }

    // --- change following part for different mip mapping tactics which decide when to load another mip level ---
    // check if we do need to change something (only react on a diff of 2 mip levels)
    int mipDiff = managedTex->GetTextureUnloaded()->GetNumMipLevels() - requiredMipLevel;
    if (1 >= abs(mipDiff))
    {
        // needed mip level or next required mip level is loaded - all fine
        return;
    }
    else if (mipDiff < -1 || 0 == requiredMipLevel)
    {
        // we need to load a higher (better!) mip map
        TextureRequestInfo newRequest;
        newRequest.SetMipLevel(requiredMipLevel);
        newRequest.SetResourceId(managedResource->GetResourceId());
        this->OnRequestOtherMipMap(managedTex, &newRequest);
    }
    else if (mipDiff > 1)
    {
        // we may release some mip levels
        TextureRequestInfo newRequest;
        newRequest.SetMipLevel(requiredMipLevel);
        newRequest.SetResourceId(managedResource->GetResourceId());
        this->OnRequestOtherMipMap(managedTex, &newRequest);
    }
}

//------------------------------------------------------------------------------
/**
    Looks up a fitting pool and a free slot in this pool is looked up.
    If there is a free slot available the slot's Resource is set to requested Resource
    and a new LoadingResource-entry is sorted into the loadingQueue.
    If NO free slot is currently available false is returned.
    If NO free pool is found an error is thrown as this must not occur as each requested
    resource should have a valid entry in the resource dictionary.
*/
bool
TexturePoolMapperScheduler::OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo)
{
    n_assert(managedResource.isvalid());
    // look up resource in active resources if it's already loaded
    IndexT activeResourceIdx = this->poolMapper->activeResources.FindIndex(managedResource->GetResourceId());
    if (activeResourceIdx != InvalidIndex)
    {
        n_error("Resource already loaded but ResourceManager didn't recognize...");
        // resource is registered in a pool
        // check if it's in loading queue and eventually increase loading-priority
        this->poolMapper->IncreaseLoadingPriority(managedResource->GetResourceId());

        //managedResource = this->poolMapper->activeResources.ValueAtIndex(activeResourceIdx);
        n_assert(!managedResource->IsPlaceholder());

        if (managedResource->IsAutoManaged())
        {
            // set to NOT auto-managed if demoInfo says so
            managedResource->SetAutoManaged(requestInfo->IsAutoManaged());
        }
    }
    else
    {
        managedResource->SetResourceType(this->mapper->resType);
        // resource not loaded nor loading
        // first look up resInfo in resInfo-table
        const ResourceInfo* resInfo = this->poolMapper->GetResourceInfo(managedResource->GetResourceId());

        // now find an appropriate pool
        IndexT fittingPoolIdx = this->poolMapper->GetFittingPoolIndex(resInfo);
        if (InvalidIndex == fittingPoolIdx)
        {
            n_error("Error: no fitting pool found for '%s'", requestInfo->GetResourceId().AsString().AsCharPtr());
        }
        else
        {
            // try to get a free slot from appropriate pool
            Ptr<ResourceSlot> freeSlot = this->poolMapper->pools[fittingPoolIdx]->RequestSlot(requestInfo);
            if (freeSlot.isvalid())
            {
                // free slot found
                // remove the entry from active resources
                if (freeSlot->GetCurrentManagedResource().isvalid())
                {
                    // erase resource managed so far
                    this->poolMapper->activeResources.Erase(freeSlot->GetCurrentManagedResource()->GetResourceId());
                    IndexT i;
                    // ensure outkicked resource is not loading
                    for (i = 0; i < this->poolMapper->loadingQueue.Size(); i++)
                    {
                        if (this->poolMapper->loadingQueue[i]->GetManagedResource()->GetResourceId() == freeSlot->GetCurrentManagedResource()->GetResourceId())
                        {
                            this->poolMapper->loadingQueue[i]->OnCancelRequest();
                            this->poolMapper->loadingQueue.EraseIndex(i);
                            break;
                        }
                    }
                    freeSlot->Reset();
                }
                // set slot's new ManagedResource
                freeSlot->SetupFromManagedResource(managedResource);
                managedResource->SetResource(freeSlot->GetResource());
                this->poolMapper->AppendLoadingResource(LoadingResource::RTTI, managedResource, freeSlot->GetResource());
            }
            else
            {
                // no free slot found
                this->poolMapper->NoSlotFound(managedResource, this->poolMapper->frameIdx);

#if !PUBLIC_BUILD
                this->poolMapper->pools[fittingPoolIdx]->IncreaseRejectedRequests(managedResource->GetResourceId());
#endif
            }
        }
    }
    managedResource->SetFrameId(this->poolMapper->frameIdx);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
TexturePoolMapperScheduler::OnRemoveFromMapper()
{
    this->poolMapper = 0;
    ResourceScheduler::OnRemoveFromMapper();
}

//------------------------------------------------------------------------------
/**
*/
bool
TexturePoolMapperScheduler::OnRequestOtherMipMap(const Ptr<ManagedTexture>& managedTexture, const TextureRequestInfo* requestInfo)
{
    // copy requested mip level once so we don't copy on each access at using TextureRequestInfo::GetMipLevel()
    uint requestedMipLevel = requestInfo->GetMipLevel();
    n_assert(managedTexture.isvalid());
    n_assert(!managedTexture->IsPlaceholder());    

    // copy texInfo as we need to change mip level from default
    TextureInfo texInfo = *(TextureInfo*)this->poolMapper->resourceDictionary[managedTexture->GetResourceId()];
    uint maxMips = texInfo.GetMipLevels();
    uint maxHeight = texInfo.GetHeight();
    uint maxWidth = texInfo.GetWidth();
    uint mipsToSkip = maxMips - requestedMipLevel;

    // first find the currently used slot

    // FIXME: this is quite slow as we need to check each slot
    // if its MR matches the requested MR - seems a bit hacky anyway
    const Ptr<CoreGraphics::Texture> tex = managedTexture->GetTextureUnloaded();
    texInfo.SetMipLevels(tex->GetNumMipLevels());
    texInfo.SetWidth(tex->GetWidth());
    texInfo.SetHeight(tex->GetHeight());
    IndexT currentPoolIdx = this->poolMapper->GetFittingPoolIndex(&texInfo);
    n_assert(InvalidIndex != currentPoolIdx);
    Ptr<ResourceSlot> oldSlot = this->poolMapper->pools[currentPoolIdx]->GetSlot(managedTexture.upcast<ManagedResource>());
    n_assert(oldSlot.isvalid());

    // now try to find a new slot
    texInfo.SetMipLevels(requestedMipLevel);
    texInfo.SetHeight(maxHeight);
    texInfo.SetWidth(maxWidth);
    uint i;
    for (i = 0; i < mipsToSkip; i++)
    {
        texInfo.SetWidth(texInfo.GetWidth() >> 1);
        texInfo.SetHeight(texInfo.GetHeight() >> 1);
    }
    IndexT fittingPoolIdx = this->poolMapper->GetFittingPoolIndex(&texInfo);
    if (InvalidIndex == fittingPoolIdx)
    {
        // @todo: may try finding next lower/higher pool here
        // but for the beginning we abort here

#if !PUBLIC_BUILD
        n_printf("couldn't find fitting pool for '%s' with %i mip levels\n", managedTexture->GetResourceId().AsString().AsCharPtr(), requestedMipLevel);
        this->poolMapper->pools[currentPoolIdx]->IncreaseAutoMipPoolsNotFound();
#endif
        return false;
    }
    
    // we've got a pool...
    Ptr<ResourceSlot> newSlot = this->poolMapper->pools[fittingPoolIdx]->RequestSlot((const ResourceRequestInfo*)requestInfo, requestInfo->IsAutoManaged() || managedTexture->IsAutoManaged());
    if (!newSlot.isvalid())
    {
        // may try to find next lower/higher pool here
        // but for the beginning we abort here

#if !PUBLIC_BUILD
        n_printf("couldn't find free slot for '%s' with %i mip levels\n", managedTexture->GetResourceId().AsString().AsCharPtr(), requestedMipLevel);
        this->poolMapper->pools[fittingPoolIdx]->IncreaseAutoMipSlotsNotFound();
#endif
        return false;
    }
    if (newSlot->GetResource()->GetResourceId() == requestInfo->GetResourceId() && Resource::Loaded == newSlot->GetResource()->GetState())
    {
        // we've found texture in pool from previous usage!
        n_assert(!newSlot->GetCurrentManagedResource().isvalid());
        n_printf("found '%s' still loaded in pool with %i mips. no load required!\n", requestInfo->GetResourceId().Value(), requestedMipLevel);
        oldSlot->SetCurrentManagedResource(0);
        newSlot->SetCurrentManagedResource(managedTexture.upcast<ManagedResource>());
        managedTexture->SetResource(newSlot->GetResource());
        return true;
    }

    // we've got a new slot
    newSlot->SetupFromManagedResource(0);
    newSlot->GetResource()->SetResourceId(managedTexture->GetResourceId());
    newSlot->GetResource().downcast<Texture>()->SetSkippedMips(mipsToSkip);

    // at this point we've got:
    // oldSlot          - ManagedResource = managedResource
    //                  - Resource = resource we want to reuse partially/totally, with old mip level
    // newSlot          - ManagedResource = 0 (previously ManagedResource of this slot is kicked out and has no reference to slot' Resource any longer
    //                  - Resource = resource we want to overwrite, with new mip level (already knowing new resourceId)
    // managedResource  - pointing towards oldSlot' Resource

    // tell loader to reuse content of old slot - therefore lock old and new slot' resource
    // so they won't be used for any writing process
    oldSlot->GetResource()->Lock();
    newSlot->GetResource()->Lock();
    Ptr<TextureStreamer> texLoader = newSlot->GetResource()->GetLoader().downcast<TextureStreamer>();
    texLoader->SetReuseTexture(managedTexture->GetResource().downcast<Texture>());
    // call mapper for appending as we don't know if this scheduler or another one performs appending
    // (actually it IS the same scheduler but this will may be changed by a programmer in the future)
    const Ptr<PoolLoadingResource>& loadingResource = this->poolMapper->AppendLoadingResource(PoolLoadingResource::RTTI, managedTexture.upcast<ManagedResource>(), newSlot->GetResource()).downcast<PoolLoadingResource>();
    loadingResource->SetSourceSlot(oldSlot);
    loadingResource->SetTargetSlot(newSlot);
    return true;
}
} // namespace Resources
//------------------------------------------------------------------------------
