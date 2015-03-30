//------------------------------------------------------------------------------
//  attributecontainertest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attributecontainertest.h"
#include "attr/attributecontainer.h"
#include "testattrs.h"

namespace Test
{
__ImplementClass(Test::AttributeContainerTest, 'ACTT', Test::TestCase);

using namespace Attr;

//------------------------------------------------------------------------------
/**
*/
void
AttributeContainerTest::Run()
{
    AttributeContainer attrContainer;

    this->Verify(attrContainer.GetAttrs().IsEmpty());
    this->Verify(!attrContainer.HasAttr(Attr::Name));
    attrContainer.SetString(Attr::Name, "Osiris");
    this->Verify(!attrContainer.GetAttrs().IsEmpty());
    this->Verify(attrContainer.HasAttr(Attr::Name));
    this->Verify(attrContainer.GetString(Attr::Name) == "Osiris");
    
    attrContainer.SetString(Attr::Name, "Thor");
    this->Verify(attrContainer.HasAttr(Attr::Name));
    this->Verify(attrContainer.GetString(Attr::Name) == "Thor");

    attrContainer.SetInt(Attr::Age, 23);
    this->Verify(attrContainer.HasAttr(Attr::Age));
    this->Verify(attrContainer.GetInt(Attr::Age) == 23);
}

}