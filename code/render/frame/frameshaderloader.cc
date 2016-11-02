//------------------------------------------------------------------------------
//  frameshaderloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frameshaderloader.h"
#include "frame/framepass.h"
#include "frame/frameposteffect.h"
#include "frame/framelights.h"
#include "frame/framegui.h"
#include "frame/frametext.h"
#include "frame/frameshapes.h"
#include "frame/framecompute.h"
#include "frame/framerendertargetblit.h"
#include "lighting/lightserver.h"
#include "io/ioserver.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "frame/batchgroup.h"
#include "resources/resourcemanager.h"
#if __PS3__
#include "coregraphics/ps3/ps3rendertarget.h"
#endif
#include "frameexternal.h"
#include "frameserver.h"
#include "framealgorithm.h"
#include "core/factory.h"
#include "algorithm/attic/algorithmbase.h"
#include "algorithm/attic/algorithms.h"
#include "frametexclear.h"
#include "coregraphics/config.h"

namespace Frame
{
using namespace Lighting;
using namespace Resources;
using namespace CoreGraphics;
using namespace Models;
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
Ptr<FrameShader>
FrameShaderLoader::LoadFrameShader(const ResourceId& name, const URI& uri)
{
    Ptr<FrameShader> frameShader;
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
	Ptr<BXmlReader> xmlReader = BXmlReader::Create();
    xmlReader->SetStream(stream);
    if (xmlReader->Open())
    {
        // make sure it's a valid frame shader file
        if (!xmlReader->HasNode("/Nebula3/FrameShader"))
        {
            n_error("FrameShaderLoader: '%s' is not a valid frame shader!", uri.AsString().AsCharPtr());
            return frameShader;
        }
        xmlReader->SetToNode("/Nebula3/FrameShader");
        frameShader = FrameShader::Create();
        frameShader->SetName(name);
        ParseFrameShader(xmlReader, frameShader);
        xmlReader->Close();
    }
    else
    {
        n_error("FrameShaderLoader: failed to load frame shader '%s'!", uri.AsString().AsCharPtr());
    }
    return frameShader;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseFrameShader(const Ptr<BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
	// parse depth-stencil target declarations
	if (xmlReader->SetToFirstChild("DeclareDepthStencilTarget")) do
	{
		ParseDepthStencilTarget(xmlReader, frameShader);
	}
	while (xmlReader->SetToNextChild("DeclareDepthStencilTarget")); 

    // parse render target declarations
    if (xmlReader->SetToFirstChild("DeclareRenderTarget")) do
    {
        ParseRenderTarget(xmlReader, frameShader);
    }
    while (xmlReader->SetToNextChild("DeclareRenderTarget")); 
 
    // parse texture cube declarations
    if (xmlReader->SetToFirstChild("DeclareRenderTargetCube")) do
    {
        ParseRenderTargetCube(xmlReader, frameShader);
    }
    while (xmlReader->SetToNextChild("DeclareRenderTargetCube"));

    // parse texture declarations
    if (xmlReader->SetToFirstChild("DeclareTexture")) do
    {
        ParseReadWriteTexture(xmlReader, frameShader);
    }
    while (xmlReader->SetToNextChild("DeclareTexture"));

    // parse multiple render target declarations
    if (xmlReader->SetToFirstChild("DeclareMRT")) do
    {
        ParseMultipleRenderTarget(xmlReader, frameShader);
    }
    while (xmlReader->SetToNextChild("DeclareMRT"));

    // parse frame passes and post effects, no other child types allowed from here!
    if (xmlReader->SetToFirstChild("GlobalState") ||
		xmlReader->SetToFirstChild("Pass") || 
		xmlReader->SetToFirstChild("PostEffect") ||
		xmlReader->SetToFirstChild("Lights") ||
		xmlReader->SetToFirstChild("Text") ||
		xmlReader->SetToFirstChild("GUI") ||
		xmlReader->SetToFirstChild("Shapes") ||
		xmlReader->SetToFirstChild("Compute") ||
		xmlReader->SetToFirstChild("Clear")) do
    {
		if (xmlReader->GetCurrentNodeName() == "GlobalState")
		{
			ParseFrameGlobalState(xmlReader, frameShader);
		}
        else if (xmlReader->GetCurrentNodeName() == "Pass")
        {
            ParseFramePass(xmlReader, frameShader);
        }
        else if (xmlReader->GetCurrentNodeName() == "PostEffect")
        {   
            ParsePostEffect(xmlReader, frameShader);
        }
		else if (xmlReader->GetCurrentNodeName() == "Compute")
		{
			ParseFrameCompute(xmlReader, frameShader);
		}
		else if (xmlReader->GetCurrentNodeName() == "Algorithm")
		{
			ParseFrameAlgorithm(xmlReader, frameShader);
		}
        else if (xmlReader->GetCurrentNodeName() == "Copy")
        {
            ParseCopy(xmlReader, frameShader);
        }
		else if (xmlReader->GetCurrentNodeName() == "Clear")
		{
			ParseClear(xmlReader, frameShader);
		}
    }
    while (xmlReader->SetToNextChild());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseRenderTarget(const Ptr<BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    n_assert(DisplayDevice::Instance()->IsOpen());

    // create and configure a new render target
    Ptr<RenderTarget> renderTarget = RenderTarget::Create();
    const DisplayMode& displayMode = DisplayDevice::Instance()->GetCurrentWindow()->GetDisplayMode();
    String name = xmlReader->GetString("name");
    renderTarget->SetResolveTextureResourceId(name);
    renderTarget->SetColorBufferFormat(PixelFormat::FromString(xmlReader->GetString("format")));
	/*
    if (xmlReader->HasAttr("depth"))
    {
        if (xmlReader->GetBool("depth"))
        {
            renderTarget->AddDepthStencilBuffer();
        }
    }
    if (xmlReader->HasAttr("depthTexture"))
    {
        n_assert2(xmlReader->HasAttr("depth") && xmlReader->GetBool("depth"), "Depthbuffer must be declared for a resolve!");
        renderTarget->SetResolveDepthTextureResourceId(xmlReader->GetString("depthTexture"));
    }
	*/
	if (xmlReader->HasAttr("depthTarget"))
	{
		String name = xmlReader->GetString("depthTarget");
		n_assert(frameShader->HasDepthStencilTarget(name));
		renderTarget->SetDepthStencilTarget(frameShader->GetDepthStencilTargetByName(name));
	}
    if (xmlReader->HasAttr("width"))
    {
        renderTarget->SetWidth(xmlReader->GetInt("width"));
    }
    if (xmlReader->HasAttr("height"))
    {
        renderTarget->SetHeight(xmlReader->GetInt("height"));
    }
    if (xmlReader->HasAttr("relWidth"))
    {
		renderTarget->SetRelativeWidth(xmlReader->GetFloat("relWidth"));
    }
    if (xmlReader->HasAttr("relHeight"))
    {
		renderTarget->SetRelativeHeight(xmlReader->GetFloat("relHeight"));
    }
    if (xmlReader->HasAttr("msaa"))
    {
        if (xmlReader->GetBool("msaa"))
        {
            renderTarget->SetAntiAliasQuality(DisplayDevice::Instance()->GetCurrentWindow()->GetAntiAliasQuality());
        }
    }
    if (xmlReader->HasAttr("cpuAccess"))
    {
        renderTarget->SetResolveTargetCpuAccess(xmlReader->GetBool("cpuAccess"));
    }
    // special case for PS3, if more extra handling is necessary make frameshaderloader platform specific
#if __PS3__
    if (xmlReader->HasAttr("bindToTile"))
    {
        renderTarget.cast<PS3::PS3RenderTarget>()->SetBindToTile(xmlReader->GetBool("bindToTile"));
    }
#endif
    // special case for xbox360, currently also usable by other platforms
    if (xmlReader->HasAttr("PrecededRenderTarget"))
    {
        Util::String previousRenderTarget = xmlReader->GetString("PrecededRenderTarget");
        const Ptr<RenderTarget>& pRenderTarget = frameShader->GetRenderTargetByName(previousRenderTarget);
        renderTarget->SetMemoryOffset(pRenderTarget->GetMemorySize());
    }
    renderTarget->Setup();
    frameShader->AddRenderTarget(name, renderTarget);
}


//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseRenderTargetCube(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    n_assert(DisplayDevice::Instance()->IsOpen());

    // create and configure a new render target
    Ptr<RenderTargetCube> renderTarget = RenderTargetCube::Create();
    String name = xmlReader->GetString("name");
    renderTarget->SetResolveTextureResourceId(name);
    renderTarget->SetColorBufferFormat(PixelFormat::FromString(xmlReader->GetString("format")));
	/*
    if (xmlReader->HasAttr("depth"))
    {
        if (xmlReader->GetBool("depth"))
        {
            renderTarget->AddDepthStencilBuffer();
        }
    }
    if (xmlReader->HasAttr("depthTexture"))
    {
        n_assert2(xmlReader->HasAttr("depth") && xmlReader->GetBool("depth"), "Depthbuffer must be declared for a resolve!");
        renderTarget->SetResolveDepthTextureResourceId(xmlReader->GetString("depthTexture"));
    }
	*/
	if (xmlReader->HasAttr("useDepthCube"))
	{
		n_assert(frameShader->HasDepthStencilTarget(name));
        renderTarget->SetDepthStencilCube(xmlReader->GetBool("useDepthCube"));
	}
    if (xmlReader->HasAttr("width"))
    {
        renderTarget->SetWidth(xmlReader->GetInt("width"));
    }
    if (xmlReader->HasAttr("height"))
    {
        renderTarget->SetHeight(xmlReader->GetInt("height"));
    }
    if (xmlReader->HasAttr("cpuAccess"))
    {
        renderTarget->SetResolveTargetCpuAccess(xmlReader->GetBool("cpuAccess"));
    }
    // special case for PS3, if more extra handling is necessary make frameshaderloader platform specific
#if __PS3__
    if (xmlReader->HasAttr("bindToTile"))
    {
        renderTarget.cast<PS3::PS3RenderTarget>()->SetBindToTile(xmlReader->GetBool("bindToTile"));
    }
#endif
    renderTarget->Setup();
    frameShader->AddRenderTargetCube(name, renderTarget);
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShaderLoader::ParseDepthStencilTarget(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
	Ptr<DepthStencilTarget> depthStencilTarget = DepthStencilTarget::Create();
	String name = xmlReader->GetString("name");
	depthStencilTarget->SetResolveTextureResourceId(name);
	depthStencilTarget->SetDepthStencilBufferFormat(PixelFormat::FromString(xmlReader->GetString("format")));
	if (xmlReader->HasAttr("width"))
	{
		depthStencilTarget->SetWidth(xmlReader->GetInt("width"));
	}
	if (xmlReader->HasAttr("height"))
	{
		depthStencilTarget->SetHeight(xmlReader->GetInt("width"));
	}
	if (xmlReader->HasAttr("relWidth"))
	{
		depthStencilTarget->SetRelativeWidth(xmlReader->GetFloat("relWidth"));
	}
	if (xmlReader->HasAttr("relHeight"))
	{
		depthStencilTarget->SetRelativeHeight(xmlReader->GetFloat("relHeight"));
	}

	depthStencilTarget->Setup();
	frameShader->AddDepthStencilTarget(name, depthStencilTarget);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseShaderVariableValue(const Ptr<XmlReader>& xmlReader, const Ptr<ShaderState>& shd, const Ptr<FramePassBase>& pass)
{
    /// create a shader variable instance by semantic
    String name = xmlReader->GetString("sem");
	const Ptr<ShaderVariable>& shdVar = shd->GetVariableByName(name);

	/// get the default value of the shader variable
	String valueStr = xmlReader->GetString("value");
	switch (shdVar->GetType())
	{
	case ShaderVariable::IntType:
		shdVar->SetInt(valueStr.AsInt());
		break;

	case ShaderVariable::FloatType:
		shdVar->SetFloat(valueStr.AsFloat());
		break;

	case ShaderVariable::VectorType:
		shdVar->SetFloat4(valueStr.AsFloat4());
		break;

	case ShaderVariable::Vector2Type:
		shdVar->SetFloat2(valueStr.AsFloat2());
		break;

	case ShaderVariable::MatrixType:
		shdVar->SetMatrix(valueStr.AsMatrix44());
		break;

	case ShaderVariable::BoolType:
		shdVar->SetBool(valueStr.AsBool());
		break;

	case ShaderVariable::SamplerType:
	case ShaderVariable::TextureType:
		{                  
			// rendertargets names are also theory resource ids, texture have a name -> resourceid mapping
			const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
            if (resManager->HasResource(valueStr))
            {
                const Ptr<Resource>& resource = resManager->LookupResource(valueStr);
				shdVar->SetTexture(resource.downcast<Texture>());
            }
			else
			{
				const Ptr<ManagedResource>& resource = resManager->CreateManagedResource(Texture::RTTI, valueStr);
				shdVar->SetTexture(resource.downcast<ManagedTexture>()->GetTexture());
			}
		}
		break;

	case ShaderVariable::ImageReadWriteType:
		{                  
			// rendertargets names are also theory resource ids, texture have a name -> resourceid mapping
			const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
            if (resManager->HasResource(valueStr))
            {
                const Ptr<Resource>& resource = resManager->LookupResource(valueStr);
				shdVar->SetShaderReadWriteTexture(resource.downcast<Texture>());
            }
		}
		break;
	default:
		n_error("FrameShaderLoader::ParseShaderVariableInstance(): invalid shader variable type!\n");
		break;
	}
	
	// add variable
	pass->AddVariable(name, shdVar);   
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseShaderVariableValue(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameBatch>& batch)
{
	/// create a shader variable instance by semantic
	String semantic = xmlReader->GetString("sem");
	Ptr<ShaderVariableInstance> shdVarInst = ShaderVariableInstance::Create();

	/// get the default value of the shader variable
	String valueStr = xmlReader->GetString("value");
	if (valueStr.IsValidInt())
	{
		shdVarInst->Setup(ShaderVariable::IntType);
		shdVarInst->SetInt(valueStr.AsInt());
	}
	else if (valueStr.IsValidFloat())
	{
		shdVarInst->Setup(ShaderVariable::FloatType);
		shdVarInst->SetFloat(valueStr.AsFloat());
	}
	else if (valueStr.IsValidFloat4())
	{
		shdVarInst->Setup(ShaderVariable::VectorType);
		shdVarInst->SetFloat4(valueStr.AsFloat4());
	}
	else if (valueStr.IsValidFloat2())
	{
		shdVarInst->Setup(ShaderVariable::Vector2Type);
		shdVarInst->SetFloat2(valueStr.AsFloat2());
	}
	else if (valueStr.IsValidMatrix44())
	{
		shdVarInst->Setup(ShaderVariable::MatrixType);
		shdVarInst->SetMatrix(valueStr.AsMatrix44());
	}
	else if (valueStr.IsValidBool())
	{
		shdVarInst->Setup(ShaderVariable::BoolType);
		shdVarInst->SetBool(valueStr.AsBool());
	}
	else
	{
		// assume texture
		shdVarInst->Setup(ShaderVariable::TextureType);

		// rendertargets names are also theory resource ids, texture have a name -> resourceid mapping
		const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
		if (resManager->HasResource(valueStr))
		{
			const Ptr<Resource>& resource = resManager->LookupResource(valueStr);
			shdVarInst->SetTexture(resource.downcast<Texture>()); 
		}
	}

	// add variable
	batch->AddVariable(semantic, shdVarInst);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseFrameGlobalState(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{

}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseFramePass(const Ptr<XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    // create frame pass, optionally of requested class (default is FramePass)
    String framePassClass = xmlReader->GetOptString("class", "Frame::FramePass");
    Ptr<FramePass> framePass = (FramePass*) Core::Factory::Instance()->Create(framePassClass);
    n_assert(framePass->IsA(Frame::FramePass::RTTI));

	// get name
	framePass->SetName(xmlReader->GetString("name"));

	Ptr<ShaderState> shader = NULL;
	if (xmlReader->HasAttr("shader"))
	{
		// setup the pass shader
		ResourceId shdResId = ResourceId("shd:" + xmlReader->GetString("shader"));
		Ptr<ShaderState> shader = ShaderServer::Instance()->CreateShaderState(shdResId, { NEBULAT_DEFAULT_GROUP } );
		framePass->SetShaderState(shader);

		// add shader variable instances
		if (xmlReader->SetToFirstChild("ApplyShaderVariable")) do
		{
			ParseShaderVariableValue(xmlReader, shader, framePass.upcast<FramePassBase>());
		}
		while (xmlReader->SetToNextChild("ApplyShaderVariable"));
	}	
  
    // setup the render target (if not render to default render target)
    bool useDefaultRendertarget = true;
    if (xmlReader->HasAttr("renderTarget"))
    {
        n_assert2(!xmlReader->HasAttr("multipleRenderTarget"), "Rendertarget and multiple rendertarget are exclusive!");
        useDefaultRendertarget = false;
        String rtName = xmlReader->GetString("renderTarget");
        if (frameShader->HasRenderTarget(rtName))
        {
            framePass->SetRenderTarget(frameShader->GetRenderTargetByName(rtName));
        }
        else
        {
            n_error("FrameShaderLoader: render target '%s' not declared (%s, node %s)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
                xmlReader->GetCurrentNodeName().AsCharPtr());
        }    
    }
    else if (xmlReader->HasAttr("multipleRenderTarget"))
    {
        useDefaultRendertarget = false;
        String rtName = xmlReader->GetString("multipleRenderTarget");
        if (frameShader->HasMultipleRenderTarget(rtName))
        {
            framePass->SetMultipleRenderTarget(frameShader->GetMultipleRenderTargetByName(rtName));
        }
        else
        {
            n_error("FrameShaderLoader: multiple render target '%s' not declared (%s, node %s)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
        } 
    }
    else if (xmlReader->HasAttr("renderTargetCube"))
    {
		useDefaultRendertarget = false;
        String rtName = xmlReader->GetString("renderTargetCube");
        if (frameShader->HasRenderTargetCube(rtName))
        {
            framePass->SetRenderTargetCube(frameShader->GetRenderTargetCubeByName(rtName));
        }
        else
        {
            n_error("FrameShaderLoader: render target cube '%s' not declared (%s, node %s)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
        } 
    }

    // setup the clear color, depth and stencil (if defined)
    uint clearFlags = 0;
    if (xmlReader->HasAttr("clearColor"))
    {
        // clear color for all rendertargets
        framePass->SetClearColor(xmlReader->GetFloat4("clearColor"));
        clearFlags |= RenderTarget::ClearColor;
    }
    if (xmlReader->HasAttr("clearDepth"))
    {
        framePass->SetClearDepth(xmlReader->GetFloat("clearDepth"));
        clearFlags |= DepthStencilTarget::ClearDepth;
    }
    if (xmlReader->HasAttr("clearStencil"))
    {
        framePass->SetClearStencil(uchar(xmlReader->GetInt("clearStencil")));
        clearFlags |= DepthStencilTarget::ClearStencil;
    }
    framePass->SetClearFlags(clearFlags);

    // add render batches
    if (xmlReader->SetToFirstChild("Batch")) do
    {
        Ptr<FrameBatch> frameBatch = ParseFrameBatch(xmlReader, framePass->GetName().Value());
        framePass->AddBatch(frameBatch);
    }
    while (xmlReader->SetToNextChild("Batch"));

    // add frame pass to frame shader
    frameShader->AddFramePassBase(framePass.cast<FramePassBase>());

#if NEBULA3_ENABLE_PROFILING
    framePass->SetFramePassDebugTimer(Util::String("FramePass_") + framePass->GetName().Value());
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseFrameCompute(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    // create frame compute object
    Ptr<FrameCompute> frameComp = FrameCompute::Create();

    // setup the pass shader
    ResourceId shdResId = ResourceId("shd:" + xmlReader->GetString("shader"));
	Ptr<ShaderState> shader = ShaderServer::Instance()->CreateShaderState(shdResId, { NEBULAT_DEFAULT_GROUP } );
    frameComp->SetShaderState(shader);
	frameComp->SetName(xmlReader->GetString("name"));

    // add shader variable instances
    if (xmlReader->SetToFirstChild("ApplyShaderVariable")) do
    {
        ParseShaderVariableValue(xmlReader, shader, frameComp.upcast<FramePassBase>());
    }
    while (xmlReader->SetToNextChild("ApplyShaderVariable"));

    // get relative width
    if (xmlReader->HasAttr("relDimX"))
    {
        float rel = xmlReader->GetFloat("relDimX");
        frameComp->SetRelativeWidth(rel);
    }

    // get relative height
    if (xmlReader->HasAttr("relDimY"))
    {
        float rel = xmlReader->GetFloat("relDimY");
        frameComp->SetRelativeHeight(rel);
    }

    // get compute sizes
    uint sizeX = xmlReader->GetOptInt("dimX", 1);
    uint sizeY = xmlReader->GetOptInt("dimY", 1);
    uint sizeZ = xmlReader->GetOptInt("dimZ", 1);
    frameComp->SetComputeSize(sizeX, sizeY, sizeZ);    

    // get group sizes
    sizeX = xmlReader->GetOptInt("groupsX", 1);
    sizeY = xmlReader->GetOptInt("groupsY", 1);
    sizeZ = xmlReader->GetOptInt("groupsZ", 1);
    frameComp->SetGroupSize(sizeX, sizeY, sizeZ);

    // setup compute
    frameComp->Setup();

    // finally add to frameshader
    frameShader->AddFramePassBase(frameComp.upcast<FramePassBase>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseCopy(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    Ptr<FrameRenderTargetBlit> copy = FrameRenderTargetBlit::Create();

    // read from and to target
    String from = xmlReader->GetString("from");
    String to = xmlReader->GetString("to");

    // get targets and set in pass
    const Ptr<RenderTarget>& fromTarget = frameShader->GetRenderTargetByName(from);
    const Ptr<RenderTarget>& toTarget = frameShader->GetRenderTargetByName(to);
    copy->SetFrom(fromTarget);
    copy->SetTo(toTarget);

    // add to frame shader
    frameShader->AddFramePassBase(copy.upcast<FramePassBase>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseClear(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
	Ptr<FrameTexClear> clear = FrameTexClear::Create();
	String tex = xmlReader->GetString("name");
	Math::float4 clearColor = xmlReader->GetFloat4("clearColor");
	Ptr<CoreGraphics::ShaderReadWriteTexture> texture = frameShader->GetTextureByName(tex);

	// setup clear pass
	clear->Setup(texture, clearColor);

	// add to frame shader
	frameShader->AddFramePassBase(clear.upcast<FramePassBase>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParseFrameAlgorithm(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
	// create frame algorithm object
	Ptr<FrameAlgorithm> frameAlg = FrameAlgorithm::Create();

	// get the class name of the algorithm
	String className = xmlReader->GetString("className");

	// instantiate object of that type
	Core::RefCounted* object = Core::Factory::Instance()->Create(className);

	// cast to algorithm base class
	Ptr<Algorithm::AlgorithmBase> algorithm = (Algorithm::AlgorithmBase*)object;

	// parse inputs
	if (xmlReader->SetToFirstChild("Input")) do 
	{
		// read name, then get and add texture
		String name = xmlReader->GetString("name");
		const Ptr<Texture>& source = frameShader->GetRenderTargetByName(name)->GetResolveTexture();
		algorithm->AddInput(source);
	} 
	while (xmlReader->SetToNextChild("Input"));

	// and outputs
	if (xmlReader->SetToFirstChild("Output")) do 
	{
		// read name, then add output
		String name = xmlReader->GetString("name");
		algorithm->AddOutput(name);
	} 
	while (xmlReader->SetToNextChild("Output"));

	// and targets
	if (xmlReader->SetToFirstChild("Target")) do
	{
		String name = xmlReader->GetString("name");
		const Ptr<RenderTarget>& target = frameShader->GetRenderTargetByName(name);
		algorithm->AddRenderTarget(target);
	}
	while (xmlReader->SetToNextChild("Target"));

	// setup algorithm
	algorithm->Setup();
	
	// cast to algorithm and set in frame algorithm
	frameAlg->SetAlgorithm(algorithm);

	// add to frame shader
	frameShader->AddFramePassBase(frameAlg.upcast<Frame::FramePassBase>());
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FrameBatch>
FrameShaderLoader::ParseFrameBatch(const Ptr<BXmlReader>& xmlReader, const Util::String& passName)
{
    Ptr<FrameBatch> frameBatch = FrameBatch::Create();

    // setup batch type, model node filter, lighting and sorting mode
    frameBatch->SetType(FrameBatchType::FromString(xmlReader->GetString("type")));
    if (xmlReader->HasAttr("batchGroup"))
    {
        frameBatch->SetBatchGroup(BatchGroup::FromName(xmlReader->GetString("batchGroup")));
    }
    if (xmlReader->HasAttr("lighting"))
    {
        frameBatch->SetLightingMode(LightingMode::FromString(xmlReader->GetString("lighting")));
    }
    if (xmlReader->HasAttr("sorting"))
    {
        frameBatch->SetSortingMode(SortingMode::FromString(xmlReader->GetString("sorting")));
    }
    if (xmlReader->HasAttr("instances"))
    {
        frameBatch->SetForceInstancing(true, xmlReader->GetInt("instances"));
    }

    // add shader variable instances
    if (xmlReader->SetToFirstChild("ApplyShaderVariable")) do
    {
		ParseShaderVariableValue(xmlReader, frameBatch);
    }
    while (xmlReader->SetToNextChild("ApplyShaderVariable"));

#if NEBULA3_ENABLE_PROFILING
    // add debug timer
    Util::String name;
    name.Format("FrameBatch:%s_%s_%s_%s_%s",
        passName.AsCharPtr(),
        xmlReader->GetString("type").AsCharPtr(),
		xmlReader->GetOptString("batchGroup", "").AsCharPtr(),
        xmlReader->GetOptString("lighting", "").AsCharPtr(),
        xmlReader->GetOptString("sorting", "").AsCharPtr());
    frameBatch->SetBatchDebugTimer(name);
#endif

    return frameBatch;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShaderLoader::ParsePostEffect(const Ptr<BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    // create and configure a new post effect
    Ptr<FramePostEffect> framePostEffect = FramePostEffect::Create();
    framePostEffect->SetName(xmlReader->GetString("name"));

    // setup the pass shader
    ResourceId shdResId = ResourceId("shd:" + xmlReader->GetString("shader"));

	// heh, throw one away into the ether
	Ptr<ShaderState> shader = ShaderServer::Instance()->CreateShaderState(shdResId, { NEBULAT_DEFAULT_GROUP });
	//Ptr<ShaderState> shader = ShaderServer::Instance()->GetShader(shdResId)->GetMainState();
    framePostEffect->SetShaderState(shader);

    // setup the render target (if not render to default render target)
    bool useDefaultRendertarget = true;
    if (xmlReader->HasAttr("renderTarget"))
    {
        n_assert2(!xmlReader->HasAttr("multipleRenderTarget"), "Rendertarget and multiple rendertarget are exclusive!");
        useDefaultRendertarget = false;
        String rtName = xmlReader->GetString("renderTarget");
        if (frameShader->HasRenderTarget(rtName))
        {
            framePostEffect->SetRenderTarget(frameShader->GetRenderTargetByName(rtName));
        }
        else
        {
            n_error("FrameShaderLoader: render target '%s' not declared (%s, node %s)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
        }    
    }
    if (xmlReader->HasAttr("multipleRenderTarget"))
    {
        useDefaultRendertarget = false;
        String rtName = xmlReader->GetString("multipleRenderTarget");
        if (frameShader->HasMultipleRenderTarget(rtName))
        {
            framePostEffect->SetMultipleRenderTarget(frameShader->GetMultipleRenderTargetByName(rtName));
        }
        else
        {
            n_error("FrameShaderLoader: multiple render target '%s' not declared (%s, line %d)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
        } 
    }

	int clearFlags = 0;
	if (xmlReader->HasAttr("clearColor"))
	{
		framePostEffect->SetClearColor(xmlReader->GetFloat4("clearColor"));
		clearFlags |= RenderTarget::ClearColor;
	}

	framePostEffect->SetClearFlags(clearFlags);

    // add shader variable instances
    if (xmlReader->SetToFirstChild("ApplyShaderVariable")) do
    {
		if (shader->HasVariableByName(xmlReader->GetString("sem")))
		{
			ParseShaderVariableValue(xmlReader, shader, framePostEffect.upcast<FramePassBase>());
		}
    }
    while (xmlReader->SetToNextChild("ApplyShaderVariable"));

    // add render batches
    if (xmlReader->SetToFirstChild("Batch")) do
    {
        Ptr<FrameBatch> frameBatch = ParseFrameBatch(xmlReader, framePostEffect->GetName().Value());
        framePostEffect->AddBatch(frameBatch);
    }
    while (xmlReader->SetToNextChild("Batch"));

    // setup frame post effect
    framePostEffect->Setup();

    // add post effect to frame shader
    frameShader->AddFramePassBase(framePostEffect.cast<FramePassBase>());

#if NEBULA3_ENABLE_PROFILING
    framePostEffect->SetFramePostEffectDebugTimer(Util::String("FramePostEffect_") + framePostEffect->GetName().Value());
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShaderLoader::ParseMultipleRenderTarget(const Ptr<IO::BXmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    n_assert(DisplayDevice::Instance()->IsOpen());

    // create and configure a new render target
    Ptr<MultipleRenderTarget> multipleRenderTarget = MultipleRenderTarget::Create();
    String name = xmlReader->GetString("name");

	// set depth stencil targets
	if (xmlReader->SetToFirstChild("DepthStencilTarget"))
	{
		int clearFlags = 0;
		String dtName = xmlReader->GetString("name");
		if (frameShader->HasDepthStencilTarget(dtName))
		{
			multipleRenderTarget->SetDepthStencilTarget(frameShader->GetDepthStencilTargetByName(dtName));
			if (xmlReader->HasAttr("clearDepth"))
			{
				multipleRenderTarget->SetClearDepth(xmlReader->GetFloat("clearDepth"));
				clearFlags |= DepthStencilTarget::ClearDepth;
			}
			if (xmlReader->HasAttr("clearStencil"))
			{
				multipleRenderTarget->SetClearStencil((uchar)xmlReader->GetInt("clearStencil"));
				clearFlags |= DepthStencilTarget::ClearStencil;
			}
			multipleRenderTarget->SetDepthStencilClearFlags(clearFlags);
		}
		else
		{
			n_error("FrameShaderLoader: render target '%s' not declared (%s, node %s)",
				dtName.AsCharPtr(),
				xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
		}  
		xmlReader->SetToParent();
	}
	

    // collect all rendertargets 
    if (xmlReader->SetToFirstChild("RenderTarget")) do
    {
		int clearFlags = 0;
        String rtName = xmlReader->GetString("name");
        if (frameShader->HasRenderTarget(rtName))
        {
            multipleRenderTarget->AddRenderTarget(frameShader->GetRenderTargetByName(rtName));
            Math::float4 color(0.0f, 0.0f, 0.0f, 1.0f);
            if (xmlReader->HasAttr("clearColor"))
            {
                color = xmlReader->GetFloat4("clearColor");
				clearFlags |= RenderTarget::ClearColor;
            }
            multipleRenderTarget->SetClearColor(multipleRenderTarget->GetNumRendertargets() - 1, color);
			multipleRenderTarget->SetClearFlags(multipleRenderTarget->GetNumRendertargets() - 1, clearFlags);
        }
        else
        {
            n_error("FrameShaderLoader: render target '%s' not declared (%s, node %s)",
                rtName.AsCharPtr(),
                xmlReader->GetStream()->GetURI().AsString().AsCharPtr(), 
				xmlReader->GetCurrentNodeName().AsCharPtr());
        }   
    }
    while (xmlReader->SetToNextChild("RenderTarget"));

	multipleRenderTarget->Setup();
    frameShader->AddMultipleRenderTarget(name, multipleRenderTarget);
}

//------------------------------------------------------------------------------
/** 
*/
void 
FrameShaderLoader::ParseReadWriteTexture(const Ptr<IO::XmlReader>& xmlReader, const Ptr<FrameShader>& frameShader)
{
    n_assert(DisplayDevice::Instance()->IsOpen());

    StringAtom name = xmlReader->GetString("name");    
	PixelFormat::Code format = PixelFormat::FromString(xmlReader->GetString("format"));
	Ptr<ShaderReadWriteTexture> texture = ShaderReadWriteTexture::Create();
	if (xmlReader->HasAttr("width") && xmlReader->HasAttr("height"))
	{
		texture->Setup(xmlReader->GetInt("width"), xmlReader->GetInt("height"), format, name);
	}
	else if (xmlReader->HasAttr("relWidth") && xmlReader->HasAttr("relHeight"))
	{
		texture->SetupWithRelativeSize(xmlReader->GetFloat("relWidth"), xmlReader->GetFloat("relHeight"), format, name);
	}
	else
	{
		n_error("Frame shader texture must have either relative or fixed heights and widths");
	}
	frameShader->AddReadWriteTexture(name, texture);
}

} // namespace Frame