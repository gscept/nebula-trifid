//------------------------------------------------------------------------------
//  genrandtableapplication.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "genrandtableapplication.h"
#include "io/ioserver.h"
#include "io/textwriter.h"

using namespace Util;
using namespace IO;
using namespace Math;

namespace Tools
{

//------------------------------------------------------------------------------
/**
*/
void
GenRandTableApplication::Run()
{
    SizeT tableSize = this->args.GetInt("-size", 1024);
    URI uri = this->args.GetString("-file", "randtable.txt");

    Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
    stream->SetURI(uri);
    Ptr<TextWriter> textWriter = TextWriter::Create();    
    textWriter->SetStream(stream);
    if (textWriter->Open())
    {
        textWriter->WriteFormatted("static float RandTable[%d] = {\n", tableSize);
        IndexT i;
        for (i = 0; i < tableSize; i++)
        {
            textWriter->WriteLine("    " + String::FromFloat(n_rand()) + ",");
        }
        textWriter->WriteLine("};");
        textWriter->Close();
    }
    else
    {
        n_printf("Error opening file '%s' for writing!\n", uri.AsString().AsCharPtr());
    }
}

} // namespace Tools


