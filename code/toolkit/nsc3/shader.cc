//------------------------------------------------------------------------------
//  shader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shader.h"
#include "shadernode.h"
#include "shaderfragmentmanager.h"

namespace Tools
{
__ImplementClass(Tools::Shader, 'SHDR', Core::RefCounted);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
Shader::Shader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Shader::~Shader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::SetFileUri(const URI& uri)
{
    this->fileUri = uri;
}

//------------------------------------------------------------------------------
/**
*/
const URI&
Shader::GetFileUri() const
{
    return this->fileUri;
}

//------------------------------------------------------------------------------
/**
*/
void __cdecl
Shader::SetError(const char* fmt, ...)
{
    String fmtString;
    va_list argList;
    va_start(argList, fmt);
    fmtString.FormatArgList(fmt, argList);
    va_end(argList);

    this->error.Format("ERROR in shader '%s' (%s): %s!\n", 
        this->GetName().AsCharPtr(),
        this->GetFileUri().AsString().AsCharPtr(),
        fmtString.AsCharPtr());

    n_printf(this->error.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
const String&
Shader::GetError() const
{
    return this->error;
}

//------------------------------------------------------------------------------
/**
    Clear the default nodes (Vertex, Interpolator, etc...).
*/
void
Shader::ClearDefaultNodes()
{
    Array<String> nodeNames;
    nodeNames.Append("Vertex");
    nodeNames.Append("Interpolator");
    nodeNames.Append("Sampler");
    nodeNames.Append("Constant");
    nodeNames.Append("Shared");
    nodeNames.Append("Result");
    IndexT i;
    for (i = 0; i < nodeNames.Size(); i++)
    {
        if (this->shaderNodes.Contains(nodeNames[i]))
        {
            this->shaderNodes.Erase(nodeNames[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This setups default shader nodes which serve as end points for
    open shader bindings (these node represent the global and shared
    parameters, vertex declarations, pixel shader results, etc...
*/
void
Shader::SetupDefaultNodes()
{
    Array<String> nodeNames;
    nodeNames.Append("Vertex");
    nodeNames.Append("Interpolator");
    nodeNames.Append("Sampler");
    nodeNames.Append("Constant");
    nodeNames.Append("Shared");
    nodeNames.Append("Result");
    
    IndexT i;
    for (i = 0; i < nodeNames.Size(); i++)
    {
        Ptr<ShaderNode> shaderNode = ShaderNode::Create();
        shaderNode->SetName(nodeNames[i]);
        n_assert(!this->shaderNodes.Contains(nodeNames[i]));
        this->shaderNodes.Add(shaderNode->GetName(), shaderNode);
    }
}
 
//------------------------------------------------------------------------------
/**
*/
bool
Shader::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Shader");

    n_assert(xmlReader->HasAttr("name"));
    this->name = xmlReader->GetString("name");

    // parse shader nodes...
    if (xmlReader->SetToFirstChild("Node")) do
    {
        // parse shader node attributes...
        Ptr<ShaderNode> newNode = ShaderNode::Create();
        if (newNode->Parse(xmlReader))
        {
            this->shaderNodes.Add(newNode->GetName(), newNode);
        }
        else
        {
            return false;
        }
    }
    while (xmlReader->SetToNextChild("Node"));
    return true;
}

//------------------------------------------------------------------------------
/**
    Setup a new slot connection from an input slot to an output slot.
*/
bool
Shader::SetupUplinkSlotConnection(const Ptr<ShaderNode>& shaderNode,
                                  const String& paramStr,
                                  const String& bindNode,
                                  const String& bindParam)
{
    // find the target shader node
    if (this->shaderNodes.Contains(bindNode))
    {
        const Ptr<ShaderNode> destNode = this->shaderNodes[bindNode];
        if (shaderNode->GetInputSlots().Contains(paramStr))
        {
            // connect input slot to an output slot
            const Ptr<ShaderSlot>& inputSlot = shaderNode->GetInputSlots()[paramStr];
            if (destNode->GetOutputSlots().Contains(bindParam))
            {
                const Ptr<ShaderSlot>& outputSlot = destNode->GetOutputSlots()[bindParam];
                n_assert((outputSlot->GetSlotType() == inputSlot->GetSlotType()) ||
                         (outputSlot->GetSlotType() == ShaderSlot::General) ||
                         (inputSlot->GetSlotType() == ShaderSlot::General));
                n_assert(outputSlot->GetDataType() == inputSlot->GetDataType());
                inputSlot->AddConnection(outputSlot);
                outputSlot->AddConnection(inputSlot);
            }
            else
            {
                // if the output slot doesn't exist, this MUST be one of the special
                // nodes (Vertex, Pixel, Globals, Shared, etc...) these special nodes
                // don't have a fragment set...
                if (destNode->GetFragmentName().IsEmpty())
                {
                    // add output slot to special node...
                    Ptr<ShaderSlot> newSlot = ShaderSlot::Create();
                    newSlot->SetNodeName(destNode->GetName());
                    newSlot->SetSlotType(ShaderSlot::General);
                    newSlot->SetSlotChannel(ShaderSlot::Output);
                    newSlot->SetName(bindParam);                    
                    newSlot->SetDataType(inputSlot->GetDataType());
                    destNode->AddOutputSlot(newSlot);
                    inputSlot->AddConnection(newSlot);
                    newSlot->AddConnection(inputSlot);
                }
                else
                {
                    // an error: invalid bind param name...
                    this->SetError("Binding error: target shader node '%s' doesn't have output parameter '%s' (referenced by node '%s')\n",
                        destNode->GetName().AsCharPtr(), bindParam.AsCharPtr(), shaderNode->GetName().AsCharPtr());
                    return false;
                }
            }
        }
        else
        {
            // error... non-existing input parameter
            this->SetError("Binding error: shader node '%s' doesn't have input parameter '%s'!\n",
                shaderNode->GetName().AsCharPtr(), paramStr.AsCharPtr());
            return false;
        }
    }
    else
    {
        this->SetError("Destination shader node '%s' not defined in shader!\n", bindNode.AsCharPtr());
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Setup a new slot connection from an output slot to an input slot.
    NOTE: there may only be one downlink connection per input slot.
*/
bool
Shader::SetupDownlinkSlotConnection(const Ptr<ShaderNode>& shaderNode,
                                    const String& paramStr,
                                    const String& bindNode,
                                    const String& bindParam)
{
    // find the target shader node
    if (this->shaderNodes.Contains(bindNode))
    {
        const Ptr<ShaderNode> destNode = this->shaderNodes[bindNode];
        if (shaderNode->GetOutputSlots().Contains(paramStr))
        {
            // connect output slot to an input slot
            const Ptr<ShaderSlot>& outputSlot = shaderNode->GetOutputSlots()[paramStr];
            if (destNode->GetInputSlots().Contains(bindParam))
            {
                const Ptr<ShaderSlot>& inputSlot = destNode->GetInputSlots()[bindParam];
                n_assert((outputSlot->GetSlotType() == inputSlot->GetSlotType()) ||
                         (outputSlot->GetSlotType() == ShaderSlot::General) ||
                         (inputSlot->GetSlotType() == ShaderSlot::General));
                n_assert(outputSlot->GetDataType() == inputSlot->GetDataType());
                inputSlot->AddConnection(outputSlot);
                outputSlot->AddConnection(inputSlot);
            }
            else
            {
                // if the output slot doesn't exist, this MUST be one of the special
                // nodes (Vertex, Pixel, Globals, Shared, etc...) these special nodes
                // don't have a fragment set...
                if (destNode->GetFragmentName().IsEmpty())
                {
                    // add output slot to special node...
                    Ptr<ShaderSlot> newSlot = ShaderSlot::Create();
                    newSlot->SetNodeName(destNode->GetName());
                    newSlot->SetSlotType(ShaderSlot::General);
                    newSlot->SetSlotChannel(ShaderSlot::Input);
                    newSlot->SetName(bindParam);
                    newSlot->SetDataType(outputSlot->GetDataType());
                    destNode->AddInputSlot(newSlot);
                    newSlot->AddConnection(outputSlot);
                    outputSlot->AddConnection(newSlot);
                }
                else
                {
                    // an error: invalid bind param name...
                    this->SetError("Binding error: target shader node '%s' doesn't have input parameter '%s' (referenced by node '%s')\n",
                        destNode->GetName().AsCharPtr(), bindParam.AsCharPtr(), shaderNode->GetName().AsCharPtr());
                    return false;
                }
            }
        }
        else
        {
            // error... non-existing output parameter
            this->SetError("Binding error: shader node '%s' doesn't have output parameter '%s'!\n",
                shaderNode->GetName().AsCharPtr(), paramStr.AsCharPtr());
            return false;
        }
    }
    else
    {
        this->SetError("Destination shader node '%s' not defined in shader!\n", bindNode.AsCharPtr());
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This goes through the shader nodes and setsup their manual bindings
    (connecting input slots of nodes with output slots of other nodes).
*/
bool
Shader::SetupManualBindings()
{
    // for each node...
    IndexT nodeIndex;
    for (nodeIndex = 0; nodeIndex < this->shaderNodes.Size(); nodeIndex++)
    {
        const Ptr<ShaderNode>& shaderNode = this->shaderNodes.ValueAtIndex(nodeIndex);

        // for each binding...
        IndexT bindIndex;
        for (bindIndex = 0; bindIndex < shaderNode->GetManualBindings().Size(); bindIndex++)
        {
            const Ptr<ShaderBinding>& binding = shaderNode->GetManualBindings()[bindIndex];
            const String& paramStr = binding->GetParam();
            const String& bindNode = binding->GetBindNode();
            const String& bindParam = binding->GetBindParam();
            if (!this->SetupUplinkSlotConnection(shaderNode, paramStr, bindNode, bindParam))
            {
                return false;
            }

        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Connects a slot according to its default binding. This method
    is called by SetupDefaultBindings()
*/
bool
Shader::SetupDefaultSlotConnection(const Ptr<ShaderNode>& shaderNode, const Ptr<ShaderSlot>& shaderSlot)
{
    n_assert(shaderSlot->GetSlotChannel() != ShaderSlot::InvalidSlotChannel);

    // get the slot's default binding from the shader fragment
    n_assert(shaderNode->GetFragmentName().IsValid());
    ShaderFragmentManager* fragManager = ShaderFragmentManager::Instance();
    const Ptr<ShaderFragment>& fragment = fragManager->GetFragment(shaderNode->GetFragmentName());
    String defBinding;
    bool isUplink;
    if (shaderSlot->GetSlotType() == ShaderSlot::VertexShader)
    {
        if (shaderSlot->GetSlotChannel() == ShaderSlot::Input)
        {
            n_assert(fragment->GetVertexShaderInputs().Contains(shaderSlot->GetName()));
            defBinding = fragment->GetVertexShaderInputs()[shaderSlot->GetName()].GetDefaultBinding();
            isUplink = true;
        }
        else
        {
            n_assert(fragment->GetVertexShaderOutputs().Contains(shaderSlot->GetName()));
            defBinding = fragment->GetVertexShaderOutputs()[shaderSlot->GetName()].GetDefaultBinding();
            isUplink = false;
        }
    }
    else
    {
        if (shaderSlot->GetSlotChannel() == ShaderSlot::Input)
        {
            n_assert(fragment->GetPixelShaderInputs().Contains(shaderSlot->GetName()));
            defBinding = fragment->GetPixelShaderInputs()[shaderSlot->GetName()].GetDefaultBinding();
            isUplink = true;
        }
        else
        {
            n_assert(fragment->GetPixelShaderOutputs().Contains(shaderSlot->GetName()));
            defBinding = fragment->GetPixelShaderOutputs()[shaderSlot->GetName()].GetDefaultBinding();
            isUplink = false;
        }
    }

    // create the new slot connection
    if (defBinding.IsValid())
    {
        const String& paramStr = shaderSlot->GetName();
        Array<String> bindTokens = defBinding.Tokenize(".");
        n_assert(bindTokens.Size() == 2);
        const String& bindNode = bindTokens[0];
        const String& bindParam = bindTokens[1];
        if (isUplink)
        {
            if (!this->SetupUplinkSlotConnection(shaderNode, paramStr, bindNode, bindParam))
            {
                return false;
            }
        }
        else
        {
            if (!this->SetupDownlinkSlotConnection(shaderNode, paramStr, bindNode, bindParam))
            {
                return false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This is executed after SetupManualBindings() and will connect all
    remaining "open" slots to the slots defined in the default bindings.
    After this method, no unconnected slots should exist.
*/
bool
Shader::SetupDefaultBindings()
{
    // first go through the input slots and setup missing connections
    IndexT nodeIndex;
    for (nodeIndex = 0; nodeIndex < this->shaderNodes.Size(); nodeIndex++)
    {
        const Ptr<ShaderNode>& shaderNode = this->shaderNodes.ValueAtIndex(nodeIndex);
        IndexT slotIndex;
        for (slotIndex = 0; slotIndex < shaderNode->GetInputSlots().Size(); slotIndex++)
        {
            const Ptr<ShaderSlot>& slot = shaderNode->GetInputSlots().ValueAtIndex(slotIndex);
            if (slot->GetConnections().Size() == 0)
            {
                this->SetupDefaultSlotConnection(shaderNode, slot);
            }
        }
    }

    // then go through the remaining unconnected output slots
    for (nodeIndex = 0; nodeIndex < this->shaderNodes.Size(); nodeIndex++)
    {
        const Ptr<ShaderNode>& shaderNode = this->shaderNodes.ValueAtIndex(nodeIndex);
        IndexT slotIndex;
        for (slotIndex = 0; slotIndex < shaderNode->GetOutputSlots().Size(); slotIndex++)
        {
            const Ptr<ShaderSlot>& slot = shaderNode->GetOutputSlots().ValueAtIndex(slotIndex);
            if (slot->GetConnections().Size() == 0)
            {
                this->SetupDefaultSlotConnection(shaderNode, slot);
            }
        }
    }

    // fix the interpolator
    if (!this->SetupInterpolatorPassThroughConnections())
    {
        this->SetError("Failed to setup interpolator passthru connections!\n");
        return false;
    }

    // setup interpolator semantics
    if (!this->SetupInterpolatorSemantics())
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method fixes up remaining connections on the Interpolator node
    (sitting between the pixel shader and vertex shader). Output slots
    on the interpolator node which don't have a corresponding input slot
    will have an input slot added, and connected to the Vertex shader node,
    which holds the vertex declaration. This is the case when the
    pixel shader requires an input value which isn't provided by vertex
    shader code, thus the value is directly handed from the vertex to
    the interpolator without modification by the vertex shader.
*/
bool
Shader::SetupInterpolatorPassThroughConnections()
{
    // get the Interpolator and Vertex shader node
    const Ptr<ShaderNode>& interpNode = this->shaderNodes["Interpolator"];

    // for each interpolator slot which doesn't have a matching input slot...
    IndexT slotIndex;
    for (slotIndex = 0; slotIndex < interpNode->GetOutputSlots().Size(); slotIndex++)
    {
        const Ptr<ShaderSlot>& slot = interpNode->GetOutputSlots().ValueAtIndex(slotIndex);
        if (!interpNode->GetInputSlots().Contains(slot->GetName()))
        {
            // create and configure a new input slot
            Ptr<ShaderSlot> newSlot = ShaderSlot::Create();
            newSlot->SetNodeName(slot->GetNodeName());
            newSlot->SetSlotType(ShaderSlot::General);
            newSlot->SetName(slot->GetName());
            newSlot->SetDataType(slot->GetDataType());
            newSlot->SetSlotChannel(ShaderSlot::Input);
            interpNode->AddInputSlot(newSlot);

            // connect the new slot to a corresponding vertex output slot,
            // create new one if none exists yet
            bool slotConnected = this->SetupUplinkSlotConnection(interpNode, slot->GetName(), "Vertex", slot->GetName());
            n_assert(slotConnected);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method goes through the input nodes of the Interpolator node
    and setsup its semantics. Semantics will be automatically assigned, except
    for the special slots "Position", "Fog", "PSize". This method
    must be called after SetupInterpolatorPassThroughConnections(), otherwise
    input slots may be missing from the Interpolator shader node. If the
    method is running out of valid interpolator slots, the method will
    return false.
*/
bool
Shader::SetupInterpolatorSemantics()
{
    // setup an array with auto-semantics (note: we assume
    // that COLOR0 and COLOR1 will not be clipped to 0..1, thus
    // this will only work correctly for shader model 0
    IndexT autoSemIndex = 0;
    Array<String> autoSemantics;
    autoSemantics.Append("TEXCOORD0");
    autoSemantics.Append("TEXCOORD1");
    autoSemantics.Append("TEXCOORD2");
    autoSemantics.Append("TEXCOORD3");
    autoSemantics.Append("TEXCOORD4");
    autoSemantics.Append("TEXCOORD5");
    autoSemantics.Append("TEXCOORD6");
    autoSemantics.Append("TEXCOORD7");
    autoSemantics.Append("COLOR0");
    autoSemantics.Append("COLOR1");

    // for each input slot...
    const Ptr<ShaderNode>& interpNode = this->shaderNodes["Interpolator"];
    IndexT slotIndex;
    for (slotIndex = 0; slotIndex < interpNode->GetInputSlots().Size(); slotIndex++)
    {
        const Ptr<ShaderSlot>& slot = interpNode->GetInputSlots().ValueAtIndex(slotIndex);
        if (slot->GetName() == "Position")
        {
            slot->SetSemantics("POSITION");
        }
        else if (slot->GetName() == "Fog")
        {
            slot->SetSemantics("FOG");
        }
        else if (slot->GetName() == "PSize")
        {
            slot->SetSemantics("PSIZE");
        }
        else
        {
            if (autoSemIndex >= autoSemantics.Size())
            {
                this->SetError("Too many interpolator slots used (between vertex and pixel shader)!\n");
                return false;
            }
            slot->SetSemantics(autoSemantics[autoSemIndex++]);
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
const String&
Shader::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
const Dictionary<String, Ptr<ShaderNode>>&
Shader::GetShaderNodes() const
{
    return this->shaderNodes;
}

//------------------------------------------------------------------------------
/**
    Print the shader structure for debugging.
*/
void
Shader::DebugDumpShaderNodes(const Ptr<IO::TextWriter>& textWriter)
{
    IndexT nodeIndex;
    for (nodeIndex = 0; nodeIndex < this->shaderNodes.Size(); nodeIndex++)
    {
        const Ptr<ShaderNode>& shaderNode = this->shaderNodes.ValueAtIndex(nodeIndex);
        textWriter->WriteFormatted("    node %s\n", shaderNode->GetName().AsCharPtr());
        textWriter->WriteFormatted("    {\n");
        if (shaderNode->GetFragmentName().IsValid())
        {
            textWriter->WriteFormatted("        fragment = %s\n", shaderNode->GetFragmentName().AsCharPtr());
        }

        IndexT slotTypeIndex;
        for (slotTypeIndex = 0; slotTypeIndex < ShaderSlot::NumSlotTypes; slotTypeIndex++)
        {
            // input slots
            ShaderSlot::SlotType slotType = (ShaderSlot::SlotType) slotTypeIndex;
            if (shaderNode->CountSlotsByTypeAndChannel(slotType, ShaderSlot::Input))
            {
                textWriter->WriteFormatted("        %s inputs\n", ShaderSlot::SlotTypeAsString(slotType).AsCharPtr());
                textWriter->WriteFormatted("        {\n");
                IndexT slotIndex;
                for (slotIndex = 0; slotIndex < shaderNode->GetInputSlots().Size(); slotIndex++)
                {
                    const Ptr<ShaderSlot>& shaderSlot = shaderNode->GetInputSlots().ValueAtIndex(slotIndex);
                    if (shaderSlot->GetSlotType() == slotType)
                    {
                        textWriter->WriteFormatted("            %s %s\n", shaderSlot->GetDataType().AsCharPtr(), shaderSlot->GetName().AsCharPtr());
                        IndexT connIndex;
                        for (connIndex = 0; connIndex < shaderSlot->GetConnections().Size(); connIndex++)
                        {
                            const Ptr<ShaderSlot>& connSlot = shaderSlot->GetConnections()[connIndex];
                            textWriter->WriteFormatted("                -> %s %s.%s (chn=%s, type=%s)\n",
                                connSlot->GetDataType().AsCharPtr(),
                                connSlot->GetNodeName().AsCharPtr(),
                                connSlot->GetName().AsCharPtr(),
                                ShaderSlot::SlotChannelAsString(connSlot->GetSlotChannel()).AsCharPtr(),
                                ShaderSlot::SlotTypeAsString(connSlot->GetSlotType()).AsCharPtr());
                        }
                    }
                }
                textWriter->WriteFormatted("        }\n");
            }

            // output slots
            if (shaderNode->CountSlotsByTypeAndChannel(slotType, ShaderSlot::Output))
            {
                textWriter->WriteFormatted("        %s outputs\n", ShaderSlot::SlotTypeAsString(slotType).AsCharPtr());
                textWriter->WriteFormatted("        {\n");
                IndexT slotIndex;
                for (slotIndex = 0; slotIndex < shaderNode->GetOutputSlots().Size(); slotIndex++)
                {
                    const Ptr<ShaderSlot>& shaderSlot = shaderNode->GetOutputSlots().ValueAtIndex(slotIndex);
                    if (shaderSlot->GetSlotType() == slotType)
                    {
                        textWriter->WriteFormatted("            %s %s\n", shaderSlot->GetDataType().AsCharPtr(), shaderSlot->GetName().AsCharPtr());
                        IndexT connIndex;
                        for (connIndex = 0; connIndex < shaderSlot->GetConnections().Size(); connIndex++)
                        {
                            const Ptr<ShaderSlot>& connSlot = shaderSlot->GetConnections()[connIndex];
                            textWriter->WriteFormatted("                -> %s %s.%s (chn=%s, type=%s)\n",
                                connSlot->GetDataType().AsCharPtr(),
                                connSlot->GetNodeName().AsCharPtr(),
                                connSlot->GetName().AsCharPtr(),
                                ShaderSlot::SlotChannelAsString(connSlot->GetSlotChannel()).AsCharPtr(),
                                ShaderSlot::SlotTypeAsString(connSlot->GetSlotType()).AsCharPtr());
                        }
                    }
                }
                textWriter->WriteFormatted("        }\n");
            }
        }
        textWriter->WriteFormatted("    }\n");
    }
}

//------------------------------------------------------------------------------
/**
    This returns an array of all unique fragment pointers used by this shader.
*/
Array<Ptr<ShaderFragment>>
Shader::GatherShaderFragments() const
{
    Dictionary<String, Ptr<ShaderFragment>> fragments;
    IndexT nodeIndex;
    for (nodeIndex = 0; nodeIndex < this->shaderNodes.Size(); nodeIndex++)
    {
        const Ptr<ShaderNode>& shaderNode = this->shaderNodes.ValueAtIndex(nodeIndex);
        const String& fragName = shaderNode->GetFragmentName();
        if (fragName.IsValid())
        {
            // don't add fragments twice
            if (!fragments.Contains(fragName))
            {
                fragments.Add(fragName, ShaderFragmentManager::Instance()->GetFragment(fragName));
            }
        }
    }
    return fragments.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
    This takes a start shader nodes, and returns an array of uplink
    dependency nodes. The method will recurse until a special
    node is hit.
*/
void
Shader::GetUplinkDependencyNodes(const String& startNode, 
                                 ShaderSlot::SlotType slotType, 
                                 Array<Ptr<ShaderNode>>& inOutNodes) const
{
    const Ptr<ShaderNode>& node = this->shaderNodes[startNode];

    // for each input slot...
    const Dictionary<String, Ptr<ShaderSlot>>& inputSlots = node->GetInputSlots();
    IndexT i;
    for (i = 0; i < inputSlots.Size(); i++)
    {
        const Ptr<ShaderSlot>& slot = inputSlots.ValueAtIndex(i);
        if (slot->GetConnections().Size() > 0)
        {
            n_assert(slot->GetConnections().Size() == 1);
            const Ptr<ShaderSlot>& uplinkSlot = slot->GetConnections()[0];
            if (uplinkSlot->GetSlotType() == slotType)
            {
                const Ptr<ShaderNode>& uplinkNode = this->shaderNodes[uplinkSlot->GetNodeName()];
                if (uplinkNode->GetFragmentName().IsValid())
                {
                    if (InvalidIndex == inOutNodes.FindIndex(uplinkNode))
                    {
                        // add new shader node and recurse
                        inOutNodes.Insert(0, uplinkNode);
                        this->GetUplinkDependencyNodes(uplinkNode->GetName(), slotType, inOutNodes);
                    }
                }
            }
        }
        else
        {
            n_printf("WARNING: input slot '%s.%s' is not connected!\n", startNode.AsCharPtr(), slot->GetName().AsCharPtr());
        }
    }
}

//------------------------------------------------------------------------------
/**
    This resets the contained shader nodes for re-compilation.
*/
void
Shader::ResetNodes()
{
    this->ClearDefaultNodes();
    this->SetupDefaultNodes();
    IndexT i;
    for (i = 0; i < this->shaderNodes.Size(); i++)
    {
        this->shaderNodes.ValueAtIndex(i)->SetupSlotsFromFragment();
    }
}

} // namespace Tools
