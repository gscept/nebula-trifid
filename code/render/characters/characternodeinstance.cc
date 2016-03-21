//------------------------------------------------------------------------------
//  characternodeinstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characternodeinstance.h"
#include "characters/characternode.h"
#include "characters/characterserver.h"

namespace Characters
{
__ImplementClass(Characters::CharacterNodeInstance, 'CHNI', Models::TransformNodeInstance);

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
    CharacterServer::Instance()->GatherVisibleCharacter(this->charInstance, time);
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

} // namespace Characters
