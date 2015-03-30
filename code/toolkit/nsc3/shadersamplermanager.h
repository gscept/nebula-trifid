#pragma once
#ifndef TOOLS_SHADERSAMPLERMANAGER_H
#define TOOLS_SHADERSAMPLERMANAGER_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderSamplerManager
    
    Parse and offer shader texture sampler definitions.
    
    (C) 2007 Radon Labs GmbH  
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "shadersampler.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderSamplerManager : public Core::RefCounted
{
    __DeclareClass(ShaderSamplerManager);
    __DeclareSingleton(ShaderSamplerManager);
public:
    /// constructor
    ShaderSamplerManager();
    /// destructor
    virtual ~ShaderSamplerManager();
    /// load samplers from the provided path
    bool LoadSamplers(const IO::URI& path);
    /// return true if a sampler exists
    bool HasSampler(const Util::String& name) const;
    /// get access to a sampler
    const Ptr<ShaderSampler>& GetSampler(const Util::String& name) const;

private:
    Util::Dictionary<Util::String, Ptr<ShaderSampler>> samplers;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderSamplerManager::HasSampler(const Util::String& name) const
{
    return this->samplers.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ShaderSampler>&
ShaderSamplerManager::GetSampler(const Util::String& name) const
{
    return this->samplers[name];
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif


