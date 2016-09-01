#pragma once
//------------------------------------------------------------------------------
/**
	Loads a frame shader from JSON file
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "framescript.h"
#include "framepass.h"
#include "framesubpass.h"
#include "resources/resourceid.h"
#include "io/uri.h"
#include "cjson/cJSON/cJSON.h"
#include "coregraphics/shaderstate.h"

namespace Frame2
{
class FrameScriptLoader
{
public:
	static Ptr<FrameScript> LoadFrameScript(const IO::URI& path);
private:
	/// main parse function
	static void ParseFrameScript(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse color texture list
	static void ParseColorTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse depth-stencil texture list
	static void ParseDepthStencilTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse image read-write texture list
	static void ParseImageReadWriteTextureList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse image read-write buffer list
	static void ParseImageReadWriteBufferList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse algorithm list
	static void ParseAlgorithmList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse event list
	static void ParseEventList(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse global state
	static void ParseGlobalState(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse blit
	static void ParseBlit(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse compute
	static void ParseCompute(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse compute algorithm
	static void ParseComputeAlgorithm(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse event
	static void ParseEvent(const Ptr<Frame2::FrameScript>& script, cJSON* node);

	/// parse pass
	static void ParsePass(const Ptr<Frame2::FrameScript>& script, cJSON* node);
	/// parse attachment list
	static void ParseAttachmentList(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::Pass>& framebuffer, cJSON* node);
	/// parse subpass
	static void ParseSubpass(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::Pass>& pass, const Ptr<Frame2::FramePass>& framePass, cJSON* node);
	/// parse subpass dependencies
	static void ParseSubpassDependencies(CoreGraphics::Pass::Subpass& subpass, cJSON* node);
	/// parse subpass dependencies
	static void ParseSubpassAttachments(CoreGraphics::Pass::Subpass& subpass, cJSON* node);
	/// parse subpass inputs
	static void ParseSubpassInputs(CoreGraphics::Pass::Subpass& subpass, cJSON* node);
	/// parse subpass algorithm
	static void ParseSubpassAlgorithm(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse subpass batch
	static void ParseSubpassBatch(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse subpass sorted batch
	static void ParseSubpassSortedBatch(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse subpass post effect
	static void ParseSubpassFullscreenEffect(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse subpass copy
	static void ParseSubpassCopy(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse event in subpass
	static void ParseSubpassEvent(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);
	/// parse system in subpass
	static void ParseSubpassSystem(const Ptr<Frame2::FrameScript>& script, const Ptr<Frame2::FrameSubpass>& subpass, cJSON* node);

	/// helper to parse shader variables
	static void ParseShaderVariables(const Ptr<Frame2::FrameScript>& script, const Ptr<CoreGraphics::ShaderState>& state, cJSON* node);
};
} // namespace Frame2
