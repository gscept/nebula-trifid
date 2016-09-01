//------------------------------------------------------------------------------
// framescriptloader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framescriptloader.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "framepass.h"
#include "coregraphics/rendertexture.h"
#include "frameglobalstate.h"
#include "frameblit.h"
#include "framecompute.h"
#include "framecomputealgorithm.h"
#include "framesubpass.h"
#include "frameevent.h"
#include "framesubpassalgorithm.h"
#include "framesubpassbatch.h"
#include "framesubpassorderedbatch.h"
#include "framesubpassfullscreeneffect.h"
#include "framesubpasscopy.h"
#include "framesubpasssystem.h"
#include "frameserver.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/config.h"
#include "resources/resourcemanager.h"
#include "core/factory.h"

using namespace CoreGraphics;
using namespace IO;
namespace Frame2
{

//------------------------------------------------------------------------------
/**
*/
Ptr<Frame2::FrameScript>
FrameScriptLoader::LoadFrameScript(const IO::URI& path)
{
	Ptr<FrameScript> script = FrameScript::Create();
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
	if (stream->Open())
	{
		void* data = stream->Map();
		cJSON* json = cJSON_Parse((const char*)data);
		if (json == NULL)
		{
			n_error(cJSON_GetErrorPtr());
		}

		// assert version is compatible
		cJSON* node = cJSON_GetObjectItem(json, "version");
		n_assert(node->valueint >= 2);
		node = cJSON_GetObjectItem(json, "engine");
		n_assert(Util::String(node->valuestring) == "NebulaTrifid");

		// run parser entry point
		json = cJSON_GetObjectItem(json, "framescript");
		ParseFrameScript(script, json);

		stream->Unmap();
		stream->Close();
	}
	else
	{
		n_error("Failed to load frame script '%s'\n", path.LocalPath().AsCharPtr());
	}
	return script;
}


//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseFrameScript(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		Util::String name(cur->string);
		if (name == "renderTextures")			ParseColorTextureList(script, cur);
		else if (name == "depthStencils")		ParseDepthStencilTextureList(script, cur);
		else if (name == "readWriteTextures")	ParseImageReadWriteTextureList(script, cur);
		else if (name == "readWriteBuffers")	ParseImageReadWriteBufferList(script, cur);
		else if (name == "algorithms")			ParseAlgorithmList(script, cur);
		else if (name == "events")				ParseEventList(script, cur);
		else if (name == "globalState")			ParseGlobalState(script, cur);
		else if (name == "blit")				ParseBlit(script, cur);
		else if (name == "compute")				ParseCompute(script, cur);
		else if (name == "computeAlgorithm")	ParseComputeAlgorithm(script, cur);
		else if (name == "pass")				ParsePass(script, cur);
		else
		{
			n_error("Frame script operation '%s' is unrecognized.\n", name.AsCharPtr());
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseColorTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		if (Util::String(name->valuestring) == "__WINDOW__")
		{
			// code to fetch window render texture goes here
			Ptr<CoreGraphics::RenderTexture> tex = FrameServer::Instance()->GetWindowTexture();
			script->AddColorTexture("__WINDOW__", tex);
		}
		else
		{
			cJSON* format = cJSON_GetObjectItem(cur, "format");
			n_assert(format != NULL);
			cJSON* width = cJSON_GetObjectItem(cur, "width");
			n_assert(width != NULL);
			cJSON* height = cJSON_GetObjectItem(cur, "height");
			n_assert(height != NULL);

			// get format
			CoreGraphics::PixelFormat::Code fmt = CoreGraphics::PixelFormat::FromString(format->valuestring);

			// create texture
			Ptr<CoreGraphics::RenderTexture> tex = CoreGraphics::RenderTexture::Create();
			tex->SetResourceId(name->valuestring);
			tex->SetPixelFormat(fmt);
			tex->SetUsage(RenderTexture::ColorAttachment);
			tex->SetTextureType(Texture::Texture2D);

			// set relative, dynamic or msaa if defined
			if (cJSON_HasObjectItem(cur, "relative")) tex->SetIsScreenRelative(cJSON_GetObjectItem(cur, "relative")->valueint == 1 ? true : false);
			if (cJSON_HasObjectItem(cur, "dynamic")) tex->SetIsDynamicScaled(cJSON_GetObjectItem(cur, "dynamic")->valueint == 1 ? true : false);
			if (cJSON_HasObjectItem(cur, "msaa")) tex->SetEnableMSAA(cJSON_GetObjectItem(cur, "msaa")->valueint == 1 ? true : false);

			// if cube, use 6 layers
			float depth = 1;
			if (cJSON_HasObjectItem(cur, "cube"))
			{
				bool isCube = cJSON_GetObjectItem(cur, "cube")->valueint == 1 ? true : false;
				if (isCube)
				{
					tex->SetTextureType(Texture::TextureCube);
					depth = 6;
				}
			}

			// set dimension after figuring out if the texture is a cube
			tex->SetDimensions((float)width->valuedouble, (float)height->valuedouble, depth);

			// add to script
			tex->Setup();
			script->AddColorTexture(name->valuestring, tex);
		}		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseDepthStencilTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		cJSON* format = cJSON_GetObjectItem(cur, "format");
		n_assert(name != NULL);
		cJSON* width = cJSON_GetObjectItem(cur, "width");
		n_assert(width != NULL);
		cJSON* height = cJSON_GetObjectItem(cur, "height");
		n_assert(height != NULL);

		// get format
		CoreGraphics::PixelFormat::Code fmt = CoreGraphics::PixelFormat::FromString(format->valuestring);

		// create texture
		Ptr<CoreGraphics::RenderTexture> tex = CoreGraphics::RenderTexture::Create();
		tex->SetResourceId(name->valuestring);
		tex->SetPixelFormat(fmt);
		tex->SetUsage(RenderTexture::DepthStencilAttachment);
		tex->SetTextureType(Texture::Texture2D);

		// set relative, dynamic or msaa if defined
		if (cJSON_HasObjectItem(cur, "relative")) tex->SetIsScreenRelative(cJSON_GetObjectItem(cur, "relative")->valueint == 1 ? true : false);
		if (cJSON_HasObjectItem(cur, "dynamic")) tex->SetIsDynamicScaled(cJSON_GetObjectItem(cur, "dynamic")->valueint == 1 ? true : false);
		if (cJSON_HasObjectItem(cur, "msaa")) tex->SetEnableMSAA(cJSON_GetObjectItem(cur, "msaa")->valueint == 1 ? true : false);

		// if cube, use 6 layers
		float depth = 1;
		if (cJSON_HasObjectItem(cur, "cube"))
		{
			bool isCube = cJSON_GetObjectItem(cur, "cube")->valueint == 1 ? true : false;
			if (isCube)
			{
				tex->SetTextureType(Texture::TextureCube);
				depth = 6;
			}
		}

		// set dimension after figuring out if the texture is a cube
		tex->SetDimensions((float)width->valuedouble, (float)height->valuedouble, depth);

		// add to script
		tex->Setup();
		script->AddDepthStencilTexture(name->valuestring, tex);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseImageReadWriteTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		cJSON* format = cJSON_GetObjectItem(cur, "format");
		n_assert(name != NULL);
		cJSON* width = cJSON_GetObjectItem(cur, "width");
		n_assert(width != NULL);
		cJSON* height = cJSON_GetObjectItem(cur, "height");
		n_assert(height != NULL);

		// setup shader read-write texture
		Ptr<ShaderReadWriteTexture> tex = ShaderReadWriteTexture::Create();
		CoreGraphics::PixelFormat::Code fmt = CoreGraphics::PixelFormat::FromString(format->valuestring);
		
		bool relativeSize = false;
		bool dynamicSize = false;
		bool msaa = false;
		if (cJSON_HasObjectItem(cur, "relative")) relativeSize = cJSON_GetObjectItem(cur, "relative")->valueint == 1 ? true : false;
		if (cJSON_HasObjectItem(cur, "dynamic")) dynamicSize = cJSON_GetObjectItem(cur, "dynamic")->valueint == 1 ? true : false;
		if (cJSON_HasObjectItem(cur, "msaa")) msaa = cJSON_GetObjectItem(cur, "msaa")->valueint == 1 ? true : false;
		if (relativeSize)
		{
			tex->SetupWithRelativeSize((float)width->valuedouble, (float)height->valuedouble, fmt, name->valuestring);
		}
		else
		{
			tex->Setup((SizeT)width->valuedouble, (SizeT)height->valuedouble, fmt, name->valuestring);
		}

		// add texture
		script->AddReadWriteTexture(name->valuestring, tex);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseImageReadWriteBufferList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		cJSON* size = cJSON_GetObjectItem(cur, "size");
		n_assert(size != NULL);

		// create shader buffer 
		Ptr<ShaderReadWriteBuffer> buffer = ShaderReadWriteBuffer::Create();

		bool relativeSize = false;
		if (cJSON_HasObjectItem(cur, "relative")) buffer->SetIsRelativeSize(cJSON_GetObjectItem(cur, "relative")->valueint == 1 ? true : false);

		// setup buffer
		buffer->SetSize(size->valueint);
		buffer->Setup(1);

		// add to script
		script->AddReadWriteBuffer(name->valuestring, buffer);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseAlgorithmList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);

