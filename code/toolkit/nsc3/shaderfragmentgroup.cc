//------------------------------------------------------------------------------
//  shaderfragmentgroup.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderfragmentgroup.h"

namespace Tools
{
__ImplementClass(Tools::ShaderFragmentGroup, 'SFGP', Core::RefCounted);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ShaderFragmentGroup::ShaderFragmentGroup() :
    isExclusive(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderFragmentGroup::~ShaderFragmentGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderFragmentGroup::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "FragmentGroup");

    // parse attributes
    n_assert(xmlReader->HasAttr("name"));
    n_assert(xmlReader->HasAttr("category"));
    n_assert(xmlReader->HasAttr("desc"));
    this->name = xmlReader->GetString("name");
    this->category = xmlReader->GetString("category");
    this->desc = xmlReader->GetString("desc");
    if (xmlReader->HasAttr("exclusive"))
    {
        this->isExclusive = xmlReader->GetBool("exclusive");
    }

    // parse shader fragments...
    if (xmlReader->SetToFirstChild("Fragment")) do
    {
        Ptr<ShaderFragment> frag = ShaderFragment::Create();
        frag->SetFileURI(fileUri);
        if (frag->Parse(xmlReader))
        {
            if (!this->fragments.Contains(frag->GetName()))
            {
                this->fragments.Add(frag->GetName(), frag);
            }
            else
            {
                n_printf("WARNING: duplicate group fragment '%s' in file '%s'!\n",
                    frag->GetName().AsCharPtr(),
                    this->fileUri.AsString().AsCharPtr());
            }
        }
        else
        {
            n_printf("ERROR parsing group fragment in file '%s' (skipping)", this->fileUri.AsString().AsCharPtr());
        }
    }
    while (xmlReader->SetToNextChild("Fragment"));
    return true;
}

} // namespace Tools