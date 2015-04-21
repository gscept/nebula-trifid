#pragma once
//------------------------------------------------------------------------------
/**
    @class ToolkitUtil::LevelParser
    
    base class for level parsing
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/xmlreader.h"
#include "attributecontainer.h"
#include "math/bbox.h"
namespace ToolkitUtil
{
class LevelParser : public Core::RefCounted
{
	__DeclareClass(LevelParser);
public:
	/// constructor
	LevelParser();
	/// destructor
	virtual ~LevelParser();

    /// Loads a level from an xml file in work:levels. 
    bool LoadXmlLevel(const Ptr<IO::XmlReader> & reader);
protected:
    /// set level name
    virtual void SetName(const Util::String & name) = 0;
    /// parse layer information
    virtual void AddLayer(const Util::String & name, bool visible, bool autoload, bool locked) = 0;
    /// add entity
    virtual void AddEntity(const Util::String & category, const Attr::AttributeContainer & attrs) = 0;
    /// posteffect
    virtual void SetPosteffect(const Util::String & preset, const Math::matrix44 & globallightTransform) = 0;
    /// level dimensions
    virtual void SetDimensions(const Math::bbox & box) = 0;

}; 
} // namespace ToolkitUtil
//------------------------------------------------------------------------------