		// algorithm needs name and class
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		cJSON* clazz = cJSON_GetObjectItem(cur, "class");
		n_assert(clazz != NULL);

		// create algorithm
		Ptr<Core::RefCounted> alg = Core::Factory::Instance()->Create(clazz->valuestring);
		script->AddAlgorithm(name->valuestring, alg.downcast<Algorithms::Algorithm>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseEventList(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);

		// create event
		Ptr<CoreGraphics::Event> ev = CoreGraphics::Event::Create();
		if (cJSON_HasObjectItem(cur, "signaled")) ev->SetSignaled(cJSON_GetObjectItem(cur, "signaled")->valueint == 1 ? true : false);

		// add event
		ev->Setup();
		script->AddEvent(name->valuestring, ev);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseGlobalState(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	Ptr<FrameGlobalState> op = FrameGlobalState::Create();

	// set name of op
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	// create shared state, this will be set while running the script and update the shared state
	Ptr<CoreGraphics::ShaderState> state = ShaderServer::Instance()->CreateSharedShaderState("shd:shared", { NEBULAT_FRAME_GROUP });
	op->SetShaderState(state);

	// setup variables
	cJSON* variables = cJSON_GetObjectItem(node, "variables");
	if (variables != NULL)
	{
		IndexT i;
		for (i = 0; i < cJSON_GetArraySize(variables); i++)
		{
			cJSON* var = cJSON_GetArrayItem(variables, i);

			// variables need to define both semantic and value
			cJSON* sem = cJSON_GetObjectItem(var, "semantic");
			n_assert(sem != NULL);
			cJSON* val = cJSON_GetObjectItem(var, "value");
			n_assert(val != NULL);
			Util::String valStr(val->valuestring);

			// get variable
			const Ptr<ShaderVariableInstance>& variable = state->GetVariableByName(sem->valuestring)->CreateInstance();
			switch (variable->GetShaderVariable()->GetType())
			{
			case ShaderVariable::IntType:
				variable->SetInt(valStr.AsInt());
				break;
			case ShaderVariable::FloatType:
				variable->SetFloat(valStr.AsFloat());
				break;
			case ShaderVariable::VectorType:
				variable->SetFloat4(valStr.AsFloat4());
				break;
			case ShaderVariable::Vector2Type:
				variable->SetFloat2(valStr.AsFloat2());
				break;
			case ShaderVariable::MatrixType:
				variable->SetMatrix(valStr.AsMatrix44());
				break;
			case ShaderVariable::BoolType:
				variable->SetBool(valStr.AsBool());
				break;
			case ShaderVariable::SamplerType:
			case ShaderVariable::TextureType:
			{
				const Ptr<Resources::ResourceManager>& resManager = Resources::ResourceManager::Instance();
				if (resManager->HasResource(valStr))
				{
					const Ptr<Resources::Resource>& resource = resManager->LookupResource(valStr);
					variable->SetTexture(resource.downcast<Texture>());
				}
				break;
			}
			case ShaderVariable::ImageReadWriteType:
				variable->SetShaderReadWriteTexture(script->GetReadWriteTexture(valStr));
				break;
			case ShaderVariable::BufferReadWriteType:
				variable->SetShaderReadWriteBuffer(script->GetReadWriteBuffer(valStr));
				break;
			}

			// add variable to op
			op->AddVariableInstance(variable);
		}
	}

	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseBlit(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	Ptr<FrameBlit> op = FrameBlit::Create();

	// set name of op
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseCompute(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	Ptr<FrameCompute> op = FrameCompute::Create();

	// get name of compute sequence
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	// create shader state
	cJSON* shader = cJSON_GetObjectItem(node, "shader");
	n_assert(shader != NULL);
	Ptr<ShaderState> state = ShaderServer::Instance()->CreateShaderState(shader->valuestring, { NEBULAT_DEFAULT_GROUP });
	op->SetShaderState(state);

	// get dimensions, must be 3
	cJSON* dims = cJSON_GetObjectItem(node, "dimensions");
	n_assert(dims != NULL);
	n_assert(cJSON_GetArraySize(dims) == 3);
	op->SetInvocations(cJSON_GetArrayItem(dims, 0)->valueint, cJSON_GetArrayItem(dims, 1)->valueint, cJSON_GetArrayItem(dims, 2)->valueint);

	// parse variables
	cJSON* variables = cJSON_GetObjectItem(node, "variables");
	if (variables != NULL)
	{
		ParseShaderVariables(script, state, variables);
	}

	// add op to script
	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseComputeAlgorithm(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	Ptr<FrameComputeAlgorithm> op = FrameComputeAlgorithm::Create();

	// get function and name
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	cJSON* alg = cJSON_GetObjectItem(node, "algorithm");
	n_assert(alg != NULL);
	cJSON* function = cJSON_GetObjectItem(node, "function");
	n_assert(function != NULL);

	// get algorithm
	const Ptr<Algorithms::Algorithm>& algorithm = script->GetAlgorithm(alg->valuestring);
	op->SetFunction(function->valuestring);

	// add to script
	op->SetAlgorithm(algorithm);
	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseEvent(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	Ptr<FrameEvent> op = FrameEvent::Create();
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	const Ptr<Event>& event = script->GetEvent(name->valuestring);

	// go through ops
	cJSON* ops = cJSON_GetObjectItem(node, "actions");
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(ops); i++)
	{
		cJSON* action = cJSON_GetArrayItem(ops, i);
		Util::String str(action->valuestring);
		FrameEvent::Action a;
		if (str == "wait")			a = FrameEvent::Wait;
		else if (str == "set")		a = FrameEvent::Set;
		else if (str == "reset")	a = FrameEvent::Reset;
		else
		{
			n_error("Event has no operation named '%s'", str.AsCharPtr());
		}

		// add action to op
		op->AddAction(a);
	}

	// set event in op
	op->SetEvent(event);
	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParsePass(const Ptr<Frame2::FrameScript>& script, cJSON* node)
{
	// create pass
	Ptr<FramePass> op = FramePass::Create();
	Ptr<Pass> pass = Pass::Create();
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		Util::String name(cur->string);
		if (name == "name")					op->SetName(cur->valuestring);
		else if (name == "attachments")		ParseAttachmentList(script, pass, cur);
		else if (name == "depthStencil")
		{
			float clearDepth;
			uint clearStencil;
			uint depthStencilClearFlags = 0;
			cJSON* cd = cJSON_GetObjectItem(cur, "clear");
			if (cd != NULL)
			{
				depthStencilClearFlags |= Pass::Clear;
				clearDepth = (float)cd->valuedouble;
			}

			cJSON* cs = cJSON_GetObjectItem(cur, "clearStencil");
			if (cs != NULL)
			{
				depthStencilClearFlags |= Pass::ClearStencil;
				clearStencil = cs->valueint;
			}

			cJSON* ld = cJSON_GetObjectItem(cur, "load");
			if (ld != NULL && ld->valueint == 1)
			{
				// can't really load and store
				n_assert(cd == NULL);
				depthStencilClearFlags |= Pass::Load;
			}

			cJSON* ls = cJSON_GetObjectItem(cur, "loadStencil");
			if (ls != NULL && ls->valueint == 1)
			{
				// can't really load and store
				n_assert(cs == NULL);
				depthStencilClearFlags |= Pass::LoadStencil;
			}

			cJSON* sd = cJSON_GetObjectItem(cur, "store");
			if (sd != NULL && sd->valueint == 1)
			{
				depthStencilClearFlags |= Pass::Store;
			}

			cJSON* ss = cJSON_GetObjectItem(cur, "storeStencil");
			if (ss != NULL && ss->valueint == 1)
			{
				depthStencilClearFlags |= Pass::StoreStencil;
			}

			// set attachment in framebuffer
			pass->SetDepthStencilAttachment(script->GetDepthStencilTexture(cur->valuestring));
			pass->SetDepthStencilClear(clearDepth, clearStencil);
			pass->SetDepthStencilFlags((Pass::AttachmentFlags)depthStencilClearFlags);
		}
		else if (name == "subpass")				ParseSubpass(script, pass, op, cur);
		else
		{
			n_error("Passes don't support operations, and '%s' is no exception.\n", name.AsCharPtr());
		}
	}

	// setup framebuffer and bind to pass
	pass->Setup();
	op->SetPass(pass);
	script->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseAttachmentList(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::Pass>& pass, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		cJSON* name = cJSON_GetObjectItem(cur, "name");
		n_assert(name != NULL);
		const Ptr<RenderTexture>& tex = script->GetColorTexture(name->valuestring);

		// add texture to pass
		pass->AddColorAttachment(tex);

		// set clear flag if present
		cJSON* clear = cJSON_GetObjectItem(cur, "clear");
		uint flags = 0;
		if (clear != NULL)
		{
			Math::float4 clearValue = Util::String(clear->valuestring).AsFloat4();
			pass->SetColorAttachmentClear(i, clearValue);
			flags |= Pass::Clear;
		}

		// set if attachment should store at the end of the pass
		cJSON* store = cJSON_GetObjectItem(cur, "store");
		if (store && store->valueint == 1)
		{
			
			flags |= Pass::Store;
		}

		cJSON* load = cJSON_GetObjectItem(cur, "load");
		if (load && load->valueint == 1)
		{
			// we can't really load and clear
			n_assert(clear == NULL);
			flags |= Pass::Load;
		}
		pass->SetColorAttachmentFlags(i, (Pass::AttachmentFlags)flags);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpass(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::Pass>& pass, const Ptr<Frame2::FramePass>& framePass, cJSON* node)
{
	Ptr<Frame2::FrameSubpass> frameSubpass = Frame2::FrameSubpass::Create();
	Pass::Subpass subpass;
	subpass.resolve = false;
	subpass.bindDepth = false;
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		Util::String name(cur->string);
		if (name == "name")						frameSubpass->SetName(cur->valuestring);
		else if (name == "dependencies")		ParseSubpassDependencies(subpass, cur);
		else if (name == "attachments")			ParseSubpassAttachments(subpass, cur);
		else if (name == "depth")				subpass.bindDepth = cur->valueint == 1 ? true : false;
		else if (name == "resolve")				subpass.resolve = cur->valueint == 1 ? true : false;
		else if (name == "subpassAlgorithm")	ParseSubpassAlgorithm(script, frameSubpass, cur);
		else if (name == "batch")				ParseSubpassBatch(script, frameSubpass, cur);
		else if (name == "sortedBatch")			ParseSubpassSortedBatch(script, frameSubpass, cur);
		else if (name == "fullscreenEffect")	ParseSubpassFullscreenEffect(script, frameSubpass, cur);
		else if (name == "copy")				ParseSubpassCopy(script, frameSubpass, cur);
		else if (name == "event")				ParseSubpassEvent(script, frameSubpass, cur);
		else if (name == "system")				ParseSubpassSystem(script, frameSubpass, cur);
		else
		{
			n_error("Subpass operation '%s' is invalid.\n", name.AsCharPtr());
		}
	}

	// link together frame operations
	pass->AddSubpass(subpass);
	framePass->AddSubpass(frameSubpass);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassDependencies(CoreGraphics::Pass::Subpass& subpass, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		subpass.dependencies.Append(cur->valueint);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassAttachments(CoreGraphics::Pass::Subpass& subpass, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		subpass.attachments.Append(cur->valueint);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassInputs(CoreGraphics::Pass::Subpass& subpass, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* cur = cJSON_GetArrayItem(node, i);
		subpass.inputs.Append(cur->valueint);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassAlgorithm(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassAlgorithm> op = Frame2::FrameSubpassAlgorithm::Create();

	// get function and name
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	cJSON* alg = cJSON_GetObjectItem(node, "algorithm");
	n_assert(alg != NULL);
	cJSON* function = cJSON_GetObjectItem(node, "function");
	n_assert(function != NULL);

	// get algorithm
	const Ptr<Algorithms::Algorithm>& algorithm = script->GetAlgorithm(alg->valuestring);
	op->SetFunction(function->valuestring);

	// add to script
	op->SetAlgorithm(algorithm);
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassBatch(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassBatch> op = Frame2::FrameSubpassBatch::Create();
	Frame::BatchGroup::Code code = Frame::BatchGroup::FromName(node->valuestring);
	op->SetBatchCode(code);
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassSortedBatch(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassOrderedBatch> op = Frame2::FrameSubpassOrderedBatch::Create();
	Frame::BatchGroup::Code code = Frame::BatchGroup::FromName(node->valuestring);
	op->SetBatchCode(code);
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassFullscreenEffect(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassFullscreenEffect> op = Frame2::FrameSubpassFullscreenEffect::Create();

	// get function and name
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);
	
	// create shader state
	cJSON* shader = cJSON_GetObjectItem(node, "shader");
	n_assert(shader != NULL);
	Ptr<ShaderState> state = ShaderServer::Instance()->CreateShaderState(shader->valuestring, { NEBULAT_DEFAULT_GROUP });
	op->SetShaderState(state);

	// get texture
	cJSON* texture = cJSON_GetObjectItem(node, "sizeFromTexture");
	n_assert(texture != NULL);
	Ptr<CoreGraphics::RenderTexture> tex = script->GetColorTexture(texture->valuestring);
	op->SetRenderTexture(tex);

	// parse variables
	cJSON* variables = cJSON_GetObjectItem(node, "variables");
	if (variables != NULL)
	{
		ParseShaderVariables(script, state, variables);
	}
	
	// add op to subpass
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassCopy(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassCopy> op = Frame2::FrameSubpassCopy::Create();

	// get function and name
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	op->SetName(name->valuestring);

	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassEvent(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<FrameEvent> op = FrameEvent::Create();
	cJSON* name = cJSON_GetObjectItem(node, "name");
	n_assert(name != NULL);
	const Ptr<Event>& event = script->GetEvent(name->valuestring);

	// go through ops
	cJSON* ops = cJSON_GetObjectItem(node, "actions");
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(ops); i++)
	{
		cJSON* action = cJSON_GetArrayItem(ops, i);
		Util::String str(action->valuestring);
		FrameEvent::Action a;
		if (str == "wait")			a = FrameEvent::Wait;
		else if (str == "set")		a = FrameEvent::Set;
		else if (str == "reset")	a = FrameEvent::Reset;
		else
		{
			n_error("Event has no operation named '%s'", str.AsCharPtr());
		}

		// add action to op
		op->AddAction(a);
	}

	// set event in op
	op->SetEvent(event);
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseSubpassSystem(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node)
{
	Ptr<Frame2::FrameSubpassSystem> op = Frame2::FrameSubpassSystem::Create();

	Util::String subsystem(node->valuestring);
	if (subsystem == "Lights")				op->SetSubsystem(FrameSubpassSystem::Lights);
	else if (subsystem == "LightProbes")	op->SetSubsystem(FrameSubpassSystem::LightProbes);
	else if (subsystem == "UI")				op->SetSubsystem(FrameSubpassSystem::UI);
	else if (subsystem == "Text")			op->SetSubsystem(FrameSubpassSystem::Text);
	else
	{
		n_error("No subsystem called '%s' exists", subsystem.AsCharPtr());
	}
	subpass->AddOp(op.upcast<Frame2::FrameOp>());
}


//------------------------------------------------------------------------------
/**
*/
void
FrameScriptLoader::ParseShaderVariables(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::ShaderState>& state, cJSON* node)
{
	IndexT i;
	for (i = 0; i < cJSON_GetArraySize(node); i++)
	{
		cJSON* var = cJSON_GetArrayItem(node, i);

		// variables need to define both semantic and value
		cJSON* sem = cJSON_GetObjectItem(var, "semantic");
		n_assert(sem != NULL);
		cJSON* val = cJSON_GetObjectItem(var, "value");
		n_assert(val != NULL);
		Util::String valStr(val->valuestring);

		// get variable
		const Ptr<ShaderVariable>& variable = state->GetVariableByName(sem->valuestring);
		switch (variable->GetType())
		{
		case ShaderVariable::IntType:
			variable->SetInt(valStr.AsInt());
			break;
		case ShaderVariable::FloatType:
			variable->SetFloat(valStr.AsFloat());
			break;
		case ShaderVariable::VectorType:
			variable->SetFloat4(valStr.AsFloat4());
			break;
		case ShaderVariable::Vector2Type:
			variable->SetFloat2(valStr.AsFloat2());
			break;
		case ShaderVariable::MatrixType:
			variable->SetMatrix(valStr.AsMatrix44());
			break;
		case ShaderVariable::BoolType:
			variable->SetBool(valStr.AsBool());
			break;
		case ShaderVariable::SamplerType:
		case ShaderVariable::TextureType:
		{
			const Ptr<Resources::ResourceManager>& resManager = Resources::ResourceManager::Instance();
			if (resManager->HasResource(valStr))
			{
				const Ptr<Resources::Resource>& resource = resManager->LookupResource(valStr);
				variable->SetTexture(resource.downcast<Texture>());
			}
			break;
		}
		case ShaderVariable::ImageReadWriteType:
			variable->SetShaderReadWriteTexture(script->GetReadWriteTexture(valStr));
			break;
		case ShaderVariable::BufferReadWriteType:
			variable->SetShaderReadWriteBuffer(script->GetReadWriteBuffer(valStr));
			break;
		}
	}
}

} // namespace Frame2