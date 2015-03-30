#pragma once
//------------------------------------------------------------------------------
/**
    @class Attr::AttrContainerXMLStorage
  
    Provides read/write access to an attribute container using a XML file
        
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "attr/io/attrcontainerstoragebase.h"

//------------------------------------------------------------------------------
namespace Attr
{
class AttrContainerXMLStorage : public Attr::AttrContainerStorageBase
{
    __DeclareClass(AttrContainerXMLStorage);

public:
    /// constructor
    AttrContainerXMLStorage();
    /// destructor
    virtual ~AttrContainerXMLStorage();

    /// set stream for reading/writing
    void SetStream(const Ptr<IO::Stream>& stream, const Util::String & table);

    /// save attributes to stream
    virtual void Save();
    /// load attributes from stream
    virtual void Load();
private:
    Ptr<IO::Stream> stream;
};


/**
*/
inline void
AttrContainerXMLStorage::SetStream(const Ptr<IO::Stream>& istream, const Util::String & itable)
{
    this->stream = istream;
    this->table = itable;
}
}