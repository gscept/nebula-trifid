//------------------------------------------------------------------------------
//  shaderparam.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderparam.h"

namespace Tools
{
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
bool
ShaderParam::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Param");
    n_assert(xmlReader->HasAttr("name"));
    n_assert(xmlReader->HasAttr("type"));
    n_assert(xmlReader->HasAttr("annotation"));
    this->name = xmlReader->GetString("name");
    this->type = xmlReader->GetString("type");
    this->annotation = xmlReader->GetString("annotation");
    if (xmlReader->HasAttr("default"))
    {
        this->defaultValue = xmlReader->GetString("default");
    }
    if (xmlReader->HasAttr("defaultBinding"))
    {
        this->defaultBinding = xmlReader->GetString("defaultBinding");
    }
    return true;
}

} // namespace Tools