#include "stdneb.h"
#include "elementminimap.h"
#include <Rocket/Core/GeometryUtilities.h>
#include <Rocket/Core/ElementDocument.h>

namespace Rocket
{
	using namespace Core;
namespace MiniMap
{

	
//------------------------------------------------------------------------------
/**
*/
ElementMiniMap::ElementMiniMap(const Rocket::Core::String& tag) :
	Core::Element(tag), 
	geometry(this),
	geometry_dirty(true),
	texture(NULL),
	overlay(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
ElementMiniMap::~ElementMiniMap()
{
	if (this->texture)
	{
		n_delete(this->texture);
	}
	if (this->overlay)
	{
		n_delete(this->overlay);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ElementMiniMap::OnRender()
{
	if (this->geometry_dirty)
	{
		GenerateGeometry();
	}
		
	this->geometry.Render(GetAbsoluteOffset(Rocket::Core::Box::CONTENT));	
	if (this->overlay)
	{
		this->overlayGeometry.Render(GetAbsoluteOffset(Rocket::Core::Box::CONTENT));
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ElementMiniMap::GenerateGeometry()
{


	// Release the old geometry before specifying the new vertices.
	this->overlayGeometry.Release(true);
	if (!this->overlay)
	{

		const Rocket::Core::Property* prop = this->GetProperty("minimapoverlay");
		if (prop)
		{
			this->overlay = n_new(Rocket::Core::Texture);
			Rocket::Core::String target = prop->Get<String>();            
			this->overlay->Load(target.CString(), this->GetOwnerDocument()->GetSourceURL());
		}
	}
	if (this->overlay)
	{	
		std::vector< Rocket::Core::Vertex >& vertices = this->overlayGeometry.GetVertices();
		std::vector< int >& indices = this->overlayGeometry.GetIndices();

		vertices.resize(4);
		indices.resize(6);

		// Generate the texture coordinates.
		Vector2f texcoords[2];
		texcoords[0] = Vector2f(0.0f, 0.0f);
		texcoords[1] = Vector2f(1.0f, 1.0f);


		Rocket::Core::GeometryUtilities::GenerateQuad(&vertices[0],									// vertices to write to
			&indices[0],									// indices to write to
			Vector2f(0, 0),					// origin of the quad
			GetBox().GetSize(Rocket::Core::Box::CONTENT),	// size of the quad
			Colourb(255, 255, 255, 255),		// colour of the vertices
			texcoords[0],									// top-left texture coordinate
			texcoords[1]);								// top-right texture coordinate

		this->overlayGeometry.SetTexture(this->overlay);
	}
	// Release the old geometry before specifying the new vertices.
	geometry.Release(true);

	std::vector< Rocket::Core::Vertex >& vertices = this->geometry.GetVertices();
	std::vector< int >& indices = this->geometry.GetIndices();

	vertices.resize(4);
	indices.resize(6);

	float offset = 0.0f;
	const Rocket::Core::Property* off = this->GetProperty("minimapoffset");
	if (off)
	{
		offset = off->Get<float>();
	}
	// Generate the texture coordinates.
	Vector2f texcoords[2];
    texcoords[0] = Vector2f(0.0f, 0.0f);
    texcoords[1] = Vector2f(1.0f, 1.0f);
    Vector2f box = this->GetBox().GetSize(Rocket::Core::Box::CONTENT);


    Rocket::Core::GeometryUtilities::GenerateQuad(&vertices[0],									// vertices to write to
        &indices[0],									// indices to write to
        box * offset,
        box * (1.0f - (2.0f * offset)),
		Colourb(255, 255, 255, 255),		// colour of the vertices
		texcoords[0],									// top-left texture coordinate
		texcoords[1]);								// top-right texture coordinate

	geometry_dirty = false;
		
	if (!this->texture)
	{
		this->texture = n_new(Rocket::Core::Texture);
		const Rocket::Core::Property* prop = this->GetProperty("minimaptexture");		
		if (prop)
		{
			Rocket::Core::String target = prop->Get<String>();
			this->texture->Load(target.CString());
		}
		else
		{
			this->texture->Load("?Minimap");
		}
	}
	if (this->texture)
	{
		this->geometry.SetTexture(this->texture);
	}	

	this->DirtyLayout();
}

//------------------------------------------------------------------------------
/**
*/
bool
ElementMiniMap::GetIntrinsicDimensions(Core::Vector2f& dimensions)
{
	if (this->HasAttribute("width"))
	{
		dimensions.x = GetAttribute< float >("width", 512);
	}
	else
	{
		dimensions.x = 512;
	}
	if (this->HasAttribute("height"))
	{
		dimensions.y = GetAttribute< float >("height", 512);
	}
	else
	{
		dimensions.y = 512;
	}
	
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ElementMiniMap::OnPropertyChange(const Rocket::Core::PropertyNameList& changed_properties)
{
	Element::OnPropertyChange(changed_properties);
}


}
}
