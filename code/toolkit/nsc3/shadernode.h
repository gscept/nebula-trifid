#pragma once
#ifndef TOOLS_SHADERNODE_H
#define TOOLS_SHADERNODE_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderNode
    
    A shader node associates a shader fragment with shader parameter bindings
    to other shader nodes. Shader nodes are organized into a shader tree.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/xmlreader.h"
#include "shaderbinding.h"
#include "shaderslot.h"
#include "shaderfragment.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderNode : public Core::RefCounted
{
    __DeclareClass(ShaderNode);
public:
    /// constructor
    ShaderNode();
    /// destructor
    virtual ~ShaderNode();

    /// parse the fragment from an open XmlReader
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);
    /// get error string
    const Util::String& GetError() const;
    /// setup slots from shader fragment
    void SetupSlotsFromFragment();

    /// set the name of the shader node
    void SetName(const Util::String& n);
    /// get the name of the shader node
    const Util::String& GetName() const;
    /// get the fragment name of the shader node
    const Util::String& GetFragmentName() const;
    /// add a shader input slot
    void AddInputSlot(const Ptr<ShaderSlot>& slot);
    /// get shader input slots
    const Util::Dictionary<Util::String, Ptr<ShaderSlot>>& GetInputSlots() const;
    /// add a shader output slot
    void AddOutputSlot(const Ptr<ShaderSlot>& slot);
    /// get shader output slot
    const Util::Dictionary<Util::String, Ptr<ShaderSlot>>& GetOutputSlots() const;
    /// get manually defined bindings
    const Util::Array<Ptr<ShaderBinding>>& GetManualBindings() const;
    /// count shader slots by slot channel and slot type
    SizeT CountSlotsByTypeAndChannel(ShaderSlot::SlotType slotType, ShaderSlot::SlotChannel slotChannel) const;

private:
    /// set error string
    void __cdecl SetError(const char* fmt, ...);
    /// correctly cleanup slot arrays
    void ClearSlots();

    Util::String error;
    Util::String name;
    Util::String fragmentName;
    Util::Dictionary<Util::String, Ptr<ShaderSlot>> inputSlots;
    Util::Dictionary<Util::String, Ptr<ShaderSlot>> outputSlots;
    Util::Array<Ptr<ShaderBinding>> bindings;
};

} // namespace ShaderNode
//------------------------------------------------------------------------------
#endif

    