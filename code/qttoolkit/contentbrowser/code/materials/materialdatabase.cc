//------------------------------------------------------------------------------
//  materialdatabase.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materialdatabase.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "materials/materialserver.h"

using namespace Util;
using namespace Graphics;
namespace ContentBrowser
{
__ImplementClass(ContentBrowser::MaterialDatabase, 'MADB', Core::RefCounted);
__ImplementSingleton(ContentBrowser::MaterialDatabase);

//------------------------------------------------------------------------------
/**
*/
MaterialDatabase::MaterialDatabase() :
	isOpen(false)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MaterialDatabase::~MaterialDatabase()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
MaterialDatabase::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;
	
    // get materials
    const Util::Array<Ptr<Materials::Material>>& materials = Materials::MaterialServer::Instance()->GetMaterials();

	IndexT i;
	for (i = 0; i < materials.Size(); i++)
	{
		const Ptr<Materials::Material>& material = materials[i];
		if (material->GetVirtual()) continue;
		String name = material->GetName().AsString();
		StringAtom type = Materials::MaterialServer::Instance()->FeatureMaskToString(material->GetFeatures());
		this->materials.Add(name, material);

		if (this->materialsByType.Contains(type))
		{
			this->materialsByType[type].Append(material);
		}
		else
		{
			this->materialsByType.Add(type, Array<Ptr<Materials::Material> >());
			this->materialsByType[type].Append(material);
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialDatabase::Close()
{
	n_assert(this->IsOpen());
	this->materials.Clear();
	this->materialsByType.Clear();
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Materials::Material>
MaterialDatabase::GetMaterial(const Util::String& name)
{
	if (!this->materials.Contains(name)) return this->materials["Placeholder"];
	else								 return this->materials[name];
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String> 
MaterialDatabase::GetMaterialList() const
{
	return this->materials.KeysAsArray();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<Materials::Material> >
MaterialDatabase::GetMaterialsByType(const Util::String& type)
{
	n_assert(this->materialsByType.Contains(type));
	return this->materialsByType[type];
}

//------------------------------------------------------------------------------
/**
*/
bool
MaterialDatabase::HasMaterialsByType(const Util::String& type)
{
	return this->materialsByType.FindIndex(type) != InvalidIndex;
}

}
