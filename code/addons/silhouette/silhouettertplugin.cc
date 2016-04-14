//------------------------------------------------------------------------------
//  gridrtplugin.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "silhouettertplugin.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/displaydevice.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shadersemantics.h"
#include "models/modelnode.h"
#include "models/nodes/shapenode.h"
#include "models/nodes/shapenodeinstance.h"
#include "characters/characterskinnodeinstance.h"
#include "particles/particlesystemnodeinstance.h"
#include "frame/frameserver.h"
#include "framesync/framesynctimer.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
using namespace Models;
using namespace Characters;
using namespace Particles;
namespace Silhouette
{
__ImplementClass(Silhouette::SilhouetteRTPlugin, 'SIRT', RenderModules::RTPlugin);

//------------------------------------------------------------------------------
/**
*/
SilhouetteRTPlugin::SilhouetteRTPlugin() :
	visible(true),
	model(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SilhouetteRTPlugin::~SilhouetteRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
SilhouetteRTPlugin::OnRegister()
{
	// create new shader
	this->shader = ShaderServer::Instance()->GetShader("shd:silhouette");
	this->colorVar = this->shader->GetVariableByName(NEBULA3_SEMANTIC_MATDIFFUSE);
	this->prepassVariation = ShaderServer::Instance()->FeatureStringToMask("Alt0");
	this->outlineVariation = ShaderServer::Instance()->FeatureStringToMask("Alt1");
}

//------------------------------------------------------------------------------
/**
*/
void
SilhouetteRTPlugin::OnUnregister()
{
	this->shader = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SilhouetteRTPlugin::OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch)
{
#if __OGL4__
	glLineWidth(5.0f);
#endif
    // get current frame index from graphics server
    IndexT frameIndex = FrameSync::FrameSyncTimer::Instance()->GetFrameIndex();

    if (frameBatch->GetType() == FrameBatchType::Shapes)
	{
		const Ptr<ShaderServer>& shaderServer = ShaderServer::Instance();
		shaderServer->SetActiveShader(this->shader);

		IndexT variationIndex;
		for (variationIndex = 0; variationIndex < 2; variationIndex++)
		{
			// set shading 
			shaderServer->ResetFeatureBits();
			if (variationIndex == 0)		shaderServer->SetFeatureBits(this->prepassVariation);
			else if (variationIndex == 1)	shaderServer->SetFeatureBits(this->outlineVariation);	

			// start pass
			this->shader->SelectActiveVariation(shaderServer->GetFeatureBits());
			this->shader->Apply();

			IndexT modelIndex;			
			for (modelIndex = 0; modelIndex < this->models.Size(); modelIndex++)
			{
				const Util::KeyValuePair<Math::float4, Util::Array<Ptr<Graphics::ModelEntity>>> & entry = models.ValueAtIndex(modelIndex);
				const Util::Array<Ptr<Graphics::ModelEntity>>& groupModels = entry.Value();
				for (IndexT i = 0;i < groupModels.Size();i++)
				{
					const Ptr<Graphics::ModelEntity>& model = groupModels[i];
					if (model->IsValid() && model->GetModelInstance().isvalid())
					{
						const Util::Array<Ptr<ModelNode>>& nodes = model->GetModelInstance()->GetModel()->GetNodes();
						const Util::Array<Ptr<ModelNodeInstance>>& nodeInstances = model->GetModelInstance()->GetNodeInstances();

						// set transform
						this->shader->BeginUpdate();
						this->colorVar->SetFloat4(entry.Key());
						this->shader->EndUpdate();

						// render stencil first
						IndexT nodeInstIndex;
						for (nodeInstIndex = 0; nodeInstIndex < nodeInstances.Size(); nodeInstIndex++)
						{
							//get node
							Ptr<ModelNodeInstance> nodeInstance = nodeInstances[nodeInstIndex];

							if (nodeInstance->IsVisible())
							{
								// apply shared state, which means mesh and feature bits
								nodeInstance->GetModelNode()->ApplySharedState(-1);

								// apply node
								nodeInstance->ApplyState(frameIndex, 0, this->shader);

								// only apply model for shapes
								if (nodeInstance->IsA(ShapeNodeInstance::RTTI))
								{
									// draw!
									this->shader->Commit();
									nodeInstance->Render();
								}
							}
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SilhouetteRTPlugin::SetModels(const Util::String& group, const Util::Array<Ptr<Graphics::ModelEntity>>& mdls, const Math::float4& colour)
{
	if (this->models.Contains(group))
	{
		this->models[group] = Util::KeyValuePair<Math::float4, Util::Array<Ptr<Graphics::ModelEntity>>>(colour, mdls);
	}
	else
	{
		this->models.Add(group, Util::KeyValuePair<Math::float4, Util::Array<Ptr<Graphics::ModelEntity>>>(colour, mdls));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SilhouetteRTPlugin::ClearModelGroup(const Util::String& group)
{
	n_assert(this->models.Contains(group));
	this->models.Erase(group);
}

} // namespace Grid