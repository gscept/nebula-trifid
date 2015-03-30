#pragma once
#ifndef TOOLS_SHADERSLOT_H
#define TOOLS_SHADERSLOT_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderSlot
    
    An input or output slot on a shader node which can be connected
    to other slots.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderSlot : public Core::RefCounted
{
    __DeclareClass(ShaderSlot);
public:
    /// slot types
    enum SlotType
    {
        VertexShader,
        PixelShader,
        General,

        NumSlotTypes,
        InvalidSlotType,
    };

    /// input/output channel enum
    enum SlotChannel
    {
        Input,
        Output,

        NumSlotChannels,
        InvalidSlotChannel,
    };

    /// constructor
    ShaderSlot();
    /// destructor
    virtual ~ShaderSlot();
    /// set shader node name
    void SetNodeName(const Util::String& n);
    /// get shader node name
    const Util::String& GetNodeName() const;
    /// set slot type
    void SetSlotType(SlotType t);
    /// get slot type
    SlotType GetSlotType() const;
    /// set slot channel
    void SetSlotChannel(SlotChannel chn);
    // get slot channel
    SlotChannel GetSlotChannel() const;
    /// set slot parameter name
    void SetName(const Util::String& n);
    /// get slot parameter name
    const Util::String& GetName() const;
    /// set slot parameter type
    void SetDataType(const Util::String& t);
    /// get slot parameter type
    const Util::String& GetDataType() const;
    /// set slot semantics
    void SetSemantics(const Util::String& s);
    /// get slot semantics, generate semantics from slot name if not set yet
    const Util::String& GetSemantics();
    /// add a connection (must have compatible type)
    void AddConnection(const Ptr<ShaderSlot>& slot);
    /// get connections
    const Util::Array<Ptr<ShaderSlot>>& GetConnections() const;
    /// convert slot type to string
    static Util::String SlotTypeAsString(SlotType t);
    /// convert slot channel to string
    static Util::String SlotChannelAsString(SlotChannel c);
    /// clear connections (must be called before destruction to remove cyclic dependencies)
    void ClearConnections();

private:
    SlotType slotType;
    SlotChannel slotChannel;
    Util::String nodeName;
    Util::String name;
    Util::String dataType;
    Util::String semantics;
    Util::Array<Ptr<ShaderSlot>> connections;
};

} // namespace Tools
//------------------------------------------------------------------------------
#endif

    