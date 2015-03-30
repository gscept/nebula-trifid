//------------------------------------------------------------------------------
//  shaderslot.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderslot.h"

namespace Tools
{
__ImplementClass(Tools::ShaderSlot,'SHDS', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ShaderSlot::ShaderSlot() :
    slotType(InvalidSlotType),
    slotChannel(InvalidSlotChannel)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderSlot::~ShaderSlot()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Call this method before deconstruction to resolve cyclic 
    dependency between bi-directionally connected slots.
*/
void
ShaderSlot::ClearConnections()
{
    this->connections.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetNodeName(const String& n)
{
    this->nodeName = n;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderSlot::GetNodeName() const
{
    return this->nodeName;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetSlotType(SlotType t)
{
    this->slotType = t;
}

//------------------------------------------------------------------------------
/**
*/
ShaderSlot::SlotType
ShaderSlot::GetSlotType() const
{
    return this->slotType;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetSlotChannel(SlotChannel chn)
{
    this->slotChannel = chn;
}

//------------------------------------------------------------------------------
/**
*/
ShaderSlot::SlotChannel
ShaderSlot::GetSlotChannel() const
{
    return this->slotChannel;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetName(const String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderSlot::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetDataType(const String& t)
{
    this->dataType = t;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderSlot::GetDataType() const
{
    return this->dataType;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::SetSemantics(const String& s)
{
    this->semantics = s;
}

//------------------------------------------------------------------------------
/**
    NOTE: if no semantics have been set yet, this method will set the
    internal semantics string to the slot's name in all upper-case letters.
*/
const String&
ShaderSlot::GetSemantics()
{
    if (!this->semantics.IsValid())
    {
        this->semantics = this->name;
        this->semantics.ToUpper();
    }
    return this->semantics;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderSlot::AddConnection(const Ptr<ShaderSlot>& slot)
{
    this->connections.Append(slot);
}

//------------------------------------------------------------------------------
/**
*/
const Array<Ptr<ShaderSlot>>&
ShaderSlot::GetConnections() const
{
    return this->connections;
}

//------------------------------------------------------------------------------
/**
*/
String
ShaderSlot::SlotTypeAsString(SlotType t)
{
    switch (t)
    {
        case VertexShader:  return "VertexShader";
        case PixelShader:   return "PixelShader";
        case General:       return "General";
    }
    return "InvalidSlotType";
}

//------------------------------------------------------------------------------
/**
*/
String
ShaderSlot::SlotChannelAsString(SlotChannel c)
{
    switch (c)
    {
        case Input: return "Input";
        case Output: return "Output";
    }
    return "InvalidSlotChannel";
}

} // namespace Tools
