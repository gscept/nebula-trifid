//------------------------------------------------------------------------------
//  grasspatch.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grasspatch.h"
#include "vegetation/grasslibrary.h"
#include "vegetation/grassplantmesh.h"
#include "vegetation/grassrenderer.h"
#include "vegetation/grassheightmap.h"
#include "vegetation/grassgroundmap.h"
#include "vegetation/grasspatcharray.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/memorymeshloader.h"
#include "resources/sharedresourceserver.h"
#include "util/randomnumbertable.h"
#include "models/model.h"
#include "models/modelnode.h"
#include "models/managedmodel.h"
#include "models/nodes/shapenodeinstance.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "vegetation/emptyresourceloader.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassPatch, 'VGPA', Core::RefCounted);

using namespace Util;
using namespace CoreGraphics;
using namespace InternalGraphics;
using namespace Resources;
using namespace Math;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
GrassPatch::GrassPatch() :    
    isActive(false),
    origMinY(0.0f),
    origMaxY(0.0f),
    mapLookupArray(0),
    weightMapStartIndex(0),
    intRGBAMask(0),
    grassLibraryBackPointer(0),
    grassLibraryPatchIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GrassPatch::~GrassPatch()
{
    this->Cleanup();
}

//------------------------------------------------------------------------------
/**
    Cleanup any resources used by the patch.
*/
void
GrassPatch::Cleanup()
{
    if (this->IsActive())
    {
        this->Deactivate();
    }
    if (this->managedModel.isvalid())
    {
        const Ptr<Model>& model = this->managedModel->GetModel();        
        model->RemoveNode(this->refShapeNode.cast<ModelNode>());
        model->Unload();
        this->managedModel->Clear();
        this->managedModel = 0;
    }
    if (this->refShapeNode.isvalid())
    {   
        this->refShapeNode = 0;
    }
    if (this->refMesh.isvalid())
    {   
        this->refMesh->Unload();
        this->refMesh = 0;
    }
    if (this->graphicsEntity.isvalid())
    {        
        this->graphicsEntity = 0;
    }    
    if (0 != this->mapLookupArray)
    {
        Memory::Free(Memory::ResourceHeap, this->mapLookupArray);
        this->mapLookupArray = 0;
    }
    this->grassLibraryBackPointer = 0;
    this->grassLibraryPatchIndex = 0;      
}

