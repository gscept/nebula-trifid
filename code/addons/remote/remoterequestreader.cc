//------------------------------------------------------------------------------
//  remoterequestreader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remoterequestreader.h"
//#include "io/textreader.h"
#include "io/xmlreader.h"

namespace Remote
{
__ImplementClass(Remote::RemoteRequestReader, 'RRQR', IO::StreamReader);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
RemoteRequestReader::RemoteRequestReader() :
    isValidRemoteRequest(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
RemoteRequestReader::ReadRequest()
{
    this->isValidRemoteRequest = false;
        
    // attach an xml reader to our stream and parse the request header
    this->xmlReader = XmlReader::Create();
    this->xmlReader->SetStream(this->stream);
    if (xmlReader->Open())
    {
        Util::String rootNodeName = xmlReader->GetCurrentNodeName();
    
        // test if valid remote control xml chunk
        if ("RemoteControl" == rootNodeName)
        {
            // get attribttes
            this->controllerName = this->xmlReader->GetString("controller");
            // set as valid request
            this->isValidRemoteRequest = true;
        }

        this->xmlReader->Close();
        return true;
    }
    return false;
}

} // namespace Remote