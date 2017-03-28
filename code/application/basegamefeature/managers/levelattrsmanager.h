#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::LevelAttrsManager
  
    Provides read access to level attributes stored in database
       
    (C) 2016 Individual contributors, see AUTHORS file
*/    
#include "game/manager.h"
#include "core/singleton.h"
#include "db/valuetable.h"
#include "basegamefeatureunit.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class LevelAttrsManager : public Game::Manager
{
__DeclareClass(LevelAttrsManager);
__DeclareSingleton(LevelAttrsManager);
public:
    /// constructor
	LevelAttrsManager();
    /// destructor
	virtual ~LevelAttrsManager();

    /// return true if global attribute exists
    bool HasAttr(const Attr::AttrId& attrId);
    /// get a global string attribute
    const Util::String& GetString(const Attr::StringAttrId& attrId);
    /// get a global int attribute
	int GetInt(const Attr::IntAttrId& attrId);
    /// get a global float attribute
    float GetFloat(const Attr::FloatAttrId& attrId);    
    /// get a global bool attribute
    bool GetBool(const Attr::BoolAttrId& attrId);    
    /// get a global float4 attribute
    const Math::float4 GetFloat4(const Attr::Float4AttrId& attrId);    
    /// get a global matrix44 attribute
    const Math::matrix44 GetMatrix44(const Attr::Matrix44AttrId& attrId);    
    /// get a global guid attribute
    const Util::Guid& GetGuid(const Attr::GuidAttrId& attrId);    
    /// get a global blob attribute
    const Util::Blob& GetBlob(const Attr::BlobAttrId& attrId);

private:

	/// update level table if the current level does not match the stored one
	void UpdateLevelTable(const Util::String & level);
	/// check for current level
	void ValidateLevel();
	Util::String currentLevel;
	Ptr<Db::ValueTable> table;
	IndexT row;
};


//------------------------------------------------------------------------------
/**
*/
inline void
LevelAttrsManager::ValidateLevel()
{
	Util::String level = BaseGameFeature::BaseGameFeatureUnit::GetCurrentLevel();
	if (this->currentLevel != level)
	{
		this->UpdateLevelTable(level);
	}	
}

//------------------------------------------------------------------------------
/**
*/
inline bool
LevelAttrsManager::HasAttr(const Attr::AttrId& attrId)
{
	this->ValidateLevel();
    return this->table->HasColumn(attrId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
LevelAttrsManager::GetString(const Attr::StringAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetString(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline int
LevelAttrsManager::GetInt(const Attr::IntAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetInt(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline float
LevelAttrsManager::GetFloat(const Attr::FloatAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetFloat(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
LevelAttrsManager::GetBool(const Attr::BoolAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetBool(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4
LevelAttrsManager::GetFloat4(const Attr::Float4AttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetFloat4(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44
LevelAttrsManager::GetMatrix44(const Attr::Matrix44AttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetMatrix44(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Guid&
LevelAttrsManager::GetGuid(const Attr::GuidAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetGuid(attrId, this->row);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
LevelAttrsManager::GetBlob(const Attr::BlobAttrId& attrId)
{
	this->ValidateLevel();
	return this->table->GetBlob(attrId, this->row);
}

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
