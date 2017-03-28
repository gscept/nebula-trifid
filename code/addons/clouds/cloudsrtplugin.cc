//------------------------------------------------------------------------------
//  gridrtplugin.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cloudsrtplugin.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/displaydevice.h"
#include "resources/resourcemanager.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/primitivetopology.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
namespace Clouds
{
__ImplementClass(Clouds::CloudsRTPlugin, 'CLRT', RenderModules::RTPlugin);

//------------------------------------------------------------------------------
/**
*/
CloudsRTPlugin::CloudsRTPlugin() :
	visible(true),
	cloudSize(512),
	cloudThickness(16),
	cloudThickColor(float4(0.9f))
{
	// empty
	this->cloudSettings[0] = { 256, 0.3f, "tex:system/cloudbase.dds" };
	this->cloudSettings[1] = { 768, 0.25f, "tex:system/cloud1.dds" };
	this->cloudSettings[2] = { 128, 0.17f, "tex:system/cloud2.dds" };
}

//------------------------------------------------------------------------------
/**
*/
CloudsRTPlugin::~CloudsRTPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsRTPlugin::OnRegister()
{
	// create new shader
	this->shader = ShaderServer::Instance()->CreateShaderState("shd:clouds", { NEBULAT_DEFAULT_GROUP });
	this->cloudsSizesVar = this->shader->GetVariableByName("CloudSizes");
	this->cloudsLayer1TexVar = this->shader->GetVariableByName("CloudLayer1");
	this->cloudsLayer2TexVar = this->shader->GetVariableByName("CloudLayer2");
	this->cloudsLayer3TexVar = this->shader->GetVariableByName("CloudLayer3");
	this->cloudThicknessesVar = this->shader->GetVariableByName("CloudThicknesses");
	this->cloudThickColorVar = this->shader->GetVariableByName("CloudThickColor");

	// load texture
	IndexT i;
	for (i = 0; i < NumCloudLayers; i++)
	{
		this->textures[i] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, this->cloudSettings[i].tex).downcast<ManagedTexture>();
	}

#define VERTEX_POS 750.0f
	// setup VBO
	Util::Array<VertexComponent> components;
	components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float2, 0));
	float verts[] = { -VERTEX_POS, -VERTEX_POS, VERTEX_POS, -VERTEX_POS, VERTEX_POS, VERTEX_POS, -VERTEX_POS, VERTEX_POS };
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(components, 4, verts, sizeof(verts), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	
	this->vbo = VertexBuffer::Create();
	this->vbo->SetLoader(vboLoader.upcast<ResourceLoader>());
	this->vbo->SetAsyncEnabled(false);
	this->vbo->Load();
	n_assert(this->vbo->IsLoaded());
	this->vbo->SetLoader(NULL);	

	short indices[] = { 0, 1, 2, 2, 3, 0 };
	Ptr<MemoryIndexBufferLoader> iboLoader = MemoryIndexBufferLoader::Create();
	iboLoader->Setup(IndexType::Index16, 6, indices, sizeof(indices));

	this->ibo = IndexBuffer::Create();
	this->ibo->SetLoader(iboLoader.upcast<ResourceLoader>());
	this->ibo->SetAsyncEnabled(false);
	this->ibo->Load();
	n_assert(this->ibo->IsLoaded());
	this->ibo->SetLoader(NULL);	
	
	this->primitive.SetBaseIndex(0);
	this->primitive.SetNumVertices(4);
	this->primitive.SetBaseIndex(0);
	this->primitive.SetNumIndices(6);

	// get batch group
	this->group = Graphics::BatchGroup::FromName("Foreground");
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsRTPlugin::OnUnregister()
{
	IndexT i;
	for (i = 0; i < NumCloudLayers; i++)
	{
		ResourceManager::Instance()->DiscardManagedResource(this->textures[i].upcast<ManagedResource>());
		this->textures[i] = 0;
	}
	
	this->ibo->Unload();
	this->ibo = 0;
	this->vbo->Unload();
	this->vbo = 0;

	this->cloudsSizesVar = 0;
	this->cloudThicknessesVar = 0;
	this->cloudsLayer1TexVar = 0;
	this->cloudsLayer2TexVar = 0;
	this->shader = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsRTPlugin::OnRender(const Util::StringAtom& filter)
{
	if (filter == "Skybox" && this->visible)
	{
		Ptr<RenderDevice> device = RenderDevice::Instance();
		Ptr<TransformDevice> trans = TransformDevice::Instance();

		// start pass
        this->shader->Apply();

		// set variables
		this->cloudsLayer1TexVar->SetTexture(this->textures[0]->GetTexture());
		this->cloudsLayer2TexVar->SetTexture(this->textures[1]->GetTexture());
		this->cloudsLayer3TexVar->SetTexture(this->textures[2]->GetTexture());
		this->cloudsSizesVar->SetFloat4(float4(this->cloudSettings[0].size, this->cloudSettings[1].size, this->cloudSettings[2].size, 0));
		this->cloudThicknessesVar->SetFloat4(float4(this->cloudSettings[0].thickness, this->cloudSettings[1].thickness, this->cloudSettings[2].thickness, 0));
		this->cloudThickColorVar->SetFloat4(this->cloudThickColor);
		this->shader->Commit();

		device->SetVertexLayout(this->vbo->GetVertexLayout());
		device->SetPrimitiveTopology(CoreGraphics::PrimitiveTopology::TriangleList);
		device->SetPrimitiveGroup(this->primitive);
		device->SetStreamVertexBuffer(0, this->vbo, 0);
		device->SetIndexBuffer(this->ibo);
		device->DrawIndexedInstanced(2, 0);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsRTPlugin::SetCloudSettings(const CloudSettings& settings, IndexT index)
{
	// if we swap textures, make sure to unload old and load new
	if (this->cloudSettings[index].tex != settings.tex)
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->textures[index].upcast<ManagedResource>());
		this->textures[index] = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, settings.tex).downcast<ManagedTexture>();
	}
	this->cloudSettings[index] = settings;
}

} // namespace Grid