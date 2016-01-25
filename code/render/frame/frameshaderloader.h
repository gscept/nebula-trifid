#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameShaderLoader
    
    Loader class to load frame shader from XML stream.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "frame/frameshader.h"
#include "io/uri.h"
#include "io/xmlreader.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/shaderinstance.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameShaderLoader
{
public:
    /// load a frame shader from an XML file
    static Ptr<FrameShader> LoadFrameShader(const Resources::ResourceId& name, const IO::URI& uri);

private:
    /// parse frame shader from XML
    static void ParseFrameShader(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse render target declaration from XML
    static void ParseRenderTarget(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse render target cube declaration from xml
    static void ParseRenderTargetCube(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse multiple render target declaration from XML
    static void ParseMultipleRenderTarget(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
	/// parse depth stencil render target declaration from XML
	static void ParseDepthStencilTarget(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse texture declaration from XML
    static void ParseTexture(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse shader variable instance from XML
    static void ParseShaderVariableInstance(const Ptr<IO::XmlReader>& xmlReader, const Ptr<CoreGraphics::Shader>& shd, const Ptr<FramePassBase>& pass);
	/// parse shader variable instance from XML
	static void ParseShaderVariableInstance(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameBatch>& batch);
    /// parse frame pass from XML
    static void ParseFramePass(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
	/// parse frame compute
    static void ParseFrameCompute(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse frame copy
    static void ParseCopy(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
	/// parse frame copy
	static void ParseClear(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
	/// parse frame algorithm from XML
	static void ParseFrameAlgorithm(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse post effect from XML
    static void ParsePostEffect(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader);
    /// parse frame batch from XML
    static Ptr<FrameBatch> ParseFrameBatch(const Ptr<IO::XmlReader>& xmlReader, const Util::String& passName);
};

} // namespace Frame
//------------------------------------------------------------------------------
   