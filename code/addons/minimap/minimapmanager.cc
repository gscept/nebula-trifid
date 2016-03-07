//------------------------------------------------------------------------------
//  minimap/minimapnmanager.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "addons/minimap/minimapmanager.h"
#include "minimapplugin.h"
#include "ui/uifeatureunit.h"
#include "coregraphics/shaderserver.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "minimap.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "rocket/elementminimap.h"
#include "coregraphics/vertexbuffer.h"
#include "resources/resource.h"
#include "resources/resourcemanager.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"







namespace Rocket {
namespace MiniMap {


//------------------------------------------------------------------------------
/**
*/
void RegisterElementInstancers()
{
    Core::ElementInstancer* instancer = new Core::ElementInstancerGeneric<ElementMiniMap>();
    Core::Factory::RegisterElementInstancer("minimap", instancer);
    instancer->RemoveReference();
}

static bool initialised = false;


//------------------------------------------------------------------------------
/**
*/
class MiniMapPlugin : public Rocket::Core::Plugin
{
public:
	void OnShutdown()
	{
		initialised = false;
		delete this;
	}

	int GetEventClasses()
	{
		return Rocket::Core::Plugin::EVT_BASIC;
	}
};


//------------------------------------------------------------------------------
/**
*/
void Initialise()
{
	// Prevent double initialisation
	if (!initialised)
	{
		RegisterElementInstancers();

		// Register the progress bar plugin, so we'll be notified on Shutdown
		Rocket::Core::RegisterPlugin(new MiniMapPlugin());

		initialised = true;
	}
}
}
}
 
namespace Minimap
{
__ImplementClass(Minimap::MinimapManager, 'MMPR', Game::Manager);
__ImplementSingleton(Minimap::MinimapManager);

using namespace CoreGraphics;
using namespace Util;
using namespace Math;
using namespace UI;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
MinimapManager::MinimapManager():
	plugin(NULL)
{
	__ConstructSingleton;	
    this->plugin = n_new(MinimapPlugin);
    UiFeatureUnit::Instance()->RegisterUIRenderPlugin(this->plugin);
}

//------------------------------------------------------------------------------
/**
*/
MinimapManager::~MinimapManager()
{
    UiFeatureUnit::Instance()->UnregisterUIRenderPlugin(this->plugin);
    n_delete(this->plugin);
    this->plugin = 0;
	__DestructSingleton;	
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::OnActivate()
{
//	this->plugin = Minimap::MinimapPlugin::Create();

    // setup render target
    this->minimapTarget = RenderTarget::Create();
    this->minimapTarget->SetWidth(512);
    this->minimapTarget->SetHeight(512);
    this->minimapTarget->SetAntiAliasQuality(AntiAliasQuality::None);
    this->minimapTarget->SetColorBufferFormat(PixelFormat::A8R8G8B8);
	this->minimapTarget->SetResolveTextureResourceId("Minimap");
	this->minimapTarget->SetClearColor(float4(0, 0, 0, 0));
	this->minimapTarget->SetClearFlags(Base::RenderTargetBase::ClearColor);
    this->minimapTarget->Setup();

    // resize transforms array
    this->transforms.Resize(this->MaxNumIconsPerBatch);
    this->colors.Resize(this->MaxNumIconsPerBatch);
    this->iconSizes.Resize(this->MaxNumIconsPerBatch);

	// create shader
	this->minimapShader = ShaderServer::Instance()->GetShader("shd:minimap");
	this->transformsVar = this->minimapShader->GetVariableByName("TransArray");
    this->portraitVar = this->minimapShader->GetVariableByName("Portrait");    
    this->colorsVar = this->minimapShader->GetVariableByName("ColorArray");

    // setup vertex and index buffers
    Array<VertexComponent> cornerComponents;
    cornerComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float2, 0));
    float cornerVertexData[] = { 0, 0,  1, 0,  1, 1,  0, 1 };
    Ptr<MemoryVertexBufferLoader> cornerVBLoader = MemoryVertexBufferLoader::Create();
    cornerVBLoader->Setup(cornerComponents, 4, cornerVertexData, sizeof(cornerVertexData), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);

