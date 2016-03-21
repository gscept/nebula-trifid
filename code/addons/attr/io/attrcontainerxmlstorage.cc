//------------------------------------------------------------------------------
//  attrcontainerxmlstorage.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attr/io/attrcontainerxmlstorage.h"
#include "io/xmlreader.h"
#include "io/xmlwriter.h"

namespace Attr
{
__ImplementClass(AttrContainerXMLStorage, 'ACXM', Attr::AttrContainerStorageBase);    

//------------------------------------------------------------------------------
/**
*/
AttrContainerXMLStorage::AttrContainerXMLStorage()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttrContainerXMLStorage::~AttrContainerXMLStorage()
{
    if(this->stream.isvalid())
    {
        this->stream = 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
void
AttrContainerXMLStorage::Load()
{
    n_assert(this->stream.isvalid());
    this->attrs.Clear();
    Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
    xmlReader->SetStream(this->stream);
    this->stream->SetAccessMode(IO::Stream::ReadAccess);
    if(xmlReader->Open())
    {
        
        if(xmlReader->SetToFirstChild("Attribute"))
        {
            Util::Array<Attr::Attribute> attrs;
            do
            {
                Util::String name = xmlReader->GetString("name");
                Attr::ValueType vtype = Attr::Attribute::StringToValueType(xmlReader->GetString("type"));
                Util::String val = xmlReader->GetString("value");
                if(!Attr::AttributeDefinitionBase::FindByName(name))
                {
                    Attr::AttributeDefinitionBase::RegisterDynamicAttribute(name, Util::FourCC(), vtype, Attr::ReadWrite);
                }						                
                Attr::Attribute atr;
                atr.SetAttrId(Attr::AttrId(name));						
                atr.SetValueFromString(val);
                this->attrs.AddAttr(atr);                
            }
            while(xmlReader->SetToNextChild("Attribute"));

          
        }
        xmlReader->Close();				
    }    

}


//------------------------------------------------------------------------------
/**
*/
void
AttrContainerXMLStorage::Save()
{
    n_assert(this->stream.isvalid());

    Ptr<IO::XmlWriter> xmlWriter = IO::XmlWriter::Create();
    xmlWriter->SetStream(this->stream);
    this->stream->SetAccessMode(IO::Stream::WriteAccess);
    if(xmlWriter->Open())
    {
        xmlWriter->BeginNode(this->table);

        IndexT i;
        SizeT num = this->attrs.GetAttrs().Size();
        for(i = 0 ; i<num ; i++)
        {
            const Attr::Attribute& value = this->attrs.GetAttrs().ValueAtIndex(i);
            const Attr::AttrId& attrId = value.GetAttrId();
            xmlWriter->BeginNode("Attribute");
            xmlWriter->SetString("name",attrId.GetName());
            xmlWriter->SetString("type",Attr::Attribute::ValueTypeToString(attrId.GetValueType()));
            xmlWriter->SetString("value",value.ValueAsString());
            xmlWriter->EndNode();
        }
        xmlWriter->EndNode();
        xmlWriter->Close();
    }
}
}