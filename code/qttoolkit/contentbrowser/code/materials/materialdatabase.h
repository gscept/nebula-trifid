#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::MaterialDatabase
    
    Implements a material database, which can load materials and store them for easy access.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/singleton.h"
#include "core/refcounted.h"
#include "materials/material.h"

namespace ContentBrowser
{
class MaterialDatabase : public Core::RefCounted
{
	__DeclareClass(MaterialDatabase);
	__DeclareSingleton(MaterialDatabase);	
public:
	/// constructor
	MaterialDatabase();
	/// destructor
	virtual ~MaterialDatabase();
	/// open the MaterialDatabase
	bool Open();
	/// close the MaterialDatabase
	void Close();
	/// return if MaterialDatabase is open
	bool IsOpen() const;

	/// returns pointer to material
	Ptr<Materials::Material> GetMaterial(const Util::String& name);
	/// returns list of materials
	Util::Array<Util::String> GetMaterialList() const;
	/// returns list of materials based on type
	const Util::Array<Ptr<Materials::Material> > GetMaterialsByType(const Util::String& type);
	/// returns true if any materials exist matching the given type
	bool HasMaterialsByType(const Util::String& type);

private:
	bool isOpen;
	Util::Dictionary<Util::String, Ptr<Materials::Material>> materials;
	Util::Dictionary<Util::StringAtom, Util::Array<Ptr<Materials::Material>>> materialsByType;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
MaterialDatabase::IsOpen() const
{
	return this->isOpen;
}

} // namespace ContentBrowser
//------------------------------------------------------------------------------