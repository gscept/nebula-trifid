//------------------------------------------------------------------------------
//  resourcedictionary.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourcedictionary.h"
#include "io/ioserver.h"
#include "io/stream.h"

namespace Resources
{
__ImplementClass(Resources::ResourceDictionary, 'RDIC', Core::RefCounted);
__ImplementSingleton(Resources::ResourceDictionary);

using namespace Util;
using namespace IO;
using namespace System;

//------------------------------------------------------------------------------
/**
*/
ResourceDictionary::ResourceDictionary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ResourceDictionary::~ResourceDictionary()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    NOTE: we assume that the dictionary file is already in host byte
    order!
*/
bool
ResourceDictionary::Load()
{
    n_assert(this->uri.IsValid());
    n_assert(!this->IsValid());

    Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->uri);
    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
        // map the stream into memory
        uint* ptr = (uint*) stream->Map();
        
        // read header data
        FourCC magic    = *ptr++;
        uint version    = *ptr++;
        uint numEntries = *ptr++;
        if (FourCC(magic) != FourCC('RDIC'))
        {
            n_error("ResourceDictionary: '%s' is not a resource dictionary file!", this->uri.AsString().AsCharPtr());
            return false;
        }
        if (version != 1)
        {
            n_error("ResourceDictionary: '%s' has unsupported version!", this->uri.AsString().AsCharPtr());
            return false;
        }
        // check that file size is corrent
        n_assert(stream->GetSize() == (Stream::Size)(3 * sizeof(int) + numEntries * 128));

        // FIXME: actually, we can guarantee that the data in the dictionary 
        // is already in sorted order!
        this->dict.Reserve(numEntries);
        this->dict.BeginBulkAdd();
        IndexT i;
        for (i = 0; i < (SizeT)numEntries; i++)
        {
            Entry entry;
            entry.SetSize(*ptr++);
            String resId((const char*)ptr);
            ptr += (MaxResIdLength / sizeof(uint));     // skip to next entry
            this->dict.Add(resId, entry);
        }
        this->dict.EndBulkAdd();
        stream->Unmap();
        stream->Close();
        return true;
    }
    else
    {
        n_error("ResourceDictionary: failed to load dictionary file '%s'!", this->uri.AsString().AsCharPtr());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceDictionary::Unload()
{
    n_assert(this->IsValid());
    this->dict.Clear();
}

} // namespace Resources