//------------------------------------------------------------------------------
/**
    This will fill the static grass patch mesh with random plant geometry 
    from the grass library.

    NOTE: this method will usually only be called once at level start,
    so it is not super-optimized.
*/
void
GrassPatch::Setup(GrassLibrary* lib, int patchIndex)
{
    n_assert(0 != lib);

    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    const int numPlantsPerDim = grassRenderer->GetNumPlantsPerPatchDimension();
    const float maxSpreadRel = grassRenderer->GetMaxRandomPlantSpread();

    // cleanup previous stuff
    this->Cleanup();

    // store setup info (needed in Validate())
    this->grassLibraryBackPointer = lib;
    this->grassLibraryPatchIndex = patchIndex;

    /* since we need to know the size of the vertex and index buffers
     beforehand we create an array of pseudo-random plant indices*/
    Array<Array<int>> plantIndices;        
    int numVertices = 0;
    int numIndices = 0;
    int randKey = 0;
    int x, y;
    for (x = 0; x < numPlantsPerDim; x++)
    {
        Array<int> yArray;
        for (y = 0; y < numPlantsPerDim; y++)
        {
            int plantIndex = int(Math::n_rand() * (lib->GetNumPlantMeshes()-1));
            yArray.Append(plantIndex);
            numVertices += lib->GetPlantMeshAtIndex(plantIndex)->GetNumVertices();
            numIndices += lib->GetPlantMeshAtIndex(plantIndex)->GetNumIndices();
        }
        plantIndices.Append(yArray);
    }

    // generate a resource name for the patch
    Util::String patchRsrcName = "grasspatches/";
    Util::String libRsrcName = lib->GetResourceId().Value();
    libRsrcName.SubstituteChar('/', '_');
    Util::String shapeNodeName = libRsrcName;
    shapeNodeName.AppendInt(patchIndex);
    patchRsrcName.Append(shapeNodeName);

    // create a static mesh object
    // create vertex buffer
    Ptr<MemoryVertexBufferLoader> vertexBufferLoader = MemoryVertexBufferLoader::Create();
    Ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create();
    Array<CoreGraphics::VertexComponent> components;
    components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
    SizeT bytes = (components.End() - 1)->GetByteSize();
    components.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Short2));
    bytes += (components.End() - 1)->GetByteSize();
    components.Append(VertexComponent(VertexComponent::TexCoord, 1, VertexComponent::Short2));
    bytes += (components.End() - 1)->GetByteSize();
    SizeT vertexBufferSize = numVertices * bytes;
    vertexBufferLoader->Setup(components, numVertices, 0, vertexBufferSize, VertexBuffer::UsageDynamic, VertexBuffer::AccessWrite);
    vertexBuffer->SetLoader(vertexBufferLoader.cast<ResourceLoader>());
    vertexBuffer->Load();
    vertexBuffer->SetLoader(0);

    // create index buffer
    Ptr<MemoryIndexBufferLoader> indexBufferLoader = MemoryIndexBufferLoader::Create();
    Ptr<IndexBuffer> indexBuffer = IndexBuffer::Create();
    SizeT indexBufferSize = numIndices * IndexType::SizeOf(IndexType::Index16);
    indexBufferLoader->Setup(IndexType::Index16, numIndices, 0, indexBufferSize, IndexBuffer::UsageDynamic, IndexBuffer::AccessWrite);
    indexBuffer->SetLoader(indexBufferLoader.cast<ResourceLoader>());
    indexBuffer->Load();
    indexBuffer->SetLoader(0);

    // allocate the height map lookup info array
    this->AllocHeightLookupArray(numVertices);

    // fill the mesh with geometry, to get a somewhat even spacing
    // we place the plants along a grid and displace randomly a bit
    // we also rotate the plants by some random angle
    ushort baseVertexIndex = 0;
    float* vertexBufferPtr = (float*)vertexBuffer->Map(VertexBuffer::MapWrite);
    ushort* indexBufferPtr = (ushort*)indexBuffer->Map(VertexBuffer::MapWrite);
    float* dstVertexPtr = vertexBufferPtr;
    ushort* dstIndexPtr = indexBufferPtr;

    Math::matrix44 plantTransform;
    Math::vector plantPos;
    float patchSize = GrassRenderer::Instance()->GetPatchSize();
    float avgPlantDist = patchSize / numPlantsPerDim;
    float maxSpreadAbs = avgPlantDist * maxSpreadRel;
    int curVertexIndex = 0;
    randKey = 0;
    SizeT incomningVertexBufferSize = 0;
    SizeT incomningIndexBufferSize = 0;
    for (x = 0; x < numPlantsPerDim; x++)
    {
        float midPointX = (x * avgPlantDist) - (patchSize * 0.5f);
        for (y = 0; y < numPlantsPerDim; y++)
        {
            //setup a transform matrix for the current plant
            float midPointY = (y * avgPlantDist) - (patchSize * 0.5f);
            plantPos.x() = midPointX + RandomNumberTable::Rand(randKey++, -maxSpreadAbs, +maxSpreadAbs);
            plantPos.z() = midPointY + RandomNumberTable::Rand(randKey++, -maxSpreadAbs, +maxSpreadAbs);
            float rot = RandomNumberTable::Rand(randKey++, 0.0f, n_deg2rad(360.0f));            
            plantTransform = matrix44::rotationy(rot);
            plantTransform.set_position(plantPos);

            //copy vertices and indices
            Ptr<GrassPlantMesh> srcPlantMesh = lib->GetPlantMeshAtIndex(plantIndices[x][y]);
            incomningVertexBufferSize += srcPlantMesh->GetNumVertices() * bytes;
            float* nextDstVertexPtr = srcPlantMesh->CopyVertices(dstVertexPtr, plantTransform);
            incomningIndexBufferSize += srcPlantMesh->GetNumIndices() * IndexType::SizeOf(IndexType::Index16);
            ushort* nextDstIndexPtr = srcPlantMesh->CopyIndices(dstIndexPtr, baseVertexIndex);
            baseVertexIndex += (ushort) srcPlantMesh->GetNumVertices();
            
            //setup pre-computed lookat-info for this plant
            int plantNumVertices = srcPlantMesh->GetNumVertices();
            int plantVertexWidth = srcPlantMesh->GetVertexWidth();
            this->SetupMapLookupInfos(plantPos, dstVertexPtr, curVertexIndex, plantNumVertices, plantVertexWidth);

            dstVertexPtr = nextDstVertexPtr;
            dstIndexPtr = nextDstIndexPtr;
            curVertexIndex += plantNumVertices;
        }
    }
    n_assert(incomningVertexBufferSize == vertexBufferSize);
    n_assert(incomningIndexBufferSize == indexBufferSize);
    vertexBuffer->Unmap();
    indexBuffer->Unmap();

    Util::Array<PrimitiveGroup> primitiveGroups;
    PrimitiveGroup group;
    group.SetBaseIndex(0);
    group.SetBaseVertex(0);
    group.SetNumIndices(numIndices);
    group.SetNumVertices(numVertices);
    group.SetPrimitiveTopology(PrimitiveTopology::TriangleList);    
    primitiveGroups.Append(group);

    Ptr<MemoryMeshLoader> memoryMeshLoader = MemoryMeshLoader::Create();
    memoryMeshLoader->SetVertexBuffer(vertexBuffer);
    memoryMeshLoader->SetIndexBuffer(indexBuffer);
    memoryMeshLoader->SetPrimitiveGroups(primitiveGroups);
    
    //compute the min and max fade distance in projection space
    float fadeMaxDist = ((grassRenderer->GetPatchArraySize() / 2) * patchSize) + patchSize * 0.5f;
    float fadeMinDist = fadeMaxDist - patchSize;

    // create a new model with shape node
    Ptr<Models::ShapeNode> shapeNode = Models::ShapeNode::Create();
    this->refShapeNode = shapeNode;
    /// setup shapenode parameter
    this->refShapeNode->SetName(shapeNodeName);
    this->refShapeNode->SetMeshResourceLoader(memoryMeshLoader.cast<ResourceLoader>());    
    this->refShapeNode->SetMeshResourceId(ResourceId(shapeNodeName + "_MemoryMesh"));
    this->refShapeNode->SetPrimitiveGroupIndex(0);
    this->refShapeNode->SetShader(ResourceId("shd:grass"));
    this->refShapeNode->SetType(ModelNodeType::FromName(ModelNodeType::Name("Alpha")));
    // create and register managed resources for rendertargets
    const Util::Array<Util::KeyValuePair<Util::StringAtom, Util::Variant>>& params = lib->GetPlantMeshAtIndex(0)->GetShapeNode()->GetShaderParameter();
    IndexT i;
    for (i = 0; i < params.Size() ; i++)
    {
        if (params[i].Key() == Util::StringAtom("DiffMap0"))	
        {
            this->refShapeNode->AddShaderParam(params[i].Key().Value(), params[i].Value());
        }
    }    
    this->refShapeNode->AddShaderParam("DiffMap1", Variant("GrassColorMapRT"));
    this->refShapeNode->AddShaderParam("DiffMap2", Variant("GrassLightMapRT"));
    this->refShapeNode->AddShaderParam("DiffMap3", Variant("GrassNormalMapRT"));
    this->refShapeNode->AddShaderParam("DiffMap4", Variant("GrassWeightMapRT"));
    this->refShapeNode->AddShaderParam("MinDist", Variant(fadeMinDist));
    this->refShapeNode->AddShaderParam("MaxDist", Variant(fadeMaxDist));

    // create managed model
    this->managedModel = ManagedModel::Create();
    this->managedModel->SetResourceId(ResourceId(patchRsrcName));
    Ptr<Model> model = Model::Create();
    managedModel->SetResource(model.cast<Resource>());
    model->AttachNode(this->refShapeNode.cast<ModelNode>());    
    Ptr<Vegetation::EmptyResourceLoader> emptyResLoader = Vegetation::EmptyResourceLoader::Create();
    emptyResLoader->SetState(Resource::Loaded);    
    model->SetLoader(emptyResLoader.cast<ResourceLoader>());
    model->LoadResources();
    model->Load();
    model->SetLoader(0);    
    this->refShapeNode->SetMeshResourceLoader(0);  

    this->refMesh = this->refShapeNode->GetManagedMesh()->GetMesh();

    //create a graphics entity and set resource to the shape node we just created,
    //do not attach it to the world yet (this happens in Activate()
    this->graphicsEntity = InternalModelEntity::Create();    
    this->graphicsEntity->SetResourceId(ResourceId(patchRsrcName));  

    // store the original minY and maxY of the graphics entity's bounding box
    this->origMinY = this->graphicsEntity->GetGlobalBoundingBox().pmin.y();
    this->origMaxY = this->graphicsEntity->GetGlobalBoundingBox().pmax.y();
}

