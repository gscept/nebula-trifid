#pragma once
#include "resources\resourceloader.h"
//------------------------------------------------------------------------------
/**
@class Vegetation::EmptyResourceLoader


(C) 2008 Radon Labs GmbH
*/
namespace Vegetation 
{
    
class EmptyResourceLoader : public Resources::ResourceLoader
{
    __DeclareClass(EmptyResourceLoader)
public:
    /// set resource state
    void SetState(Resources::Resource::State state);
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
};
//------------------------------------------------------------------------------
/**
*/
inline void 
EmptyResourceLoader::SetState(Resources::Resource::State state)
{
    this->state  = state; 
}

}
