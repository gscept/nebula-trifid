#pragma once
#ifndef TOOLS_SHADER_H
#define TOOLS_SHADER_H
//------------------------------------------------------------------------------
/**
    @class Tools::Shader
    
    Wraps a shader definition (consisting of ShaderNodes and
    ShaderBindings) from a shader definition xml file.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/xmlreader.h"
#include "shaderbinding.h"
#include "shaderparam.h"
#include "shaderslot.h"
#include "io/textwriter.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderNode;
class ShaderFragment;

class Shader : public Core::RefCounted
{
    __DeclareClass(Shader);
public:
    /// constructor
    Shader();
    /// destructor
    virtual ~Shader();
    /// parse the shader definition from an XML reader
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);
    /// clear default nodes
    void ClearDefaultNodes();
    /// setup default shader nodes
    void SetupDefaultNodes();
    /// setup manual bindings
    bool SetupManualBindings();
    /// setup default bindings for slots without manual binding
    bool SetupDefaultBindings();
    /// reset shader nodes for recompilation
    void ResetNodes();
    /// print shader node structure
    void DebugDumpShaderNodes(const Ptr<IO::TextWriter>& textWriter);
    /// get error string
    const Util::String& GetError() const;
    
    /// set file uri (there may be multiple shaders defined in a single file)
    void SetFileUri(const IO::URI& uri);
    /// get file uri
    const IO::URI& GetFileUri() const;
    /// get the shader's name
    const Util::String& GetName() const;    
    /// get shader nodes
    const Util::Dictionary<Util::String, Ptr<ShaderNode>>& GetShaderNodes() const;
    /// get all used fragments
    Util::Array<Ptr<ShaderFragment>> GatherShaderFragments() const;
    /// get dependent shader nodes in reverse dependency order
    void GetUplinkDependencyNodes(const Util::String& startNode, ShaderSlot::SlotType slotType, Util::Array<Ptr<ShaderNode>>& inOutNodes) const;

private:
    /// set error string
    void __cdecl SetError(const char* fmt, ...);
    /// setup a single default slot connection (for slots which haven't been connected by manual bindings)
    bool SetupDefaultSlotConnection(const Ptr<ShaderNode>& shaderNode, const Ptr<ShaderSlot>& shaderSlot);
    /// setup a single downlink slot connection
    bool SetupDownlinkSlotConnection(const Ptr<ShaderNode>& shaderNode, const Util::String& paramStr, const Util::String& bindNode, const Util::String& bindParam);
    /// setup a single uplink slot connection
    bool SetupUplinkSlotConnection(const Ptr<ShaderNode>& shaderNode, const Util::String& paramStr, const Util::String& bindNode, const Util::String& bindParam);
    /// setup missing interpolator connections
    bool SetupInterpolatorPassThroughConnections();
    /// setup interpolator semantics
    bool SetupInterpolatorSemantics();

    Util::String error;
    IO::URI fileUri;
    Util::String name;
    Util::Dictionary<Util::String, Ptr<ShaderNode>> shaderNodes;
};

} // namespace Tools
//------------------------------------------------------------------------------
#endif
