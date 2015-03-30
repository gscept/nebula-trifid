//------------------------------------------------------------------------------
//  grassrenderer.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grassrenderer.h"
#include "vegetation/grasspatcharray.h"
#include "vegetation/grassheightmap.h"
#include "vegetation/grassgroundmap.h"
#include "graphics/graphicsserver.h"
#include "util/commandlineargs.h"
#include "models/modelinstance.h"
#include "models/modelnode.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalview.h"
#include "internalgraphics/internalcameraentity.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassRenderer, 'VGRE', Core::RefCounted);
__ImplementSingleton(Vegetation::GrassRenderer);

using namespace InternalGraphics;
using namespace Models;
using namespace Util;

#define Layer0Attr Util::String("GrassLayer0")
#define Layer1Attr Util::String("GrassLayer1")
#define Layer2Attr Util::String("GrassLayer2")
#define Layer3Attr Util::String("GrassLayer3")

//------------------------------------------------------------------------------
/**
*/
GrassRenderer::GrassRenderer() :
    isOpen(false),
    grassRenderingEnabled(true),
    patchArraySize(9),
    numPlantsPerPatchDimension(6),
    maxRandomPlantSpread(0.15f),
    allResourcesValid(false),
    frameId(0)
{
    __ConstructSingleton;  
}

//------------------------------------------------------------------------------
/**
*/
GrassRenderer::~GrassRenderer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Setup grass layers from a graphics entity. This will check the graphics
    resource of the entity for nMultilayerShapeNodes, and check they have
    any grass layers defined. If yes, they will be added.
*/
void
GrassRenderer::SetupGrassLayersFromGraphicsEntity(const Ptr<InternalModelEntity>& gfxEntity)
{
    n_assert(0 != gfxEntity);   
    
    const Ptr<ModelInstance>& modelInst = gfxEntity->GetModelInstance();
    const Ptr<Model>& model = modelInst->GetModel();
    const Ptr<ModelNode>& rootNode = model->GetRootNode();
    n_assert(0 != rootNode);    
    
    this->ScanNodesForGrassLayer(gfxEntity, rootNode->GetChildren());
}

//------------------------------------------------------------------------------
/**
*/
void 
GrassRenderer::ScanNodesForGrassLayer(const Ptr<InternalGraphics::InternalModelEntity> gfxEntity, Util::Array<Ptr<ModelNode>> nodes)
{
    IndexT i;
    for (i = 0; i < nodes.Size(); i++)
    {            
        // check for GrassLayerX-Attributes
        if (nodes[i]->HasStringAttr(Layer0Attr))
        {
            this->SetupGrassLayerFromMultilayerNode(gfxEntity, Layer0Attr, nodes[i]->GetStringAttr(Layer0Attr));
        }
        if (nodes[i]->HasStringAttr(Layer1Attr))
        {
            this->SetupGrassLayerFromMultilayerNode(gfxEntity, Layer1Attr, nodes[i]->GetStringAttr(Layer1Attr));
        }
        if (nodes[i]->HasStringAttr(Layer2Attr))
        {
            this->SetupGrassLayerFromMultilayerNode(gfxEntity, Layer2Attr, nodes[i]->GetStringAttr(Layer2Attr));
        }
        if (nodes[i]->HasStringAttr(Layer3Attr))
        {
            this->SetupGrassLayerFromMultilayerNode(gfxEntity, Layer3Attr, nodes[i]->GetStringAttr(Layer3Attr));
        }
        if (nodes[i]->GetChildren().Size())
        {
            this->ScanNodesForGrassLayer(gfxEntity, nodes[i]->GetChildren());
        }
    }     
}

//------------------------------------------------------------------------------
/**
    Setup a single grass layer from a multilayer node attribute (as
    exported by the Nebula2 Toolkit For Maya). This is just a private
    helper method called by SetupGrassLayersFromGraphicsEntity().
*/
void
GrassRenderer::SetupGrassLayerFromMultilayerNode(const Ptr<InternalGraphics::InternalModelEntity> gfxEntity, const Util::StringAtom& attrName, const Util::StringAtom& attrValue)
{
    n_assert(0 != gfxEntity);    
    n_assert(attrValue.IsValid());

    // split string into key/values
    Util::Dictionary<Util::String, Util::String> args;
    Util::Array<String> tokens = attrValue.AsString().Tokenize(" \t\n=", '"');
    n_assert(0 == (tokens.Size() & 1)); // num tokens must be even
    IndexT i;
    for (i = 0; i < tokens.Size(); i += 2)
    {
        args.Add(tokens[i], tokens[i + 1]);
    }
    n_assert(args.Contains("chn"));
    n_assert(args.Contains("lib"));
    int chn = args["chn"].AsInt();
    const Util::String& lib = args["lib"];
    if (!lib.IsValid())
    {
        n_error("GrassRenderer: no grass library set on grass layer in '%s'!", gfxEntity->GetResourceId().Value());
        return;
    }

    // add a new grass layer
    GrassLayerInfo inf;
    inf.SetLayerName(attrName.Value());
    inf.SetWeightTextureChannelIndex(chn);
    inf.SetGrassLibraryName(lib);
    inf.SetGroundName(gfxEntity->GetResourceId().Value());
    inf.SetGroundTransform(gfxEntity->GetTransform());
    this->AddGrassLayer(inf);
}

