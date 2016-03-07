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
	geometry_dirty(true)
{
}

//------------------------------------------------------------------------------
/**
*/
ElementMiniMap::~ElementMiniMap()
{
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
}

//------------------------------------------------------------------------------
/**
*/
void 
ElementMiniMap::GenerateGeometry()
{
	// Release the old geometry before specifying the new vertices.
	geometry.Release(true);

	std::vector< Rocket::Core::Vertex >& vertices = this->geometry.GetVertices();
	std::vector< int >& indices = this->geometry.GetIndices();

	vertices.resize(4);
	indices.resize(6);

	// Generate the texture coordinates.
	Vector2f texcoords[2];
	texcoords[0] = Vector2f(0, 0);
	texcoords[1] = Vector2f(1, 1);


	Rocket::Core::GeometryUtilities::GenerateQuad(&vertices[0],									// vertices to write to
		&indices[0],									// indices to write to
		Vector2f(0, 0),					// origin of the quad
		GetBox().GetSize(Rocket::Core::Box::CONTENT),	// size of the quad
		Colourb(255, 255, 255, 255),		// colour of the vertices
		texcoords[0],									// top-left texture coordinate
		texcoords[1]);								// top-right texture coordinate

	geometry_dirty = false;
	this->texture.Load("?Minimap");
	this->geometry.SetTexture(&this->texture);
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


}
}
