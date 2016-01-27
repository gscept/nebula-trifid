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
MinimapManager::MinimapManager()
{
	__ConstructSingleton;	
}

//------------------------------------------------------------------------------
/**
*/
MinimapManager::~MinimapManager()
{
	__DestructSingleton;	
}

//------------------------------------------------------------------------------
/**
*/
void 
MinimapManager::OnActivate()
{
	this->plugin = Minimap::MinimapPlugin::Create();
	UiFeatureUnit::Instance()->RegisterUIRenderPlugin(this->plugin.cast<UI::UiPlugin>());

    // setup render target
    this->minimapTarget = RenderTarget::Create();
    this->minimapTarget->SetWidth(512);
    this->minimapTarget->SetHeight(512);
    this->minimapTarget->SetAntiAliasQuality(AntiAliasQuality::None);
    this->minimapTarget->SetColorBufferFormat(PixelFormat::A8R8G8B8);
    this->minimapTarget->Setup();

    // resize transforms array
    this->transforms.Resize(this->MaxNumIconsPerBatch);
    this->colors.Resize(this->MaxNumIconsPerBatch);
    this->iconSizes.Resize(this->MaxNumIconsPerBatch);

	// create shader
	this->minimapShader = ShaderServer::Instance()->GetShader("minimap");
	this->transformsVar = this->minimapShader->GetVariableByName("ModelArray");
    this->portraitVar = this->minimapShader->GetVariableByName("Portrait");
    this->portraitScalesVar = this->minimapShader->GetVariableByName("PortraitScaleArray");
    this->colorsVar = this->minimapShader->GetVariableByName("ColorArray");

    // setup vertex and index buffers
    Array<VertexComponent> cornerComponents;
    cornerComponents.Append(VertexComponent(VertexComponent::TexCoord1, 0, VertexComponent::Float2, 0));
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

	UiFeatureUnit::Instance()->UnregisterUIRenderPlugin(this->plugin.cast<UI::UiPlugin>());
	this->plugin = 0;
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

	// iterate through textures 
	IndexT i;
	for (i = 0; i < this->textures.Size(); i++)
	{
		// get texture at resource, this may or may not be placeholder, but we don't really care
		const Ptr<Texture>& tex = this->texturePool[this->textures[i]]->GetTexture();

		// get entity list
		const Util::Array<Ptr<Game::Entity>>& ents = this->entities[this->textures[i]];

		// get reference for minimap
		const vector& size = this->minimapBox.size();

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
                const float4& pos = ent->GetMatrix44(Attr::Transform).get_position();
                const float4& iconSize = ent->GetFloat4(Attr::MinimapIconSize);
                const float4& color = ent->GetFloat4(Attr::MinimapIconColor);

                // calculate map relative coordinates
                float x = (pos.x() + size.x() * 0.5f) / size.x() * this->MinimapResolutionX;
                float z = (pos.z() + size.z() * 0.5f) / size.z() * this->MinimapResolutionY;

                // create transform matrix, 0 z, and x, z will be mapped to x, y
                matrix44 trans = matrix44::translation(x, z, 0);

                this->colors[numBatchEntities] = color;
                this->transforms[numBatchEntities] = trans;
                this->iconSizes[numBatchEntities] = float2(iconSize.x(), iconSize.y());

                numBatchEntities++;
            }

            // update variables
            this->transformsVar->SetMatrixArray(&this->transforms[0], numBatchEntities);
            this->colorsVar->SetFloat4Array(&this->colors[0], numBatchEntities);
            this->portraitScalesVar->SetFloat2Array(&this->iconSizes[0], numBatchEntities);
            this->portraitVar->SetTexture(tex);

            // start batch
            renderDev->BeginFrame();
            renderDev->BeginPass(this->minimapTarget, this->minimapShader);

            // setup primitive
            renderDev->SetStreamSource(0, this->quadVb, 0);
            renderDev->SetIndexBuffer(this->quadIb);
            renderDev->SetPrimitiveGroup(this->quadPrim);

            // draw
            renderDev->DrawIndexedInstanced(numBatchEntities, 0);

            renderDev->EndPass();
            renderDev->EndFrame();
        }
	}
}

} // namespace Minimap