//------------------------------------------------------------------------------
/**
    Validate() the grass patch, this must be called whenever IsValid()
    return false (which is usually the case when a display mode switch
    or switch to desktop happens).
*/
void
GrassPatch::Validate()
{
    n_assert(this->GetResourceState() == Resource::Initial);
    n_assert(0 != this->grassLibraryBackPointer);
    this->Setup(this->grassLibraryBackPointer, this->grassLibraryPatchIndex);
}

//------------------------------------------------------------------------------
/**
    Allocates the height lookup array which contains precomputed values
    for the per-vertex height lookup.
*/
void
GrassPatch::AllocHeightLookupArray(int numPlants)
{
    if (0 == this->mapLookupArray)
    {
        this->mapLookupArray = (LookupInfo*) Memory::Alloc(Memory::ResourceHeap, numPlants * sizeof(LookupInfo));
    }    
}

//------------------------------------------------------------------------------
/**
    This creates the heightMapIndices array which contains precomputed
    per-vertex-indices into the height map.
*/
void
GrassPatch::SetupMapLookupInfos(const Math::vector& plantMidPoint, float* vertexPtr, int firstVertexIndex, int numVertices, int vertexWidth)
{
    //n_assert(0 != this->mapLookupArray);
    Ptr<GrassHeightMap> heightMap = GrassRenderer::Instance()->HeightMap();
    Ptr<GrassGroundMap> groundMap = GrassRenderer::Instance()->GroundMap();
    n_assert(0 != heightMap);
    n_assert(0 != groundMap);

    // compute the lookup index for the midpoint, this is the same
    // for all vertices of a plant
    int midPointIndex = heightMap->ComputeLookupIndex(plantMidPoint.x(), plantMidPoint.z());
    groundMap->BeginLookupWeightMap();
    groundMap->BeginLookupPatchMap();
    int i;
    Math::float2 midPoint2(plantMidPoint.x(), plantMidPoint.z());
    Math::float2 vertexCoord2;
    for (i = firstVertexIndex; i < (firstVertexIndex + numVertices); i++)
    {
        LookupInfo& cur = this->mapLookupArray[i];
        const vector& vertexCoord = *((vector*)vertexPtr);

        cur.vertexHeightMapIndex = heightMap->ComputeLookupIndex(vertexCoord.x(), vertexCoord.z());
        cur.vertexWeightMapIndex = groundMap->ComputeWeightMapVertexIndex(vertexCoord.x(), vertexCoord.z());
        cur.midPointHeightMapIndex = midPointIndex;

        // compute the maximum admissible height difference between midpoint
        // and current vertex, this is used to prevent stretching of plant
        // vertices across cliffs
        vertexCoord2.set(vertexCoord.x(), vertexCoord.z());
        vertexCoord2 -= midPoint2;
        cur.maxHeightDist = vertexCoord2.length();

        vertexPtr += vertexWidth;
    }    
    groundMap->EndLookupPatchMap();
    groundMap->EndLookupWeightMap();   
}

