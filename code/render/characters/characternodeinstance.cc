//------------------------------------------------------------------------------
//  characternodeinstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characternodeinstance.h"
#include "characters/characternode.h"
#include "characters/characterserver.h"
#include "models/modelinstance.h"

namespace Characters
{
__ImplementClass(Characters::CharacterNodeInstance, 'CHNI', Models::TransformNodeInstance);

using namespace Math;
using namespace Models;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
CharacterNodeInstance::CharacterNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterNodeInstance::~CharacterNodeInstance()
{
    n_assert(!this->charInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeInstance::Setup(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    n_assert(!this->charInstance.isvalid());

    // up to parent class
    TransformNodeInstance::Setup(inst, node, parentNodeInst);

    // setup a new character instance
    this->charInstance = CharacterInstance::Create();
    const Ptr<CharacterNode>& charNode = node.downcast<CharacterNode>();
    const Ptr<Character> myCharacter = charNode->GetCharacter();
	this->diagonalSize = (int)this->GetModelInstance()->GetModel()->GetBoundingBox().diagonal_size() * 5;
    this->charInstance->Setup(myCharacter, inst);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeInstance::Discard()
{
    n_assert(this->charInstance.isvalid());
    
    // discard our character instance
    this->charInstance->Discard();
    this->charInstance = 0;

    // up to parent class
    TransformNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeInstance::OnNotifyCullingVisible(IndexT frameIndex, Timing::Time time)
{
    // call parent class
    TransformNodeInstance::OnNotifyCullingVisible(frameIndex, time);

    // notify the character server
    if (this->updateThisFrame) CharacterServer::Instance()->GatherVisibleCharacter(this->charInstance, time);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeInstance::RenderDebug()
{
    TransformNodeInstance::RenderDebug();
    this->charInstance->RenderDebug(this->modelTransform);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterNodeInstance::OnVisibilityResolve(IndexT frameIndex, IndexT resolveIndex, float distanceToViewer)
{
	TransformNodeInstance::OnVisibilityResolve(frameIndex, resolveIndex, distanceToViewer);
	
	// use diagonal size to determine if the character should be updated this frame
	int lod = n_max((int)distanceToViewer, this->diagonalSize) / this->diagonalSize;
	this->updateThisFrame = (frameIndex % lod == 0);
}

} // namespace Characters
