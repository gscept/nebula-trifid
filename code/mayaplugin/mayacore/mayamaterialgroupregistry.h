#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaMaterialGroupRegistry
  
    The MayaMaterialGroupRegistry maintains the shader-polygon association
    in the scene. The registry is initialized by calling the
    RegisterMeshShaders() method once for every mesh shape in the 
    scene. This will convert the local face-shader indices into global
    material group indices which will be used to sort triangled by
    material id.
    
    (C) 2009 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include "mayashaders/mayashaderfilter.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaMaterialGroupRegistry : public Core::RefCounted
{
    __DeclareClass(MayaMaterialGroupRegistry);
    __DeclareSingleton(MayaMaterialGroupRegistry);
public:
    /// constructor
    MayaMaterialGroupRegistry();
    /// destructor
    virtual ~MayaMaterialGroupRegistry();

    /// setup the object
    void Setup();
    /// discard the object
    void Discard();
    /// return true if the object is valid
    bool IsValid() const;

    /// reset object for a new scene
    void Reset();
    /// register mesh shaders
    bool RegisterShapeShaders(const MObjectArray& shaderArray, const MIntArray& faceShaderIndices, MayaShaderFilter::Code shaderFilter, const MObject& hierarchyNode, MIntArray& ouFacetMaterialGroupIndices);

private:
    /// build the validShaderMap array
    void BuildValidShaderMap(const MObjectArray& shaderArray, const MIntArray& faceShaderIndices, MayaShaderFilter::Code shaderFilter);

    class MaterialGroup
    {
    public:
        MObject mayaShader;
        MObject hierarchyNode;      // optional!
    };
    Util::Array<MaterialGroup> materialGroups;
    Util::FixedArray<bool> validShaderMap;
    Util::FixedArray<MObject> surfaceShaderArray;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaMaterialGroupRegistry::IsValid() const
{
    return this->isValid;
}

} // namespace Maya