    this->quadVb = VertexBuffer::Create();
    this->quadVb->SetLoader(cornerVBLoader.upcast<ResourceLoader>());
    this->quadVb->SetAsyncEnabled(false);
    this->quadVb->Load();
    if (!this->quadVb->IsLoaded())
    {
        n_error("MinimapManager: Failed to setup quad vertex buffer!");
    }
    this->quadVb->SetLoader(0);

    // setup the corner index buffer
    ushort cornerIndexData[] = { 0, 1, 2, 2, 3, 0 };
    Ptr<MemoryIndexBufferLoader> cornerIBLoader = MemoryIndexBufferLoader::Create();
    cornerIBLoader->Setup(IndexType::Index16, 6, cornerIndexData, sizeof(cornerIndexData), IndexBuffer::UsageImmutable, IndexBuffer::AccessNone);

    this->quadIb = IndexBuffer::Create();
    this->quadIb->SetLoader(cornerIBLoader.upcast<ResourceLoader>());
    this->quadIb->SetAsyncEnabled(false);
    this->quadIb->Load();
    if (!this->quadIb->IsLoaded())
    {
        n_error("MinimapManager: Failed to setup quad index buffer!");
    }
    this->quadIb->SetLoader(0);

    // setup vertex layout
    this->vertexLayout = this->quadVb->GetVertexLayout();

    // setup the cornerPrimitiveGroup which describes one particle instance
    this->quadPrim.SetBaseVertex(0);
    this->quadPrim.SetNumVertices(4);
    this->quadPrim.SetBaseIndex(0);
    this->quadPrim.SetNumIndices(6);
    this->quadPrim.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	Rocket::MiniMap::Initialise();

