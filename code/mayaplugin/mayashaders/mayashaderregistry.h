#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShaderRegistry

    This loads the n3shaders.xml file which describes the N3 shaders 
    available in the plugin.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "mayashaders/mayashader.h"
#include <maya/MObject.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShaderRegistry : public Core::RefCounted
{
    __DeclareClass(MayaShaderRegistry);
    __DeclareSingleton(MayaShaderRegistry);
public:
    /// constructor
    MayaShaderRegistry();
    /// destructor
    virtual ~MayaShaderRegistry();

    /// setup the object (loads the n3shader.xml file)
    void Setup();
    /// discard the object
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// check if a shader exists by id
    bool HasShader(const Util::StringAtom& id) const;
    /// get number of shaders
    SizeT GetNumShaders() const;
    /// get shader object by index
    const MayaShader& GetShaderByIndex(IndexT i) const;
    /// get shader object by id
    const MayaShader& GetShaderById(const Util::StringAtom& id) const;
    /// get shader index by id, return InvalidIndex if not found
    IndexT GetShaderIndexById(const Util::StringAtom& id) const;

private:
    /// setup content from XML shader definition file
    void SetupFromXml(const Util::String& path);

    bool isValid;
    Util::Array<MayaShader> shaders;
    Util::Dictionary<Util::StringAtom, IndexT> shaderIndexMap;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShaderRegistry::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShaderRegistry::HasShader(const Util::StringAtom& id) const
{
    return this->shaderIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
MayaShaderRegistry::GetNumShaders() const
{
    return this->shaders.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const MayaShader& 
MayaShaderRegistry::GetShaderByIndex(IndexT i) const
{
    return this->shaders[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const MayaShader&
MayaShaderRegistry::GetShaderById(const Util::StringAtom& id) const
{
    return this->shaders[this->shaderIndexMap[id]];
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
MayaShaderRegistry::GetShaderIndexById(const Util::StringAtom& id) const
{
    if (this->shaderIndexMap.Contains(id))
    {
        return this->shaderIndexMap[id];
    }
    else
    {
        return InvalidIndex;
    }
}

} // namespace Maya
//------------------------------------------------------------------------------
