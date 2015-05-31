//------------------------------------------------------------------------------
//  fbxjointnode.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fbx/node/nfbxjointnode.h"
#include "nfbxscene.h"
#include "math/matrix44.h"
#include "math/float4.h"

using namespace Math;
namespace Fbx
{
__ImplementClass(Fbx::NFbxJointNode, 'FBJN', Fbx::NFbxNode);

//------------------------------------------------------------------------------
/**
*/
NFbxJointNode::NFbxJointNode() : 
	parentIndex(-1),
	cluster(0)
{
	this->type = NFbxNode::Joint;
}

//------------------------------------------------------------------------------
/**
*/
NFbxJointNode::~NFbxJointNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::Setup( FbxNode* node, const Ptr<NFbxScene>& scene, int index )
{
	n_assert(node->GetSkeleton());
	this->joint = node->GetSkeleton();
	this->isSkeletonRoot = this->joint->IsSkeletonRoot();
	this->jointIndex = index;
	NFbxNode::Setup(node, scene);
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::ExtractKeySpan( FbxAnimStack* stack, int& span )
{
	NFbxNode::ExtractKeySpan(stack, span);
	for (int childIndex = 0; childIndex < this->children.Size(); childIndex++)
	{
		Ptr<NFbxNode> child = this->children[childIndex];
		if (child->GetNodeType() == NFbxNode::Joint)
		{
			Ptr<NFbxJointNode> jointNode = child.downcast<NFbxJointNode>();
			jointNode->ExtractKeySpan(stack, span);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::ExtractAnimationCurves( FbxAnimStack* stack, Util::Array<ToolkitUtil::AnimBuilderCurve>& curves, int& postInfType, int& preInfType, int span )
{
	NFbxNode::ExtractAnimationCurves(stack, curves, postInfType, preInfType, span);
	for (int childIndex = 0; childIndex < this->children.Size(); childIndex++)
	{
		Ptr<NFbxNode> child = this->children[childIndex];
		if (child->GetNodeType() == NFbxNode::Joint)
		{
			Ptr<NFbxJointNode> jointNode = child.downcast<NFbxJointNode>();
			jointNode->ExtractAnimationCurves(stack, curves, postInfType, preInfType, span);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::SetupFromCluster(FbxCluster* cluster)
{
	// get local matrix
	FbxAMatrix matrix;
	cluster->GetTransformLinkMatrix(matrix);

	// set cluster
	this->cluster = cluster;

	// create components
	FbxVector4 translation = matrix.GetT();
	FbxVector4 scale = matrix.GetS();
	FbxQuaternion rotation = matrix.GetQ();

	// convert from fbx to nebula math
	this->rotation = quaternion((scalar)rotation[0], (scalar)rotation[1], (scalar)rotation[2], (scalar)rotation[3]);
	this->position = float4((scalar)translation[0], (scalar)translation[1], (scalar)translation[2], (scalar)translation[3]);
	this->scale = float4((scalar)scale[0], (scalar)scale[1], (scalar)scale[2], (scalar)scale[3]);

	// get transform from quaternion
	this->transform = matrix44::rotationquaternion(this->rotation);
	this->transform.set_position(this->position);
	this->transform.scale(this->scale);
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::ConvertJointsToLocal()
{
	n_assert(this->IsRoot());
	this->RecursiveConvertToLocal(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
NFbxJointNode::RecursiveConvertToLocal( const Ptr<NFbxJointNode>& parent )
{
	// only apply if the joint has a cluster connected to it
	if (this->cluster)
	{
		FbxAMatrix thisMatrix;		
		this->cluster->GetTransformLinkMatrix(thisMatrix);

		// if we have a parent, we should multiply this matrix with the inversed matrix of the parent
		if (this != parent)
		{
			FbxAMatrix parentMatrix;
			parent->cluster->GetTransformLinkMatrix(parentMatrix);
			thisMatrix = parentMatrix.Inverse() * thisMatrix;
		}

		// create components
		FbxVector4 translation = thisMatrix.GetT();
		FbxVector4 scale = thisMatrix.GetS();
		FbxQuaternion rotation = thisMatrix.GetQ();

		// convert from fbx to nebula math
		this->rotation = quaternion((scalar)rotation[0], (scalar)rotation[1], (scalar)rotation[2], (scalar)rotation[3]);
		this->position = float4((scalar)translation[0], (scalar)translation[1], (scalar)translation[2], (scalar)translation[3]);
		this->scale = float4((scalar)scale[0], (scalar)scale[1], (scalar)scale[2], (scalar)scale[3]);
	}	

	// go through children and do the same
	IndexT childIndex;
	for (childIndex = 0; childIndex < this->children.Size(); childIndex++)
	{
		Ptr<NFbxNode> child = this->children[childIndex];
		if (child->GetNodeType() == NFbxNode::Joint)
		{
			Ptr<NFbxJointNode> jointNode = child.downcast<NFbxJointNode>();
			jointNode->RecursiveConvertToLocal(this);
		}
	}
}
} // namespace ToolkitUtil