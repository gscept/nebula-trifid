#pragma once
//------------------------------------------------------------------------------
/**
    @class Minimap::MinimapManager      
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "application/game/entity.h"
#include "minimapplugin.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"



//------------------------------------------------------------------------------
namespace Minimap
{
class MinimapManager : public Game::Manager
{
	__DeclareClass(MinimapManager);
	__DeclareSingleton(MinimapManager);
public:
	/// constructor
	MinimapManager();
	/// destructor
	virtual ~MinimapManager();

    /// called when attached to game server
    virtual void OnActivate();

    /// called when removed from game server
    virtual void OnDeactivate();
    /// called before frame by the game server
    virtual void OnBeginFrame();

	/// register an entity to the minimap
	void RegisterEntity(const Util::String& texture, const Ptr<Game::Entity>& entity);
	/// unregister an entity from the minimap
	void UnregisterEntity(const Util::String& texture, const Ptr<Game::Entity>& entity);
	
	/// set minimap world size
	void SetWorldSize(const Math::bbox & box);
	///
	void SetBackgroundTexture(const Util::String& texture);
	

	static const uint MinimapResolutionX = 512;
	static const uint MinimapResolutionY = 512;
	static const uint MaxNumIconsPerBatch = 256;
		 	
private:              

	friend Minimap::MinimapPlugin;
    // shading related variables
	Ptr<CoreGraphics::RenderTarget> minimapTarget;
	Ptr<CoreGraphics::Shader> minimapShader;
	Ptr<CoreGraphics::ShaderVariable> transformsVar;
    Ptr<CoreGraphics::ShaderVariable> portraitVar;
    Ptr<CoreGraphics::ShaderVariable> portraitScalesVar;
    Ptr<CoreGraphics::ShaderVariable> colorsVar;

    // mesh related variables
	Ptr<CoreGraphics::VertexBuffer> quadVb;
	Ptr<CoreGraphics::IndexBuffer> quadIb;
    Ptr<CoreGraphics::VertexLayout> vertexLayout;
	CoreGraphics::PrimitiveGroup quadPrim;

    Util::FixedArray<Math::float4> colors;
    Util::FixedArray<Math::float2> iconSizes;
    Util::FixedArray<Math::matrix44> transforms;

	Math::bbox minimapBox;

	Util::HashTable<Util::String, Util::Array<Ptr<Game::Entity>>> entities;
	Util::HashTable<Util::String, Ptr<Resources::ManagedTexture>> texturePool;
	Util::Array<Util::String> textures;	
	Util::String backgroundName;
	Ptr<Resources::ManagedTexture> backgroundTexture;

	Minimap::MinimapPlugin* plugin;	
};

} // namespace Minimap