	if(!this->backgroundName.IsEmpty())
	{
		this->backgroundTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, this->backgroundName, 0, true).downcast<ManagedTexture>();
		ResourceManager::Instance()->RequestResourceForLoading(this->backgroundTexture.upcast<ManagedResource>());
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::OnDeactivate()
{	
    // cleanup target
    this->minimapTarget->Discard();
    this->minimapTarget = 0;

    // cleanup shader variables
    this->transformsVar = 0;
    this->portraitVar = 0;
    this->portraitScalesVar = 0;
    this->colorsVar = 0;

    // discard shader
    this->minimapShader = 0;

    // unload buffers
    this->quadVb->Unload();
    this->quadVb = 0;

    this->quadIb->Unload();
    this->quadIb = 0;

    // clear arrays
    this->transforms.Clear();
    this->colors.Clear();
    this->iconSizes.Clear();
	
//	this->plugin = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::RegisterEntity( const Util::String& texture, const Ptr<Game::Entity>& entity )
{
	if (this->entities.Contains(texture))
	{
		this->entities[texture].Append(entity);
	}
	else
	{
		this->entities.Add(texture, Util::Array<Ptr<Game::Entity>>());
		this->entities[texture].Append(entity);
		if (!texture.IsEmpty())
		{
			Ptr<ManagedTexture> managedTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, texture, 0, true).downcast<ManagedTexture>();
			ResourceManager::Instance()->RequestResourceForLoading(managedTexture.upcast<ManagedResource>());
			this->texturePool.Add(texture,managedTexture);
			this->textures.Append(texture);
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::UnregisterEntity( const Util::String& texture, const Ptr<Game::Entity>& entity )
{
	n_assert(this->entities.Contains(texture));
	IndexT index = this->entities[texture].FindIndex(entity);
	n_assert(InvalidIndex != index);
	this->entities[texture].EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::OnBeginFrame()
{
	const Ptr<CoreGraphics::RenderDevice>& renderDev = CoreGraphics::RenderDevice::Instance();
	renderDev->BeginFrame(InvalidIndex);
	renderDev->BeginPass(this->minimapTarget, this->minimapShader);
	// render background	
	if(this->backgroundTexture.isvalid())
	{
		Math::matrix44 trans;
		trans.translate(float4(-0.5f, -0.5f, 0.0f, 0.0f));
		trans.scale(float4(2.0f, 2.0f, 1.0f, 1.0f));
		this->transforms[0] = trans;
		ShaderServer::Instance()->SetActiveShader(this->minimapShader);
		this->minimapShader->Apply();
		// update variables
		this->minimapShader->BeginUpdate();
		this->transformsVar->SetMatrixArray(&this->transforms[0], 1);
		this->colorsVar->SetFloat4Array(&this->colors[0], 1);
		this->portraitVar->SetTexture(this->backgroundTexture->GetTexture());
		this->minimapShader->EndUpdate();
		this->minimapShader->Commit();

		// start batch		
		

		// setup primitive
		renderDev->SetVertexLayout(this->quadVb->GetVertexLayout());
		renderDev->SetStreamSource(0, this->quadVb, 0);
		renderDev->SetIndexBuffer(this->quadIb);
		renderDev->SetPrimitiveGroup(this->quadPrim);

		// draw
		renderDev->DrawIndexedInstanced(1, 0);
		
	}


	// iterate through textures 
	IndexT i;
	for (i = 0; i < this->textures.Size(); i++)
	{
		// get texture at resource, this may or may not be placeholder, but we don't really care
		const Ptr<Texture>& tex = this->texturePool[this->textures[i]]->GetTexture();

		// get entity list
		const Util::Array<Ptr<Game::Entity>>& ents = this->entities[this->textures[i]];

		// get reference for minimap
		const vector& size = this->minimapBox.extents();

        // get number of entities
        SizeT numEntities = ents.Size();

#define DivAndRoundUp(x, y) (x % y == 0) ? (x / y) : (x / y + 1)

        SizeT batchSize = DivAndRoundUp(numEntities, this->MaxNumIconsPerBatch);

        // run batches
        IndexT batchIndex;
        for (batchIndex = 0;  batchIndex < batchSize; batchIndex++)
        {
            IndexT numBatchEntities = 0;

            // update and render entities
            IndexT j;
            for (j = batchIndex * this->MaxNumIconsPerBatch; j < numEntities; j++)
            {
                const Ptr<Game::Entity>& ent = ents[j];
				Ptr<GraphicsFeature::GetGraphicsVisible> vmsg = GraphicsFeature::GetGraphicsVisible::Create();
				__SendSync(ent,vmsg);
				if (!vmsg->GetVisible())
				{
					continue;
				}
                const float4& pos = ent->GetMatrix44(Attr::Transform).get_position();
                const float4& iconSize = ent->GetFloat4(Attr::MinimapIconSize);
                const float4& color = ent->GetFloat4(Attr::MinimapIconColor);

                // calculate map relative coordinates
				Math::matrix44 trans;
				trans.scale(iconSize);
				float x = (pos.x() ) / size.x();
				float z = (pos.z() ) / size.z();
				trans.translate(float4(x, z, 0, 0));
                
				this->transforms[numBatchEntities] = trans;
                this->colors[numBatchEntities] = color;                
                this->iconSizes[numBatchEntities] = float2(iconSize.x(), iconSize.y());

                numBatchEntities++;
            }
            if (numBatchEntities)
            {
                ShaderServer::Instance()->SetActiveShader(this->minimapShader);
                this->minimapShader->Apply();
                // update variables
                this->minimapShader->BeginUpdate();
                this->transformsVar->SetMatrixArray(&this->transforms[0], numBatchEntities);
                this->colorsVar->SetFloat4Array(&this->colors[0], numBatchEntities);
                this->portraitVar->SetTexture(tex);
                this->minimapShader->EndUpdate();
                this->minimapShader->Commit();

                // setup primitive
                renderDev->SetVertexLayout(this->quadVb->GetVertexLayout());
                renderDev->SetStreamSource(0, this->quadVb, 0);
                renderDev->SetIndexBuffer(this->quadIb);
                renderDev->SetPrimitiveGroup(this->quadPrim);

                // draw
                renderDev->DrawIndexedInstanced(numBatchEntities, 0);
            }
        }
	}
	renderDev->EndPass();
	renderDev->EndFrame(InvalidIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapManager::SetWorldSize(const Math::bbox & box)
{
	this->minimapBox = box;
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapManager::SetBackgroundTexture(const Util::String& texture)
{
	if (this->backgroundName != texture)
	{
		this->backgroundName = texture;
		this->backgroundTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, this->backgroundName, 0, true).downcast<ManagedTexture>();
		ResourceManager::Instance()->RequestResourceForLoading(this->backgroundTexture.upcast<ManagedResource>());	
	}	
}

} // namespace Minimap