//------------------------------------------------------------------------------
/**
    Check if our loaded resource is valid. The resource may become
    invalid when the display mode is changed.
*/
Resource::State
GrassPatch::GetResourceState() const
{
    if (this->refMesh.isvalid()) 
    {
        return this->refMesh->GetState();
    }
    else
    {
        return Resource::Initial;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPatch::Activate(int channelIndex)
{
    n_assert(!this->isActive);
    this->isActive = true;

    // set our own loaded managed model
    this->graphicsEntity->SetManagedModel(this->managedModel);  

    // convert the channelIndex into an RGBA mask    
    switch (channelIndex)
    {
        case 0: 
            this->vecRGBAMask.set(1.0f, 0.0f, 0.0f, 0.0f); 
            this->intRGBAMask = 0x00f00000;
            break;
        case 1: 
            this->vecRGBAMask.set(0.0f, 1.0f, 0.0f, 0.0f); 
            this->intRGBAMask = 0x0000f000;
            break;
        case 2: 
            this->vecRGBAMask.set(0.0f, 0.0f, 1.0f, 0.0f); 
            this->intRGBAMask = 0x000000f0;
            break;
        case 3: 
            this->vecRGBAMask.set(0.0f, 0.0f, 0.0f, 1.0f);
            this->intRGBAMask = 0xf0000000;
            break;
    }
    const Ptr<ShaderInstance>& shaderInstance = this->refShapeNode->GetShaderInstance();
    const Ptr<ShaderVariable>& shaderVariable = shaderInstance->GetVariableBySemantic(ShaderVariable::Semantic("RgbaMask"));
    shaderVariable->SetFloat4(this->vecRGBAMask);

    // note: first validate the graphics entity, this prevents
    // that AttachEntity() overwrites the bounding box with the
    // Nebula2 object's box
    //this->graphicsEntity->ValidateResource();

    // compute the start index into the weight map
    this->weightMapStartIndex = GrassRenderer::Instance()->GroundMap()->ComputeWeightMapStartIndex(this->pos);

    // update dynamic vertex attributes    
    this->Update();

    // attach grass patch to graphics level
    Math::matrix44 worldSpaceMatrix = matrix44::identity();
    worldSpaceMatrix.translate(this->pos.GetAsWorldSpace());
    this->graphicsEntity->SetTransform(worldSpaceMatrix);
    const Ptr<InternalStage>& defaultStage = InternalGraphicsServer::Instance()->GetDefaultView()->GetStage();
    defaultStage->AttachEntity(this->graphicsEntity.cast<InternalGraphicsEntity>()); 

    if (!this->textureTransformVariable.isvalid()
        && this->graphicsEntity->GetModelResourceState() == Resource::Loaded)
    {
        // get shadervariableinstance from shapenode
        const StringAtom& shapeNodeInstanceName = this->refShapeNode->GetName();
        Ptr<ShapeNodeInstance> modelNode = this->graphicsEntity->GetModelInstance()->LookupNodeInstance(shapeNodeInstanceName.Value()).downcast<ShapeNodeInstance>();
        if (modelNode->HasShaderVariableInstance(ShaderVariable::Semantic("TextureTransform0")))
        {
            this->textureTransformVariable = modelNode->GetShaderVariableInstance(ShaderVariable::Semantic("TextureTransform0"));
        }
        else
        {
            this->textureTransformVariable = modelNode->CreateShaderVariableInstance(ShaderVariable::Semantic("TextureTransform0"));
        }        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPatch::Deactivate()
{
    n_assert(this->isActive);
    if (InternalGraphicsServer::Instance()->GetDefaultView().isvalid())
    {
        const Ptr<InternalStage>& defaultStage = InternalGraphicsServer::Instance()->GetDefaultView()->GetStage();
        defaultStage->RemoveEntity(this->graphicsEntity.cast<InternalGraphicsEntity>());         
    }
    n_assert(!this->graphicsEntity->IsAttachedToStage());
    this->isActive = false;
}

//------------------------------------------------------------------------------
/**
    Called by grass patch array on all active grass patches
    when a patch boundary is crossed.
*/
void
GrassPatch::OnBoundaryCrossed()
{
    if (this->textureTransformVariable.isvalid())
    {
        // compute a model-to-uv-matrix and set as shader override
        Math::matrix44 modelToUv = GrassRenderer::Instance()->PatchArray()->ComputeModelToUvTransform(this->pos);
        this->textureTransformVariable->SetMatrix(modelToUv);
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPatch::Update()
{
    n_assert(this->isActive);

    if (this->textureTransformVariable.isvalid())
    {        
        // render a height map for this patch
        Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
        Ptr<GrassHeightMap> heightMap = grassRenderer->HeightMap();
        Ptr<GrassGroundMap> groundMap = grassRenderer->GroundMap();
        heightMap->Begin(this->pos, grassRenderer->GetCameraTransform().get_position());
        groundMap->BeginLookupWeightMap();
        float minY = 10000.0f;
        float maxY = -10000.0f;

        // fix vertex heights, record min/max Y to fix bounding box,
        // each vertex's height is checked against the midpoint height
        // of its plant, if the difference is greater then the precomputed
        // admissible difference this means we're at a cliff and the vertex
        // height will be set to the midpoint height
        const int dynHeightOffset = GrassPlantMesh::GetDynamicHeightVertexOffset();
        const Ptr<VertexBuffer>& vertexBuffer = this->refMesh->GetVertexBuffer();
        const Ptr<VertexLayout>& vertexLayout = vertexBuffer->GetVertexLayout();
        short* ptr =  (short*)((float*)vertexBuffer->Map(VertexBuffer::MapWrite) + dynHeightOffset);
        int numVertices = vertexBuffer->GetNumVertices();
        int vertexWidthShort = vertexLayout->GetVertexByteSize() / sizeof(short);  // extend from sizeof(float) to sizeof(short)
        float midPointHeight = 0.0f;
        uint curMidPointIndex = (uint) InvalidIndex;
        IndexT i;
        for (i = 0; i < numVertices; i++)
        {
            const LookupInfo& lookupInfo = this->mapLookupArray[i];

            // update midpoint height of current plant if necessary
            if (lookupInfo.midPointHeightMapIndex != curMidPointIndex)
            {
                curMidPointIndex = lookupInfo.midPointHeightMapIndex;
                midPointHeight = heightMap->LookupHeight(curMidPointIndex);
            }

            // lookup vertex height
            float vertexHeight = heightMap->LookupHeight(lookupInfo.vertexHeightMapIndex);

            // check height difference to detect cliffs
            if (n_abs(vertexHeight - midPointHeight) > lookupInfo.maxHeightDist)
            {
                // move problematic cliff grass under the ground
                ptr[0] = (short) (midPointHeight * 256.0f);
                ptr[1] = 0;
            }
            else
            {
                // write per-vertex height into mesh (as 8.8 fixed point!)
                ptr[0] = (short) (vertexHeight * 256.0f);

                // lookup the weight value of the vertex and write it as packed value
                uint weightMapIndex = this->weightMapStartIndex + lookupInfo.vertexWeightMapIndex;
                uint rgbaWeight = groundMap->LookupWeightMap(weightMapIndex);
                if ((rgbaWeight & this->intRGBAMask) != 0)
                {
                    ptr[1] = 1;
                }
                else
                {
                    ptr[1] = 0;
                }
            }

            // update min/max heights
            if (vertexHeight < minY) minY = vertexHeight;
            if (vertexHeight > maxY) maxY = vertexHeight;
            
            // advance vertex pointer
            ptr += vertexWidthShort;
        }
        vertexBuffer->Unmap();
        groundMap->EndLookupWeightMap();
        heightMap->End();

        // fix the bounding box of our graphics entity
        bbox localBox = this->graphicsEntity->GetLocalBoundingBox();
        localBox.pmin.y() = minY - this->origMinY;
        localBox.pmax.y() = maxY + this->origMaxY;
        this->graphicsEntity->SetLocalBoundingBox(localBox);
    }
}

}