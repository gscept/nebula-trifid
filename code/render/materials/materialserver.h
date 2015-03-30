#pragma once
//------------------------------------------------------------------------------
/**
	@class Material::MaterialServer
    
	Server object for the material subsystem. Factory for Materials.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "resources/resourceid.h"
#include "materials/materialpalette.h"
#include "materials/material.h"
#include "frame/frameshader.h"
#include "util/dictionary.h"
#include "util/array.h"

//------------------------------------------------------------------------------
namespace Materials
{

class MaterialServer : public Core::RefCounted
{
	__DeclareClass(MaterialServer);
	__DeclareSingleton(MaterialServer);
public:

	/// constructor
	MaterialServer();
	/// destructor
	virtual ~MaterialServer();
	/// open the material server (loads all materials)
	bool Open();
	/// close the material server
	void Close();
	/// return if server is open
	bool IsOpen() const;

    /// returns true if material exists
    bool HasMaterial(const Resources::ResourceId& name);
	/// get material by name
	const Ptr<Material>& GetMaterialByName(const Resources::ResourceId& name);
    /// get all materials, creates new array with materials
    Util::Array<Ptr<Material>> GetMaterials() const;
	/// get material codes by type
	const Util::Array<Ptr<Material> >& GetMaterialsByNodeType(const Models::ModelNodeType::Code& type);
	/// returns true if we have any materials by type
	const bool HasMaterialsByNodeType(const Models::ModelNodeType::Code& type);
	/// add a material to the server
	void AddMaterial(const Ptr<Material>& material);

	/// convert a shader feature string into a feature bit mask
	Materials::MaterialFeature::Mask FeatureStringToMask(const Util::String& str);
	/// convert shader feature bit mask into string
	Util::String FeatureMaskToString(Materials::MaterialFeature::Mask mask);

	/// reloads the render materials palette
	void ReloadRenderMaterials();

	/// gain access to a material palette by name, will be loaded if it isn't already
	Ptr<MaterialPalette> LookupMaterialPalette(const Resources::ResourceId& name);
	
private:
	/// load material palette
	void LoadMaterialPalette(const Resources::ResourceId& name);

	MaterialFeature materialFeature;
	Util::Dictionary<Resources::ResourceId, Ptr<Material> > materials;
	Util::Dictionary<Models::ModelNodeType::Code, Util::Array<Ptr<Material> > > materialsByType;
	Util::Dictionary<Resources::ResourceId, Ptr<MaterialPalette> > materialPalettes;
	bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
MaterialServer::HasMaterial( const Resources::ResourceId& name )
{
    return this->materials.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Material>&
MaterialServer::GetMaterialByName(const Resources::ResourceId& name)
{
    n_assert(this->materials.Contains(name));
    return this->materials[name];
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<Material>>
MaterialServer::GetMaterials() const
{
    return this->materials.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Material> >& 
MaterialServer::GetMaterialsByNodeType( const Models::ModelNodeType::Code& type )
{
	n_assert(this->materialsByType.Contains(type));
	return this->materialsByType[type];
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
MaterialServer::HasMaterialsByNodeType( const Models::ModelNodeType::Code& type )
{
	return this->materialsByType.Contains(type);
}

}

