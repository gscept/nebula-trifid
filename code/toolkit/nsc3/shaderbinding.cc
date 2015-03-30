//------------------------------------------------------------------------------
//  shaderbinding.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderbinding.h"

namespace Tools
{
__ImplementClass(Tools::ShaderBinding, 'SDBG', Core::RefCounted);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ShaderBinding::ShaderBinding()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderBinding::~ShaderBinding()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderBinding::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Bind");
    n_assert(xmlReader->HasAttr("param"));
    n_assert(xmlReader->HasAttr("bindto"));
    this->param = xmlReader->GetString("param");
    String bindto = xmlReader->GetString("bindto");
    Array<String> bindTokens = bindto.Tokenize(".");
    if (bindTokens.Size() != 2)
    {
        n_printf("Invalid bind attribute: %s!\n", bindto.AsCharPtr());
        return false;
    }
    this->bindNode = bindTokens[0];
    this->bindParam = bindTokens[1];
    return true;
}

//------------------------------------------------------------------------------
/**
*/
const String&
ShaderBinding::GetParam() const
{
    return this->param;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
ShaderBinding::GetBindNode() const
{
    return this->bindNode;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
ShaderBinding::GetBindParam() const
{
    return this->bindParam;
}

} // namespace Tools
