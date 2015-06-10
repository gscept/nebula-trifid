//------------------------------------------------------------------------------
//  gridrtplugin.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
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
	this->shader = ShaderServer::Instance()->CreateShaderInstance("shd:silhouette");
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
	this->shader->Discard();
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

    if (frameBatch->GetType() == FrameBatchType::Shapes)
	{
		const Ptr<ShaderServer>& shaderServer = ShaderServer::Instance();
		shaderServer->SetActiveShaderInstance(this->shader);

		IndexT variationIndex;
		for (variationIndex = 0; variationIndex < 2; variationIndex++)
		{
			// set shading 
			shaderServer->ResetFeatureBits();
			if (variationIndex == 0)		shaderServer->SetFeatureBits(this->prepassVariation);
			else if (variationIndex == 1)	shaderServer->SetFeatureBits(this->outlineVariation);			

			IndexT modelIndex;
			for (modelIndex = 0; modelIndex < this->models.Size(); modelIndex++)
			{
				const Ptr<Graphics::ModelEntity>& model = models[modelIndex];
				if (model->IsValid())
				{
					const Util::Array<Ptr<ModelNode>>& nodes = model->GetModelInstance()->GetModel()->GetNodes();
					const Util::Array<Ptr<ModelNodeInstance>>& nodeInstances = model->GetModelInstance()->GetNodeInstances();

					// set transform
					this->colorVar->SetFloat4(this->color);

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

							// start pass
							this->shader->SelectActiveVariation(shaderServer->GetFeatureBits());
							this->shader->Begin();
							this->shader->BeginPass(0);

							// apply node
							nodeInstance->ApplyState(this->shader);

							// only apply model for shapes
							if (nodeInstance->IsA(ShapeNodeInstance::RTTI))
							{
								// draw!
								this->shader->Commit();
								nodeInstance->Render();

								// do post draw
								this->shader->PostDraw();
							}

							// end pass
							this->shader->EndPass();
							this->shader->End();
						}						
					}
				}
			}
		}
	}
}


} // namespace Grid