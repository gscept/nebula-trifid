//------------------------------------------------------------------------------
//  ceuiresourceprovider.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuiresourceprovider.h"
#include "io/ioserver.h"
#include "util/array.h"
#include "io/stream.h"
#include <map>

namespace CEUI
{

//------------------------------------------------------------------------------
/**
*/
CEUIResourceProvider::CEUIResourceProvider() 
{
}

//------------------------------------------------------------------------------
/**
*/
CEUIResourceProvider::~CEUIResourceProvider() 
{
}

//------------------------------------------------------------------------------
/**
*/
void 
CEUIResourceProvider::setResourceGroupDirectory(const CEGUI::String& resourceGroup, 
                                                const CEGUI::String& directory)
{
    if (directory.length() == 0)
        return;

#if defined(_WIN32) || defined(__WIN32__)
    // while we rarely use the unportable '\', the user may have
    const CEGUI::String separators("\\/");
#else
    const  CEGUI::String separators("/");
#endif

    if ( CEGUI::String::npos == separators.find(directory[directory.length() - 1]))
        d_resourceGroups[resourceGroup] = directory + '/';
    else
        d_resourceGroups[resourceGroup] = directory;
}

//------------------------------------------------------------------------------
/**
*/
const CEGUI::String& 
CEUIResourceProvider::getResourceGroupDirectory(const CEGUI::String& resourceGroup)
{
    return d_resourceGroups[resourceGroup];
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIResourceProvider::clearResourceGroupDirectory(const CEGUI::String& resourceGroup)
{
    ResourceGroupMap::iterator iter = d_resourceGroups.find(resourceGroup);

    if (iter != d_resourceGroups.end())
        d_resourceGroups.erase(iter);
}

//------------------------------------------------------------------------------
/**
*/
void 
CEUIResourceProvider::loadRawDataContainer(const CEGUI::String& filename, 
                                           CEGUI::RawDataContainer& output, 
                                           const CEGUI::String& resourceGroup) 
{
    Util::String finalFilename = this->getFinalFilename(filename, resourceGroup).c_str();
        
    if (!IO::IoServer::Instance()->FileExists(finalFilename))
    {
        n_error("CEGUI::Layout %s could not be found!", finalFilename);
    } 
    
    // Load file and copy data to output
    Util::String uri;
    Ptr<IO::Stream> rawStream = IO::IoServer::Instance()->CreateStream(IO::URI(finalFilename));
    n_assert(rawStream->Open());
    int size = rawStream->GetSize();
    //CEGUI::uint8* data = n_new_array(CEGUI::uint8, size);
    CEGUI::uint8* data = new CEGUI::uint8[size];
    rawStream->Read(data, size);
    rawStream->Close();
    output.setSize(size);
    output.setData(data);
}

//------------------------------------------------------------------------------
/**
*/
void 
CEUIResourceProvider::unloadRawDataContainer(CEGUI::RawDataContainer& data) 
{
    delete [] data.getDataPtr();
    data.setData(0);
    data.setSize(0);   
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::String 
CEUIResourceProvider::getFinalFilename(const CEGUI::String& filename, 
                                       const CEGUI::String& resourceGroup) const
{
    CEGUI::String final_filename;

    // look up resource group directory
    ResourceGroupMap::const_iterator iter =
        d_resourceGroups.find(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);

    // if there was an entry for this group, use it's directory as the
    // first part of the filename
    if (iter != d_resourceGroups.end())
        final_filename = (*iter).second;

    // append the filename part that we were passed
    final_filename += filename;

    // added nebula 3 stuff here
    return (IO::AssignRegistry::Instance()->ResolveAssignsInString(final_filename.c_str())).AsCharPtr();
}

//------------------------------------------------------------------------------
/**
*/
size_t
CEUIResourceProvider::getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec,
                                                const CEGUI::String& file_pattern,
                                                const CEGUI::String& resource_group)
{
    // look-up resource group name
    ResourceGroupMap::const_iterator iter =
        d_resourceGroups.find(resource_group.empty() ? d_defaultResourceGroup :
        resource_group);
    // get directory that's set for the resource group
    const CEGUI::String dir_name(iter != d_resourceGroups.end() ? (*iter).second : "./");

    size_t entries = 0;
    return entries;
}

} // namespace CEUI
