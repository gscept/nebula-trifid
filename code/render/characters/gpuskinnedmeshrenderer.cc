//------------------------------------------------------------------------------
//  gpuskinnedmeshrenderer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/gpuskinnedmeshrenderer.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shadersemantics.h"

namespace Characters
{
__ImplementClass(Characters::GPUSkinnedMeshRenderer, 'WSMR', Base::SkinnedMeshRendererBase);
__ImplementSingleton(Characters::GPUSkinnedMeshRenderer);

using namespace Base;
using namespace Math;
using namespace Util;
using namespace Characters;
using namespace CoreGraphics;
using namespace Characters;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
GPUSkinnedMeshRenderer::GPUSkinnedMeshRenderer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GPUSkinnedMeshRenderer::~GPUSkinnedMeshRenderer()
{
    __DestructSingleton;    
}

//------------------------------------------------------------------------------
/**
	Draws mesh fragment
*/
void
GPUSkinnedMeshRenderer::DrawGPUSkinnedMesh(const Ptr<Mesh>& mesh, IndexT primGroupIndex)
{
	// apply mesh data and draw skinned primitives
	//mesh->ApplyPrimitives(primGroupIndex);
	RenderDevice::Instance()->Draw();
}

//------------------------------------------------------------------------------
/**
*/
void
GPUSkinnedMeshRenderer::DrawGPUSkinnedMeshInstanced(const Ptr<CoreGraphics::Mesh>& mesh, IndexT primGroupIndex, SizeT instances)
{
    // apply mesh and draw instances
    mesh->ApplyPrimitives(primGroupIndex);
    RenderDevice::Instance()->DrawIndexedInstanced(instances, 0);
}

//------------------------------------------------------------------------------
/**
	Updates GPU skinned mesh
*/
void 
GPUSkinnedMeshRenderer::UpdateGPUSkinnedJointPalette(const Ptr<Characters::CharacterInstance>& charInst, const Util::Array<IndexT>& jointPalette, const Ptr<CoreGraphics::ShaderVariable>& jointPaletteShdVar)
{
	if (charInst->IsValidForRendering())
	{
		const int maxJointPaletteSize = ConstantBufferMaxJoints;
		matrix44 jointArray[maxJointPaletteSize];

		// make sure the character update has finished
		// charInst->WaitUpdateDone();

		// extract joint palette skin matrices from skeleton
		const FixedArray<matrix44>& skinMatrixArray = charInst->Skeleton().GetSkinMatrixArray();
		IndexT i;
		SizeT numJointsInPalette = jointPalette.Size();
		n_assert(numJointsInPalette <= maxJointPaletteSize);
		for (i = 0; i < numJointsInPalette; i++)
		{
			jointArray[i] = skinMatrixArray[jointPalette[i]];
		}

		// send joints to shader
		jointPaletteShdVar->SetMatrixArray(jointArray, numJointsInPalette);
	}
}

} // namespace Characters