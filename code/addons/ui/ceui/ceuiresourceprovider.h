#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIResourceProvider

    Resource Provider wrapper for crazy eddies GUI system.

    (C) 2009 Radon Labs GmbH
*/
#include "cegui/include/CEGUIDefaultResourceProvider.h"
#include "sui/ceui/ceuirawdatacontainer.h"

namespace CEUI
{

class CEUIResourceProvider : public CEGUI::ResourceProvider
{
public:
    /// constructor
    CEUIResourceProvider();
    /// destructor
    virtual ~CEUIResourceProvider();
    ///
    virtual void setResourceGroupDirectory(const CEGUI::String& resourceGroup, 
                                           const CEGUI::String& directory);
    ///
    virtual const CEGUI::String& getResourceGroupDirectory(const CEGUI::String& resourceGroup);
    ///
    virtual void clearResourceGroupDirectory(const CEGUI::String& resourceGroup);
    ///
    virtual void loadRawDataContainer(const CEGUI::String& filename, 
                                      CEGUI::RawDataContainer& output, 
                                      const CEGUI::String& resourceGroup);
    ///
    virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);
    ///
    virtual size_t getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec,
                                             const CEGUI::String& file_pattern,
                                             const CEGUI::String& resource_group);

protected:
    //
    virtual CEGUI::String getFinalFilename(const CEGUI::String& filename, 
                                           const CEGUI::String& resourceGroup) const;

    typedef std::map<CEGUI::String, CEGUI::String, CEGUI::String::FastLessCompare> ResourceGroupMap;
    ResourceGroupMap    d_resourceGroups;
};

}

