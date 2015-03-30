//------------------------------------------------------------------------------
//  mayamaterialgroupregistry.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayamaterialgroupregistry.h"
#include "mayashaders/mayashaderregistry.h"
#include "mayautil/mayaattrutil.h"
#include "mayautil/mayashaderutil.h"
#include "util/stringatom.h"

namespace Maya
{
__ImplementClass(Maya::MayaMaterialGroupRegistry, 'MGRG', Core::RefCounted);
__ImplementSingleton(Maya::MayaMaterialGroupRegistry);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaMaterialGroupRegistry::MayaMaterialGroupRegistry() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaMaterialGroupRegistry::~MayaMaterialGroupRegistry()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaMaterialGroupRegistry::Setup()
{
    n_assert(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaMaterialGroupRegistry::Discard()
{
    n_assert(this->isValid);
    this->materialGroups.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaMaterialGroupRegistry::Reset()
{
    n_assert(this->isValid);
    this->materialGroups.Clear();
}

//------------------------------------------------------------------------------
/**
    This method is called per exported shape node with an array of 
    Maya shaders assigned to the shape, and the face-shader-index array.
    The MayaMaterialGroupRegistry will unique-sort the shaders into
    a global material group array, and return a per-face array of
    material group indices.
*/
bool
MayaMaterialGroupRegistry::RegisterShapeShaders(const MObjectArray& shaderArray,
                                                const MIntArray& faceShaderIndices,
                                                MayaShaderFilter::Code shaderFilter,
                                                const MObject& hierarchyNode,
                                                MIntArray& outFaceMaterialGroupIndices)
{
    n_assert(this->isValid);

    // first check whether the shaderArray contains shaders which are not 
    // actually used by the shape, this may happen if a shader has been
    // assigned to the shape, but afterwards every face has been overridden
    // with a face-shader assignment
    // while we're at it, we will also filter out shaders which don't match the shader filter flag
    // the result is the validShaderMap array member
    this->BuildValidShaderMap(shaderArray, faceShaderIndices, shaderFilter);

    // now unique-append the face shaders into the material group array, and
    // build an face-shader index remap table
    IndexT shapeShaderIndex;
    SizeT numShapeShaders = shaderArray.length();
    MIntArray shaderToMaterialGroupIndexRemap(numShapeShaders, -1);
    for (shapeShaderIndex = 0; shapeShaderIndex < numShapeShaders; shapeShaderIndex++)
    {
        if (this->validShaderMap[shapeShaderIndex])
        {
            n_assert(!this->surfaceShaderArray[shapeShaderIndex].isNull());
            bool matGroupExists = false;
            const MObject& surfaceShader = this->surfaceShaderArray[shapeShaderIndex];

            // check if a material group already exists for this shader
            SizeT numMatGroups = this->materialGroups.Size();
            IndexT matGroupIndex;
            for (matGroupIndex = 0; matGroupIndex < numMatGroups; matGroupIndex++)
            {
                const MaterialGroup& matGroup = this->materialGroups[matGroupIndex];
                if ((matGroup.mayaShader == surfaceShader) &&
                    (hierarchyNode.isNull()) || (matGroup.hierarchyNode == hierarchyNode))
                {
                    // material group already exists
                    matGroupExists = true;
                    shaderToMaterialGroupIndexRemap[shapeShaderIndex] = matGroupIndex;
                }
            }
            if (!matGroupExists)
            {
                // need to setup a new material group
                MaterialGroup newMaterialGroup;
                newMaterialGroup.mayaShader = surfaceShader;
                newMaterialGroup.hierarchyNode = hierarchyNode;
                this->materialGroups.Append(newMaterialGroup);
                shaderToMaterialGroupIndexRemap[shapeShaderIndex] = this->materialGroups.Size() - 1;
            }
        }
    }

    // remap shader indices into material group indices
    SizeT numFaces = faceShaderIndices.length();
    outFaceMaterialGroupIndices.setLength(numFaces);
    IndexT faceIndex;
    for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        int faceShaderIndex = faceShaderIndices[faceIndex];
        if ((-1 != faceShaderIndex) && (this->validShaderMap[faceShaderIndex]))
        {
            // all ok, write material group index of current face
            outFaceMaterialGroupIndices[faceIndex] = shaderToMaterialGroupIndexRemap[faceShaderIndex];
        }
        else
        {
            // none, or invalid shader assigned to current face
            outFaceMaterialGroupIndices[faceIndex] = -1;
        }
    }

    // cleanup
    this->validShaderMap.Clear();
    this->surfaceShaderArray.Clear();

    return true;
}

//------------------------------------------------------------------------------
/**
    This builds the valid shader map array, contains true for each shader
    in shaderArray which is valid, additionally, the surfaceShaderArray
    will be initialized for each valid shader (points to the actual
    Maya shader node for each entry in the shaderArray, which contains
    shadingEngine objects).
*/
void
MayaMaterialGroupRegistry::BuildValidShaderMap(const MObjectArray& shaderArray, 
                                               const MIntArray& faceShaderIndices,
                                               MayaShaderFilter::Code shaderFilter)
{
    n_assert(this->isValid);

    // reset the validShaderMap array
    this->validShaderMap.SetSize(shaderArray.length());
    this->validShaderMap.Fill(false);
    this->surfaceShaderArray.SetSize(shaderArray.length());

    // iterate through faces and validate their assigned shaders
    IndexT faceIndex;
    SizeT numFaces = faceShaderIndices.length();
    for (faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        if (-1 != faceShaderIndices[faceIndex])
        {
            // validate the shader
            this->validShaderMap[faceShaderIndices[faceIndex]] = true;
        }
    }

    // now check whether the shader matches the mayaShaderFilter
    MayaShaderRegistry* shdRegistry = MayaShaderRegistry::Instance();
    IndexT shaderIndex;
    SizeT numShaders = shaderArray.length();
    for (shaderIndex = 0; shaderIndex < numShaders; shaderIndex++)
    {
        bool shaderValid = false;
        MObject mayaShaderObject = MayaShaderUtil::LookupMayaShaderObject(shaderArray[shaderIndex]);
        if (!mayaShaderObject.isNull())
        {
            if (MayaAttrUtil::HasAttr(mayaShaderObject, "rlNebulaShader"))
            {
                StringAtom shdName = MayaAttrUtil::GetString(mayaShaderObject, "rlNebulaShader");
                if (shdRegistry->HasShader(shdName))
                {
                    const MayaShader& shd = shdRegistry->GetShaderById(shdName);
                    if (shd.GetShaderFilter() == shaderFilter)
                    {
                        shaderValid = true;
                        this->surfaceShaderArray[shaderIndex] = mayaShaderObject;
                    } // else: shader filter flag doesn't match
                } // else: unknown shader name
            } // else: no Nebula shader assigned to Maya shader node
        }
        this->validShaderMap[shaderIndex] = shaderValid;
    }
}

} // namespace Maya
