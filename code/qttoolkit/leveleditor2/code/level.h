#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::Level
    
    Holds level specific data, loads and saves as well
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/xmlwriter.h"
#include "io/xmlreader.h"
#include "math/bbox.h"
#include "game/levelparser.h"
#include "game/entity.h"
#include "attributecontainer.h"

namespace PostEffect
{
class PostEffectEntity;
}

namespace LevelEditor2
{

class ParsedLevel : public ToolkitUtil::LevelParser
{
	__DeclareClass(ParsedLevel);
public:
	/// set level name
	virtual void SetName(const Util::String & name);
	/// parse layer information
	virtual void AddLayer(const Util::String & name, bool visible, bool autoload, bool locked);
	/// add entity
	virtual void AddEntity(const Util::String & category, const Attr::AttributeContainer & attrs);
	/// posteffect
	virtual void SetPosteffect(const Util::String & preset, const Math::matrix44 & globallightTransform);
	///
	bool LoadEntities(const IO::URI & fileName);
	/// level dimensions
	virtual void SetDimensions(const Math::bbox & box) {}
    ///
    virtual void AddReference(const Util::String & name);


	/// 
	void UpdateGuids();

	struct _Layer
	{
		Util::String name;
		bool visible;
		bool autoload;
		bool locked;
	};
	Util::String name;
	Util::String preset;
	Math::matrix44 lightTrans;
	Util::Array<Util::KeyValuePair<Util::String, Attr::AttributeContainer>> entities;
	Util::Array<_Layer> layers;
    Util::Array<Util::String> references;
};

class Level : public ToolkitUtil::LevelParser
{
	__DeclareClass(Level);
	__DeclareSingleton(Level);
public:
    enum LoadMode
    {
        Replace,
        Merge,
        Reference
    };

	/// constructor
	Level();
	/// destructor
	virtual ~Level();

	/// Loads a level from an xml file in work:levels. creates entities via CreateEntityByAttrs and CreateGameEntityByAttrs
	bool LoadLevel(const Util::String& name, LoadMode mode);
	
	/// Save level with different name, name is only basename, without path or extension
	void SaveLevelAs(const Util::String& name);
	/// Save level
	void SaveLevel();	
	/// saves selected entities as a new level
	void SaveSelection(const Util::String & name);

	/// get level name
	const Util::String& GetName() const;

	/// set Level is starting level
	void SetStartLevel (const bool& sLevel);
	/// get Level is starting level
	const bool& GetStartLevel() const;
	/// set Auto batch of level after save
	void SetAutoBatch (const bool& batch);
	/// get Auto batch of level after save
	const bool& GetAutoBatch() const;
	/// load default level configuration
	void LoadDefaults();
	/// get current bounding box
	Math::bbox GetBoundingBox();

	/// save array of entities
	void SaveEntityArray(const Util::Array<Ptr<Game::Entity>> & entities, const IO::URI& filename);
	/// load a level section
	bool LoadEntities(const IO::URI & filename);
	///
	void Clear();

    /// 
    void RemoveReference(const Util::String & level);

protected:

	/// performs the actual saving
	void SaveLevelFile(const Util::String& name, const IO::URI & filename, bool selectedOnly, Math::bbox& box);

    bool LoadLevelFile(const Util::String& name, LoadMode mode);

    /// set level name
    virtual void SetName(const Util::String & name);
    /// parse layer information
    virtual void AddLayer(const Util::String & name, bool visible, bool autoload, bool locked);
    /// add entity
    virtual void AddEntity(const Util::String & category, const Attr::AttributeContainer & attrs);
    /// posteffect
    virtual void SetPosteffect(const Util::String & preset, const Math::matrix44 & globallightTransform);
    /// level dimensions
    virtual void SetDimensions(const Math::bbox & box);
    ///
    virtual void AddReference(const Util::String & name) {}

	/// save single entity
	bool SaveEntity(const Util::String & levelName, const Ptr<Game::Entity>& entity, const Ptr<IO::XmlWriter>& stream);

	/// replace guids in importedEntities array and update parent guids
	void UpdateGuids();
	
	/// export level to database
	void ExportLevel(const Util::String&level);
	/// construct an _ID from type optional category and name
	Util::String AllocateID(const Util::String& objType, const Util::String&category, const Util::String &name);
	/// write a string node
	void WriteString(const Ptr<IO::XmlWriter> & xmlWriter, const Util::String & node, const Util::String& content );
    /// loads generic PE attribute by attribute name and string value
    void ReadPostEffectAttribute(const Util::String& attrName, const Util::String& content, const Ptr<PostEffect::PostEffectEntity>& postEffectEntity);    

	Util::String name;	
	bool startLevel;
	bool autoBatch;		
	Util::Dictionary<Util::String,int> objectCounters;    
	Util::Dictionary<Util::String,Ptr<ParsedLevel>> refLevels;	
}; 

//------------------------------------------------------------------------------------
/**
*/
inline const Util::String&
Level::GetName () const
{
	return this->name;
}
//------------------------------------------------------------------------------------
/**
*/
inline void 
Level::SetStartLevel (const bool& sLevel)
{
	this->startLevel = sLevel;
}

//------------------------------------------------------------------------------------
/**
*/
inline const bool&
Level::GetStartLevel () const
{
	return this->startLevel;
}

//------------------------------------------------------------------------------------
/**
*/
inline void 
Level::SetAutoBatch (const bool& batch)
{
	this->autoBatch = batch;
}

//------------------------------------------------------------------------------------
/**
*/
inline const bool&
Level::GetAutoBatch () const
{
	return this->autoBatch;
};
} // namespace LevelEditor2
//------------------------------------------------------------------------------