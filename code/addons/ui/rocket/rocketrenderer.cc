//------------------------------------------------------------------------------
//  rocketrenderer.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/renderdevice.h"
#include "resources/managedtexture.h"
#include "resources/resourcemanager.h"
#include "coregraphics/memorytextureloader.h"
#include "coregraphics/texture.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/vertexlayoutserver.h"
#include "rocketserver.h"
#include "SOIL/SOIL.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
namespace LibRocket
{

//------------------------------------------------------------------------------
/**
*/
RocketRenderer::RocketRenderer()
{
	// get shader server
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();

	// set render device
	this->renderDevice = RenderDevice::Instance();

	// get shader and create instance
	const Ptr<Shader> shader = shaderServer->GetShader("shd:gui");
	this->shaderInstance = shader->CreateShaderInstance();

	// get texture
	this->diffMap = this->shaderInstance->GetVariableByName("Texture");	
	this->modelVar = this->shaderInstance->GetVariableByName("Model");

	// we get the white texture for when we don't have a diffuse map, this will be then be our default
	this->whiteTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, "tex:system/white.dds").downcast<ManagedTexture>();
}

//------------------------------------------------------------------------------
/**
*/
RocketRenderer::~RocketRenderer()
{
	this->renderDevice = 0;
	this->shaderInstance->Discard();
	this->shaderInstance = 0;
	this->diffMap = 0;
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::CompiledGeometryHandle 
RocketRenderer::CompileGeometry( Rocket::Core::Vertex* vertices, 
								 int num_vertices, 
								 int* indices, 
								 int num_indices, 
								 Rocket::Core::TextureHandle texture )
{	
	// create geometry handle
	NebulaCompiledGeometry* geometry = (NebulaCompiledGeometry*)Alloc(Memory::RocketHeap, sizeof(NebulaCompiledGeometry));
	Memory::Clear(geometry, sizeof(NebulaCompiledGeometry));

	// set texture
	NebulaTexture* nebTex = (NebulaTexture*)texture;
	if (nebTex)
	{
		geometry->texture = nebTex->tex;
	}	

	// create vertex buffer
	geometry->vb = VertexBuffer::Create();

	Util::Array<VertexComponent> vertexComponents;
	vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float2));
    vertexComponents.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::UByte4N));	
	vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));
		
	// create loader for vertex buffer
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents, 
					num_vertices, 
					vertices, 
					num_vertices * sizeof(float) * 4 + num_vertices * sizeof(unsigned char) * 4,
					Base::ResourceBase::UsageImmutable, 
					Base::ResourceBase::AccessNone);
	geometry->vb->SetLoader(vbLoader.upcast<ResourceLoader>());
	geometry->vb->SetAsyncEnabled(false);
	geometry->vb->Load();
	n_assert(geometry->vb->IsLoaded());
	geometry->vb->SetLoader(0);

    /*
	// map buffer
	NebulaVertex* verts = (NebulaVertex*)geometry->vb->Map(Base::ResourceBase::MapWriteDiscard);
	IndexT i;
	for (i = 0; i < num_vertices; i++)
	{
		verts[i].x = vertices[i].position.x;
		verts[i].y = vertices[i].position.y;

		verts[i].u = vertices[i].tex_coord[0];
		verts[i].v = vertices[i].tex_coord[1];		

		verts[i].r = vertices[i].colour.red / 255.f;	
		verts[i].g = vertices[i].colour.green / 255.f;
		verts[i].b = vertices[i].colour.blue / 255.f;
		verts[i].a = vertices[i].colour.alpha / 255.f;
	}
	geometry->vb->Unmap();
    */
	
	// create index buffer
	geometry->ib = IndexBuffer::Create();

	// create loader for index buffer
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index32, 
					num_indices, 
					indices, 
					num_indices * sizeof(int), 
					Base::ResourceBase::UsageImmutable, 
					Base::ResourceBase::AccessNone);
	geometry->ib->SetLoader(ibLoader.upcast<ResourceLoader>());
	geometry->ib->SetAsyncEnabled(false);
	geometry->ib->Load();
	n_assert(geometry->ib->IsLoaded());
	geometry->ib->SetLoader(0);

	geometry->primGroup.SetBaseIndex(0);
	geometry->primGroup.SetNumVertices(num_vertices);
	geometry->primGroup.SetBaseVertex(0);
	geometry->primGroup.SetNumIndices(num_indices);
	geometry->primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	
	return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geometry);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::RenderGeometry( Rocket::Core::Vertex* vertices, 
							   int num_vertices, 
							   int* indices, 
							   int num_indices, 
							   Rocket::Core::TextureHandle texture, 
							   const Rocket::Core::Vector2f& translation )
{
	// we don't handle rendering spontaneous geometry
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::RenderCompiledGeometry( Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation )
{		
	Ptr<RenderDevice> device = RenderDevice::Instance();
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();
	NebulaCompiledGeometry* nebGeometry = (NebulaCompiledGeometry*)geometry;

	// only render if texture is loaded
	if (nebGeometry)
	{
       	if (nebGeometry->texture.isvalid())
		{
			// set texture
			this->diffMap->SetTexture((Texture*)nebGeometry->texture);
		}
		else
		{
			this->diffMap->SetTexture(this->whiteTexture->GetTexture());
		}

		// get dimensions
		Rocket::Core::Vector2i dimensions = RocketServer::Instance()->GetContext()->GetDimensions();

		// create and set transform (this is so weird...)
		matrix44 world = matrix44::translation(float4(translation.x, translation.y, 0, 1));
		matrix44 scale = matrix44::scaling(float4(2.0f/(float)dimensions.x, -2.0f/(float)dimensions.y, 1, 1));
		matrix44 trans = matrix44::translation(float4(-1, 1, 0, 0));

		// combine matrices and set in shader
		world = matrix44::multiply(matrix44::multiply(world, scale), trans);
		this->modelVar->SetMatrix(world);

		// start pass
		IndexT passes = this->shaderInstance->Begin();
		n_assert(passes == 1);
		this->shaderInstance->BeginPass(0);

		// apply shader
		shaderServer->SetActiveShaderInstance(this->shaderInstance);

		// commit shader
		this->shaderInstance->Commit();

		// setup render device and draw
		device->SetVertexLayout(nebGeometry->vb->GetVertexLayout());
		device->SetIndexBuffer(nebGeometry->ib);
		device->SetStreamSource(0, nebGeometry->vb, 0);
		device->SetPrimitiveGroup(nebGeometry->primGroup);
		device->Draw();

        this->shaderInstance->PostDraw();

		// end pass
		this->shaderInstance->EndPass();
		this->shaderInstance->End();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::ReleaseCompiledGeometry( Rocket::Core::CompiledGeometryHandle geometry )
{	
	NebulaCompiledGeometry* nebGeometry = (NebulaCompiledGeometry*)geometry;			
	nebGeometry->ib->Unload();
	nebGeometry->ib = 0;		
	nebGeometry->vb->Unload();
	nebGeometry->vb = 0;
	nebGeometry->texture = 0;
	
	Memory::Free(Memory::RocketHeap, nebGeometry);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::EnableScissorRegion( bool enable )
{
	if (enable)
	{
		this->shaderInstance->SelectActiveVariation(ShaderServer::Instance()->FeatureStringToMask("Static|Alt0"));
	}
	else
	{
		this->shaderInstance->SelectActiveVariation(ShaderServer::Instance()->FeatureStringToMask("Static"));
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::SetScissorRegion( int x, int y, int width, int height )
{
	Ptr<RenderDevice> device = RenderDevice::Instance();
	this->scissor.set(x, y, x + width, y + height);

	// set scissor rect
	device->SetScissorRect(this->scissor, 0);
}

//------------------------------------------------------------------------------
/**
*/
bool 
RocketRenderer::LoadTexture( Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source )
{
	int width, height, channels;
	unsigned char* data = SOIL_load_image(source.CString(), &width, &height, &channels, SOIL_LOAD_RGBA);
    if(data == NULL)
    {
        return false;
    }
	texture_dimensions.x = width;
	texture_dimensions.y = height;

	// generate texture from image
	this->GenerateTexture(texture_handle, (const ubyte*)data, texture_dimensions);
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
RocketRenderer::GenerateTexture( Rocket::Core::TextureHandle& texture_handle, const ubyte* source, const Rocket::Core::Vector2i& source_dimensions )
{
	// create texture
	Ptr<Texture> texture = Texture::Create();
	Ptr<MemoryTextureLoader> loader = MemoryTextureLoader::Create();
    loader->SetImageBuffer(source, source_dimensions.x, source_dimensions.y, PixelFormat::SRGBA8);
	texture->SetLoader(loader.upcast<ResourceLoader>());
	texture->SetAsyncEnabled(false);
	texture->Load();
	n_assert(texture->IsLoaded());
	texture->SetLoader(0);

	// create intermediate texture
	NebulaTexture* tex = new NebulaTexture;
	tex->tex = texture;

	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(tex);
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketRenderer::ReleaseTexture( Rocket::Core::TextureHandle texture )
{
	NebulaTexture* tex = (NebulaTexture*)texture;
	tex->tex->Unload();
	tex->tex = 0;
	delete tex;
}

} // namespace LibRocket