//------------------------------------------------------------------------------
//  shadernode.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadernode.h"
#include "shaderfragmentmanager.h"

namespace Tools
{
__ImplementClass(Tools::ShaderNode, 'SHDN', Core::RefCounted);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderNode::ShaderNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderNode::~ShaderNode()
{
    this->ClearSlots();
}

//------------------------------------------------------------------------------
/**
    Correctly cleans up the input/output slots.
*/
void
ShaderNode::ClearSlots()
{
    // cleanup bi-directional dependencies between shader slots
    IndexT i;
    for (i = 0; i < this->inputSlots.Size(); i++)
    {
        this->inputSlots.ValueAtIndex(i)->ClearConnections();
    }
    for (i = 0; i < this->outputSlots.Size(); i++)
    {
        this->outputSlots.ValueAtIndex(i)->ClearConnections();
    }
    this->inputSlots.Clear();
    this->outputSlots.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void __cdecl
ShaderNode::SetError(const char* fmt, ...)
{
    String fmtString;
    va_list argList;
    va_start(argList, fmt);
    fmtString.FormatArgList(fmt, argList);
    va_end(argList);

    this->error.Format("ERROR (shader node %s): %s!\n", this->GetName().AsCharPtr(), fmtString.AsCharPtr());
    n_printf(this->error.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderNode::GetError() const
{
    return this->error;
}

//------------------------------------------------------------------------------
/**
    This adds input and output slots to the shader node, defined 
    by the provided shader fragment.
*/
void
ShaderNode::SetupSlotsFromFragment()
{
    this->ClearSlots();
    this->inputSlots.Clear();
    this->outputSlots.Clear();
    if (this->fragmentName.IsValid())
    {
        ShaderFragmentManager* fragManager = ShaderFragmentManager::Instance();
        n_assert(fragManager->HasFragment(this->fragmentName));
        const Ptr<ShaderFragment>& fragment = fragManager->GetFragment(this->fragmentName);

        // add vertex shader input slots
        Array<ShaderParam> vertexInputs = fragment->GetVertexShaderInputs().ValuesAsArray();
        IndexT i;
        for (i = 0; i < vertexInputs.Size(); i++)
        {
            Ptr<ShaderSlot> slot = ShaderSlot::Create();
            slot->SetNodeName(this->GetName());
            slot->SetSlotType(ShaderSlot::VertexShader);
            slot->SetSlotChannel(ShaderSlot::Input);
            slot->SetName(vertexInputs[i].GetName());
            slot->SetDataType(vertexInputs[i].GetType());
            n_assert(!this->inputSlots.Contains(slot->GetName()));
            this->AddInputSlot(slot);
        }

        // add pixel shader input slots
        Array<ShaderParam> pixelInputs = fragment->GetPixelShaderInputs().ValuesAsArray();
        for (i = 0; i < pixelInputs.Size(); i++)
        {
            Ptr<ShaderSlot> slot = ShaderSlot::Create();
            slot->SetNodeName(this->GetName());
            slot->SetSlotType(ShaderSlot::PixelShader);
            slot->SetSlotChannel(ShaderSlot::Input);
            slot->SetName(pixelInputs[i].GetName());
            slot->SetDataType(pixelInputs[i].GetType());
            n_assert(!this->inputSlots.Contains(slot->GetName()));
            this->AddInputSlot(slot);
        }

        // add vertex shader output slots
        Array<ShaderParam> vertexOutputs = fragment->GetVertexShaderOutputs().ValuesAsArray();
        for (i = 0; i < vertexOutputs.Size(); i++)
        {
            Ptr<ShaderSlot> slot = ShaderSlot::Create();
            slot->SetNodeName(this->GetName());
            slot->SetSlotType(ShaderSlot::VertexShader);
            slot->SetSlotChannel(ShaderSlot::Output);
            slot->SetName(vertexOutputs[i].GetName());
            slot->SetDataType(vertexOutputs[i].GetType());
            n_assert(!this->outputSlots.Contains(slot->GetName()));
            this->AddOutputSlot(slot);
        }

        // add pixel shader output slots
        Array<ShaderParam> pixelOutputs = fragment->GetPixelShaderOutputs().ValuesAsArray();
        for (i = 0; i < pixelOutputs.Size(); i++)
        {
            Ptr<ShaderSlot> slot = ShaderSlot::Create();
            slot->SetNodeName(this->GetName());
            slot->SetSlotType(ShaderSlot::PixelShader);
            slot->SetSlotChannel(ShaderSlot::Output);
            slot->SetName(pixelOutputs[i].GetName());
            slot->SetDataType(pixelOutputs[i].GetType());
            n_assert(!this->outputSlots.Contains(slot->GetName()));
            this->AddOutputSlot(slot);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderNode::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Node");

    n_assert(xmlReader->HasAttr("name"));
    n_assert(xmlReader->HasAttr("fragment"));
    this->name = xmlReader->GetString("name");
    this->fragmentName = xmlReader->GetString("fragment");

    // get the shader fragment from the fragment manager
    ShaderFragmentManager* fragManager = ShaderFragmentManager::Instance();
    if (!fragManager->HasFragment(this->fragmentName) &&
        !fragManager->HasFragmentGroup(this->fragmentName))
    {
        this->SetError("Invalid fragment or fragment group name: %s", this->fragmentName.AsCharPtr());
        return false;
    }
    
    // parse shader bindings (defined inside node)
    if (xmlReader->SetToFirstChild("Bind")) do
    {
        Ptr<ShaderBinding> binding = ShaderBinding::Create();
        binding->Parse(xmlReader);
        this->bindings.Append(binding);
    }
    while (xmlReader->SetToNextChild("Bind"));
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderNode::AddInputSlot(const Ptr<ShaderSlot>& slot)
{
    n_assert(!this->inputSlots.Contains(slot->GetName()));
    n_assert(slot->GetNodeName() == this->GetName());
    this->inputSlots.Add(slot->GetName(), slot);
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderNode::AddOutputSlot(const Ptr<ShaderSlot>& slot)
{
    n_assert(!this->outputSlots.Contains(slot->GetName()));
    n_assert(slot->GetNodeName() == this->GetName());
    this->outputSlots.Add(slot->GetName(), slot);
}

//------------------------------------------------------------------------------
/**
*/
const Dictionary<String, Ptr<ShaderSlot>>&
ShaderNode::GetInputSlots() const
{
    return this->inputSlots;
}

//------------------------------------------------------------------------------
/**
*/
const Dictionary<String, Ptr<ShaderSlot>>&
ShaderNode::GetOutputSlots() const
{
    return this->outputSlots;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderNode::SetName(const String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderNode::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderNode::GetFragmentName() const
{
    return this->fragmentName;
}

//------------------------------------------------------------------------------
/**
*/
const Array<Ptr<ShaderBinding>>& 
ShaderNode::GetManualBindings() const
{
    return this->bindings;
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ShaderNode::CountSlotsByTypeAndChannel(ShaderSlot::SlotType slotType, ShaderSlot::SlotChannel slotChannel) const
{
    SizeT num = 0;
    IndexT i;
    for (i = 0; i < this->inputSlots.Size(); i++)
    {
        const Ptr<ShaderSlot>& slot = this->inputSlots.ValueAtIndex(i);
        if ((slot->GetSlotType() == slotType) && (slot->GetSlotChannel() == slotChannel))
        {
            num++;
        }
    }
    for (i = 0; i < this->outputSlots.Size(); i++)
    {
        const Ptr<ShaderSlot>& slot = this->outputSlots.ValueAtIndex(i);
        if ((slot->GetSlotType() == slotType) && (slot->GetSlotChannel() == slotChannel))
        {
            num++;
        }
    }
    return num;
}

} // namespace Tools