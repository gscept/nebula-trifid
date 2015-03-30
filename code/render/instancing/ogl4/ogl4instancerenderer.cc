//------------------------------------------------------------------------------
//  ogl4instancerenderer.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ogl4instancerenderer.h"
#include "coregraphics/ogl4/ogl4renderdevice.h"
#include "coregraphics/shadersemantics.h"

using namespace Math;
using namespace OpenGL4;
using namespace CoreGraphics;
namespace Instancing
{
__ImplementClass(Instancing::OGL4InstanceRenderer, 'O4IR', Instancing::InstanceRendererBase);

//------------------------------------------------------------------------------
/**
*/
OGL4InstanceRenderer::OGL4InstanceRenderer() :
	modelArraySemantic(NEBULA3_SEMANTIC_MODELARRAY),
	modelViewArraySemantic(NEBULA3_SEMANTIC_MODELVIEWARRAY),
	modelViewProjectionArraySemantic(NEBULA3_SEMANTIC_MODELVIEWPROJECTIONARRAY),
    objectIdArraySemantic(NEBULA3_SEMANTIC_OBJECTIDARRAY)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4InstanceRenderer::~OGL4InstanceRenderer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4InstanceRenderer::Render(const SizeT multiplier)
{
	n_assert(this->shader->IsValid());

	// get render device
	Ptr<OGL4RenderDevice> renderDev = OGL4RenderDevice::Instance();

	// create shader variables
	Ptr<ShaderVariable> modelArrayVar = 0;
	Ptr<ShaderVariable> modelViewArrayVar = 0;
	Ptr<ShaderVariable> modelViewProjectionArrayVar = 0;
    Ptr<ShaderVariable> objectIdArrayVar;

	// get variables
	if (this->shader->HasVariableBySemantic(this->modelArraySemantic))
	{
		modelArrayVar = this->shader->GetVariableBySemantic(this->modelArraySemantic);
	}

	if (this->shader->HasVariableBySemantic(this->modelViewArraySemantic))
	{
		modelViewArrayVar = this->shader->GetVariableBySemantic(this->modelViewArraySemantic);
	}

	if (this->shader->HasVariableBySemantic(this->modelViewProjectionArraySemantic))
	{
		modelViewProjectionArrayVar = this->shader->GetVariableBySemantic(this->modelViewProjectionArraySemantic);
	}

    if (this->shader->HasVariableBySemantic(this->objectIdArraySemantic))
    {
        objectIdArrayVar = this->shader->GetVariableBySemantic(this->objectIdArraySemantic);
    }

	// get pointer to matrix array
	matrix44* modelTrans = &this->modelTransforms[0];
	matrix44* modelViewTrans = &this->modelViewTransforms[0];
	matrix44* modelViewProjTrans = &this->modelViewProjectionTransforms[0];
    int* objectIdArray = &this->objectIds[0];

	// we assume all arrays are equally big
	SizeT instances = this->modelTransforms.Size();
	while (instances > 0)
	{
		// calculate how many transforms we will set in this batch
		int numBatchInstances = n_min(instances, this->MaxInstancesPerBatch);

		// apply variables
		if (modelArrayVar.isvalid())
		{
			modelArrayVar->SetMatrixArray(modelTrans, numBatchInstances);
		}
		if (modelViewArrayVar.isvalid())
		{
			modelViewArrayVar->SetMatrixArray(modelViewTrans, numBatchInstances);
		}
		if (modelViewProjectionArrayVar.isvalid())
		{
			modelViewProjectionArrayVar->SetMatrixArray(modelViewProjTrans, numBatchInstances);
		}		
        if (objectIdArrayVar.isvalid())
        {
            objectIdArrayVar->SetIntArray(objectIdArray, numBatchInstances);
        }

		// commit shader
		this->shader->Commit();

		// render!
		renderDev->DrawIndexedInstanced(numBatchInstances * multiplier, 0);

        // perform post drawing
        this->shader->PostDraw();

		// decrease transform count
		instances -= this->MaxInstancesPerBatch;

		// offset all arrays
		modelTrans += this->MaxInstancesPerBatch;
		modelViewTrans += this->MaxInstancesPerBatch;
		modelViewProjTrans += this->MaxInstancesPerBatch;
	}
}
} // namespace Instancing
