//------------------------------------------------------------------------------
//  fbxtransformnode.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fbx/node/nfbxtransformnode.h"
#include "nfbxnode.h"
#include "nfbxscene.h"

namespace Fbx
{
__ImplementClass(Fbx::NFbxTransformNode, 'FBTN', Fbx::NFbxNode);

//------------------------------------------------------------------------------
/**
*/
NFbxTransformNode::NFbxTransformNode()
{
	this->type = NFbxNode::Transform;
}

//------------------------------------------------------------------------------
/**
*/
NFbxTransformNode::~NFbxTransformNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxTransformNode::Setup( FbxNode* node, const Ptr<NFbxScene>& scene )
{
	NFbxNode::Setup(node, scene);
	n_assert(node->GetNull());
	this->fbxTransform = node->GetNull();
}

//------------------------------------------------------------------------------
/**
	Quite simply remove this node, EvaulateLocalTransform should handle parented matrices
*/
void 
NFbxTransformNode::DoMerge( Util::Dictionary<Util::String, Util::Array<Ptr<NFbxMeshNode> > >& meshes )
{
	NFbxNode::DoMerge(meshes);

	// copy pointer on purpose
	Ptr<NFbxNode> parent = this->GetParent();
}

} // namespace ToolkitUtil