//------------------------------------------------------------------------------
/**
    Open the grass renderer. Before opening, all grass layers must 
    have been defined with AddGrassLayer().
*/
bool
GrassRenderer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->patchArray.isvalid());
    this->isOpen = true;
    this->allResourcesValid = false;

    // NOTE: create render targets as early as possible 
    this->heightMap = GrassHeightMap::Create();
    this->heightMap->Setup();
    this->groundMap = GrassGroundMap::Create();
    this->groundMap->Setup();

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the grass renderer. All resources will be freed.
*/
void
GrassRenderer::Close()
{
    n_assert(this->IsOpen());
    this->libraries.Clear();
    this->groundEntities.Clear();
    this->layers.Clear();
    this->patchArray = 0;
    this->isOpen = false;
    this->groundMap = 0;
    this->heightMap = 0;
}

//------------------------------------------------------------------------------
/**
    Add a new grass layer. This must happen before opening the grass renderer.
*/
void
GrassRenderer::AddGrassLayer(const GrassLayerInfo& grassLayerInfo)
{
    // check if a layer with the same name already has been registered
    if (!this->layers.Contains(grassLayerInfo.GetLayerName()))
    {
        // create a new grass layer object
        Ptr<GrassLayer> newGrassLayer = GrassLayer::Create();
        newGrassLayer->Setup(grassLayerInfo);
        this->layers.Add(grassLayerInfo.GetLayerName(), newGrassLayer);
    }

    // create a new ground entity if it doesn't exist yet
    const Util::String& groundResName = grassLayerInfo.GetGroundName();
    if (!this->groundEntities.Contains(groundResName))
    {
        Ptr<InternalGraphics::InternalModelEntity> gfxEntity = InternalGraphics::InternalModelEntity::Create();
        gfxEntity->SetResourceId(groundResName);
        gfxEntity->SetTransform(grassLayerInfo.GetGroundTransform());
        this->groundEntities.Add(groundResName, gfxEntity);
    }
}

//------------------------------------------------------------------------------
/**
    Update the grass renderer. This will maintain a 2D array of visible
    grass patches around the current camera position. Call this method per
    frame, and set the current camera transform before caling the method.
    Most of the time, this will just render the currently active
    grass patches as simple static meshes. When the viewer crosses a patch
    boundary, grass patches will be allocated and discarded as needed.
*/
void
GrassRenderer::Update()
{
    n_assert(this->IsOpen());
    
    // check if all resources are valid
    if (!this->allResourcesValid)
    {
        this->ValidateResources();
    }
    else
    {
        // only do something if there are grass layers attached (otherwise
        // the current level doesn't have grass)
        if (this->grassRenderingEnabled && (this->layers.Size() > 0))
        {
            // get current camera transform und update the patch array        
            const Ptr<InternalView>& view = InternalGraphicsServer::Instance()->GetDefaultView();
            if (view.isvalid())
            {                
                this->cameraTransform = view->GetCameraEntity()->GetTransform();
                this->patchArray->Update(this->cameraTransform);
            }        
        }
    }
    this->frameId++;
}

//------------------------------------------------------------------------------
/**
*/
Util::Dictionary<Util::String, Ptr<GrassLibrary> >&
GrassRenderer::Libraries()
{
    return this->libraries;
}

//------------------------------------------------------------------------------
/**
*/
Util::Dictionary<Util::String, Ptr<GrassLayer> >&
GrassRenderer::Layers()
{
    return this->layers;
}

//------------------------------------------------------------------------------
/**
*/
Util::Dictionary<Util::String,Ptr<InternalGraphics::InternalModelEntity> >&
GrassRenderer::GroundEntities()
{
    return this->groundEntities;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<GrassHeightMap>
GrassRenderer::HeightMap()
{
    return this->heightMap;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<GrassGroundMap>
GrassRenderer::GroundMap()
{
    return this->groundMap;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<GrassPatchArray>
GrassRenderer::PatchArray()
{
    return this->patchArray;
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44&
GrassRenderer::GetCameraTransform() const
{
    return this->cameraTransform;
}

//------------------------------------------------------------------------------
/**
*/
void
GrassRenderer::SetGrassRenderingEnabled(bool b)
{
    this->grassRenderingEnabled = b;
    int i;
    for (i = 0; i < this->libraries.Size(); i++)
    {
        this->libraries.ValueAtIndex(i)->SetGrassPatchVisibility(b);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GrassRenderer::ScanGraphicsEntities(const Util::Array<Ptr<InternalGraphics::InternalGraphicsEntity>> & entities)
{
    IndexT i;
    for (i = 0; i < entities.Size(); i++)
    {
        if (entities[i]->IsA(InternalModelEntity::RTTI))
        {
            this->SetupGrassLayersFromGraphicsEntity(entities[i].cast<InternalModelEntity>());
        }        
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GrassRenderer::ValidateResources()
{
    n_assert(!this->allResourcesValid);
    
    if (this->libraries.Size() > 0)
    {
        this->allResourcesValid = true;
        IndexT i;
        for (i = 0; i < this->libraries.Size(); i++)
        {        
            if (!this->libraries.ValueAtIndex(i)->IsValid())
            {
                this->libraries.ValueAtIndex(i)->ValidateMesh();
            }
            this->allResourcesValid &= this->libraries.ValueAtIndex(i)->IsValid();    	
        }
    }

    if (this->allResourcesValid)
    {
        // create a grass patch array and open it
        this->patchArray = GrassPatchArray::Create();
        bool grassPatchArrayOpened = this->patchArray->Open();
        n_assert(grassPatchArrayOpened);
    }
}
} // namespace Vegetation
