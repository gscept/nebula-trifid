//------------------------------------------------------------------------------
//  remoteresponsewriter.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remoteresponsewriter.h"
#include "io/textwriter.h"

namespace Remote
{
__ImplementClass(Remote::RemoteResponseWriter, 'RERW', IO::StreamWriter);

using namespace IO;

//------------------------------------------------------------------------------
/**
*/
void
RemoteResponseWriter::WriteResponse()
{
    // append content string
    if (this->contentStream.isvalid())
    {
        n_assert(this->contentStream->CanBeMapped());
        if (this->contentStream->Open())
        {
            void* ptr = this->contentStream->Map();
            this->stream->Write(ptr, this->contentStream->GetSize());
            this->contentStream->Unmap();
            this->contentStream->Close();
        }
    }
}

} // namespace Remote