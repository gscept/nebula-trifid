//------------------------------------------------------------------------------
//  idlattribute.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkitutil/idldocument/idlattribute.h"

namespace Tools
{
__ImplementClass(Tools::IDLAttribute, 'ILAT', Core::RefCounted);

using namespace IO;

//------------------------------------------------------------------------------
/**
*/
IDLAttribute::IDLAttribute() : 
hasDefault(false),system(false),hasDisplay(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
IDLAttribute::Parse(XmlReader* reader)
{
    n_assert(0 != reader);
    n_assert(reader->GetCurrentNodeName() == "Attribute");

    // parse attributes
    this->name = reader->GetString("name");
    if (reader->HasAttr("fourcc"))
    {
        this->fourcc = reader->GetString("fourcc");
    }
    if (reader->HasAttr("type"))
    {
        this->attrType = reader->GetString("type");
    }
    if (reader->HasAttr("accessMode"))
    {
        this->accessMode = reader->GetString("accessMode");
    }
    if (reader->HasAttr("system"))
    {
        this->system = reader->GetInt("system") != 0;
    }
    if (reader->HasAttr("default"))
    {
        this->hasDefault = true;
        this->defaultValue = reader->GetString("default");
    }
    if (reader->HasAttr("displayName"))
    {
        this->hasDisplay = true;
        this->displayName = reader->GetString("displayName");
    }
    Util::Array<Util::String> attrs = reader->GetAttrs();
    IndexT i;
    for (i = 0; i < attrs.Size(); i++)
    {
        this->attributes.Add(attrs[i], reader->GetString(attrs[i].AsCharPtr()));
    }

    return true;
}


} // namespace Tools