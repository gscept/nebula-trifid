//------------------------------------------------------------------------------
//  framebatch.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framebatch.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/textrenderer.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/mouserenderdevice.h"
#include "models/visresolver.h"
#include "models/model.h"
#include "models/modelnodeinstance.h"
#include "models/nodes/statenode.h"
#include "models/nodes/statenodeinstance.h"
#include "materials/material.h"
#include "materials/materialinstance.h"
#include "graphics/modelentity.h"
#include "framesync/framesynctimer.h"
#include "lighting/lightserver.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "coregraphics/shadersemantics.h"
#include "instancing/instanceserver.h"
#include "materials/materialserver.h"
#include "coregraphics/transformdevice.h"


namespace Frame
{
__ImplementClass(Frame::FrameBatch, 'FBTH', Core::RefCounted);

using namespace Instancing;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Models;
using namespace Util;
using namespace Lighting;
using namespace FrameSync;
using namespace Materials;

//------------------------------------------------------------------------------
/**
*/
FrameBatch::FrameBatch() :
    batchType(FrameBatchType::InvalidBatchType),
    batchGroup(FrameBatchType::InvalidBatchType),
    lightingMode(LightingMode::None),
    sortingMode(SortingMode::None),
    shaderFeatures(0),
    forceInstancing(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FrameBatch::~FrameBatch()
{
    // make sure Discard() has been called
    n_assert(this->shaderVariables.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::Discard()
{
    this->shaderVariables.Clear();

    _discard_timer(this->debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::Render(IndexT frameIndex)
{
    RenderDevice* renderDevice = RenderDevice::Instance();

	/*
    // apply shader variables
    IndexT varIndex;
    for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
    {
        this->shaderVariables[varIndex]->Apply();
    }
	*/

    // render the batch
    this->RenderBatch(frameIndex);
}

//------------------------------------------------------------------------------
/**
	Hmm, we only do BeginBatch and EndBatch on objects, not on the special types.
*/
void
FrameBatch::RenderBatch(IndexT frameIndex)
{
    // at first call plugins 
    const Ptr<FrameBatch> batchPtr(this);
    RenderModules::RTPluginRegistry* rtPluginRegistry = RenderModules::RTPluginRegistry::Instance();
    rtPluginRegistry->OnRenderFrameBatch(batchPtr);

    // now do usual render stuff
    ShaderServer* shaderServer = ShaderServer::Instance();
    VisResolver* visResolver = VisResolver::Instance();      
    LightServer* lightServer = LightServer::Instance();
	RenderDevice* renderDevice = RenderDevice::Instance();
	InstanceServer* instanceServer = InstanceServer::Instance();
	MaterialServer* matServer = MaterialServer::Instance();
    
	_start_timer(this->debugTimer);
    // handle special cases
    if (FrameBatchType::UI == this->batchType)
    {
		// empty, ui renders in uirtplugin
    }
    else if (FrameBatchType::WiiHBM == this->batchType || FrameBatchType::WiiPanel == this->batchType)
    {
        // wii home menu batch will be handled with a RTplugin
        // and is only available for the wii platform
        #ifndef __WII__
        n_error("Batchtype %s only available on Wii!", FrameBatchType::ToString(this->batchType).AsCharPtr());
        #endif
    }
    else if (FrameBatchType::Shapes == this->batchType)
    {
        ShapeRenderer::Instance()->DrawShapes();
    }
    else if (FrameBatchType::Text == this->batchType)
    {
		TextRenderer::Instance()->DrawTextElements();
    }
    else if (FrameBatchType::ResolveDepthBuffer == this->batchType)
    {
        // resolve depth buffer to texture
        RenderDevice* renderDevice = RenderDevice::Instance();
        n_assert(renderDevice->HasPassRenderTarget());
        renderDevice->GetPassRenderTarget()->ResolveDepthBuffer();
    }
    else if (FrameBatchType::MousePointers == this->batchType)
    {
        MouseRenderDevice::Instance()->RenderPointers();
    }
    else if (FrameBatchType::Lights == this->batchType)
    {
        LightServer::Instance()->RenderLights();
    }
	else if (FrameBatchType::LightProbes == this->batchType)
	{
		LightServer::Instance()->RenderLightProbes();
	}
    else if (matServer->HasMaterialsByBatchGroup(this->batchGroup))
    {
        // get current frame index from graphics server
        IndexT frameIndex = FrameSyncTimer::Instance()->GetFrameIndex();

		// get materials matching the batch type
        const Util::Array<Ptr<Material>>& materials = matServer->GetMaterialsByBatchGroup(this->batchGroup);

		IndexT materialIndex;
		for (materialIndex = 0; materialIndex < materials.Size(); materialIndex++)
		{
			// get material
			const Ptr<Material>& material = materials[materialIndex];
            const Util::Array<Ptr<Surface>>& surfaces = material->GetSurfaces();

            IndexT surfaceIndex;
            for (surfaceIndex = 0; surfaceIndex < surfaces.Size(); surfaceIndex++)
            {
                const Ptr<Surface>& surface = surfaces[surfaceIndex];
                const Ptr<Shader>& shader = material->GetShaderByBatchGroup(this->batchGroup);

                // set the this shader to be the main active shader
                shaderServer->SetActiveShader(shader);

				// reset features, then set the features implemented by the material
				shaderServer->ResetFeatureBits();
				shaderServer->SetFeatureBits(material->GetFeatureMask(this->batchGroup));

				// apply shared model state (mesh)
				//modelNode->ApplySharedState(frameIndex);

				// apply shader 
				shader->SelectActiveVariation(shaderServer->GetFeatureBits());
				shader->Apply();

                // select variations based on the feature bits found in the material
                // shaderInst->SetWireframe(renderDevice->GetRenderWireframe());

                // get models based on material
                const Array<Ptr<Model>>& models = visResolver->GetVisibleModels(surface->GetSurfaceCode());

                IndexT modelIndex;
                for (modelIndex = 0; modelIndex < models.Size(); modelIndex++)
                {
                    FRAME_LOG("      FrameBatch::RenderBatch() model: %s", models[modelIndex]->GetResourceId().Value());

                    // for each visible model node of the model...
                    const Array<Ptr<ModelNode>>& modelNodes = visResolver->GetVisibleModelNodes(surface->GetSurfaceCode(), models[modelIndex]);
                    IndexT modelNodeIndex;
                    for (modelNodeIndex = 0; modelNodeIndex < modelNodes.Size(); modelNodeIndex++)
                    {
						// render instances
						const Ptr<ModelNode>& modelNode = modelNodes[modelNodeIndex];
						const Array<Ptr<ModelNodeInstance>>& nodeInstances = visResolver->GetVisibleModelNodeInstances(surface->GetSurfaceCode(), modelNode);
						if (nodeInstances.IsEmpty()) continue;

						// apply shared model state (mesh)
						modelNode->ApplySharedState(frameIndex);

                        // apply batch variables to currently active shader (will only work for variables which lies outside of constant buffers)
                        // this will also be overridden by the model local state (surface material), so only use this for system textures
                        shader->BeginUpdate();
                        IndexT variableIndex;
                        for (variableIndex = 0; variableIndex < this->shaderVariablesByName.Size(); variableIndex++)
                        {
                            // get variable instance
                            const Ptr<ShaderVariableInstance>& varInst = this->shaderVariablesByName.ValueAtIndex(variableIndex);

                            // get variable name
                            const Util::StringAtom& name = this->shaderVariablesByName.KeyAtIndex(variableIndex);

                            // apply to variable in active shader
                            varInst->ApplyTo(shader->GetVariableByName(name));
                        }
                        shader->EndUpdate();

                        FRAME_LOG("        FrameBatch::RenderBatch() node: %s", modelNode->GetName().Value());

                        // begin instancing, if we are doing force instancing, use the instancing count, otherwise the multiplier is 1
                        if (this->forceInstancing) instanceServer->BeginInstancing(modelNode, this->instancingCount, shader, this->batchGroup);
                        else					   instanceServer->BeginInstancing(modelNode, 1, shader, this->batchGroup);

                        // start batch
                        renderDevice->BeginBatch(this->batchType);

                        IndexT nodeInstIndex;
                        for (nodeInstIndex = 0; nodeInstIndex < nodeInstances.Size(); nodeInstIndex++)
                        {
                            const Ptr<ModelNodeInstance>& nodeInstance = nodeInstances[nodeInstIndex];
                            const Ptr<StateNodeInstance>& stateNode = nodeInstance.downcast<StateNodeInstance>();
                            const Ptr<SurfaceInstance>& surfaceInstance = stateNode->GetSurfaceInstance();
                            const Ptr<ShaderInstance>& shaderInst = surfaceInstance->GetShaderInstance(this->batchGroup);

                            // if single-pass lighting is enabled, we need to setup the lighting 
                            // shader states
                            // FIXME: This may set a new shader variation for every node instance
                            // which is expensive! Would be better to sort node instances by number
                            // of active lights!!!
                            if (LightingMode::Forward == this->lightingMode)
                            {
                                // setup lighting render states
                                // NOTE: this may change the shader feature bit mask which may select
                                // a different shader variation per entity
                                const Ptr<Graphics::ModelEntity>& modelEntity = nodeInstance->GetModelInstance()->GetModelEntity();
                                lightServer->ApplyModelEntityLights(modelEntity);
                            }

                            // if lighting mode is Off, we can render all node instances with the same shader
                            /*
                            SizeT numPasses = shaderInst->Begin();
                            n_assert(1 == numPasses);
                            shaderInst->BeginPass(0);
                            */

                        #if NEBULA3_ENABLE_PROFILING
                            nodeInstance->StartDebugTimer();
                        #endif  

                            if (nodeInstance->GetModelInstance()->IsInstanced())
                            {
                                // add to server
                                const Ptr<ModelInstance>& modelInstance = nodeInstance->GetModelInstance();
                                const Ptr<ModelEntity>& entity = modelInstance->GetModelEntity();

                                // add instance
                                if (entity.isvalid())   instanceServer->AddInstance(entity->GetInstanceCode(), nodeInstance);
                                else                    instanceServer->AddInstance(0, nodeInstance);
                            }
                            else
                            {
                                // render the node instance
                                nodeInstance->ApplyState(frameIndex, this->batchGroup, shader);

								// commit shader
								// shader->Commit();

                                // perform rendering
                                if (this->forceInstancing)
                                {
                                    // render instanced
                                    nodeInstance->RenderInstanced(this->instancingCount);
                                }
                                else
                                {
                                    // render single
                                    nodeInstance->Render();
                                }
                            }

                        #if NEBULA3_ENABLE_PROFILING
                            nodeInstance->StopDebugTimer();
                        #endif  

                            /*
                            if (LightingMode::None == this->lightingMode)
                            {
                                shaderInst->EndPass();
                                shaderInst->End();
                            }
                            */
                        }

                        // end batch
                        renderDevice->EndBatch();

                        // render instances
                        instanceServer->Render(frameIndex);

                        // end instancing
                        instanceServer->EndInstancing();
                    }
                }
            }
		}
    }

    _stop_timer(this->debugTimer);
}

#if NEBULA3_ENABLE_PROFILING
//------------------------------------------------------------------------------
/**
*/
void 
FrameBatch::SetBatchDebugTimer(const Util::String& name)
{
    this->debugTimer = Debug::DebugTimer::Create();
    this->debugTimer->Setup(name);
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
SortByPriority(const Ptr<ModelNodeInstance>& lhs, const Ptr<ModelNodeInstance>& rhs)
{
	/*
	GraphicsEntity::PriorityType lhsPrio = lhs->GetModelInstance()->GetModelEntity()->GetPriority();
	GraphicsEntity::PriorityType rhsPrio = rhs->GetModelInstance()->GetModelEntity()->GetPriority();
	return lhsPrio > rhsPrio;
	*/
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
SortByFrontToBack(const Ptr<ModelNodeInstance>& lhs, const Ptr<ModelNodeInstance>& rhs)
{
	Ptr<CameraEntity> cam = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
	const Math::point& lhsPos = lhs.downcast<TransformNodeInstance>()->GetPosition();
	const Math::point& rhsPos = rhs.downcast<TransformNodeInstance>()->GetPosition();
	Math::scalar lhsDist = (cam->GetTransform().get_position() - lhsPos).lengthsq3();
	Math::scalar rhsDist = (cam->GetTransform().get_position() - rhsPos).lengthsq3();
	return lhsDist < rhsDist;
}

//------------------------------------------------------------------------------
/**
*/
bool
SortByBackToFront(const Ptr<ModelNodeInstance>& lhs, const Ptr<ModelNodeInstance>& rhs)
{
	Ptr<CameraEntity> cam = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
	const Math::point& lhsPos = lhs.downcast<TransformNodeInstance>()->GetPosition();
	const Math::point& rhsPos = rhs.downcast<TransformNodeInstance>()->GetPosition();
	Math::scalar lhsDist = (cam->GetTransform().get_position() - lhsPos).lengthsq3();
	Math::scalar rhsDist = (cam->GetTransform().get_position() - rhsPos).lengthsq3();
	return lhsDist > rhsDist;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameBatch::Sort(Array<Ptr<ModelNodeInstance>>& instances)
{
	switch (this->sortingMode)
	{
	case SortingMode::Priority:
		instances.SortWithFunc(SortByPriority);
		break;
	case SortingMode::FrontToBack:
		instances.SortWithFunc(SortByFrontToBack);
		break;
	case SortingMode::BackToFront:
		instances.SortWithFunc(SortByBackToFront);
		break;
	}
}




} // namespace Frame
