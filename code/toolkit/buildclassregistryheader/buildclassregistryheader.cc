//------------------------------------------------------------------------------
//  buildclassregistryheader.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "util/commandlineargs.h"
#include "app/consoleapplication.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "io/textwriter.h"

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
void
RecurseListFiles(const String& dir, const Ptr<TextWriter>& textWriter)
{
    // list files in directory
    Array<String> files = IoServer::Instance()->ListFiles(dir, "*.h");
    IndexT fileIndex;
    for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
    {
        textWriter->WriteFormatted("#include \"%s/%s\"\n", dir.AsCharPtr(), files[fileIndex].AsCharPtr());
    }   

    // parse subdirectories
    Array<String> subDirs = IoServer::Instance()->ListDirectories(dir, "*");
    IndexT dirIndex;
    for (dirIndex = 0; dirIndex < subDirs.Size(); dirIndex++)
    {
        if ((subDirs[dirIndex] != "CVS") &&
            (subDirs[dirIndex] != ".svn"))
        {
            String path = dir + "/" + subDirs[dirIndex];
            RecurseListFiles(path, textWriter);
        }
    }
}

//------------------------------------------------------------------------------
/**
    A little command line tool to build a class registry header from
    a number of include files.
*/
void __cdecl
main(int argc, const char** argv)
{
    Util::CommandLineArgs args(argc, argv);
    App::ConsoleApplication app;
    app.SetCompanyName("Radon Labs GmbH");
    app.SetAppTitle("BuildClassRegistryHeader");
    if (app.Open())
    {
        // display help?
        if ((args.GetNumArgs() == 0) || args.GetBoolFlag("-help"))
        {
            n_printf("buildclassregistryheader\n"
                     "(C) 2008 Radon Labs GmbH\n"
                     "-help -- show this help\n"
                     "-dirs -- semicolon separated list of subdirectories\n"
                     "-out  -- output filename\n");
        }
        else
        {
            // get directories to scan for headers
            String dirArg = args.GetString("-dirs", ".");
            Array<String> dirs = dirArg.Tokenize(";");

            // open output file
            String outArg = args.GetString("-out", "classregistry.h");
            Ptr<Stream> stream = IoServer::Instance()->CreateStream(outArg);
            Ptr<TextWriter> textWriter = TextWriter::Create();
            textWriter->SetStream(stream);
            if (textWriter->Open())
            {
                IndexT dirIndex;
                for (dirIndex = 0; dirIndex < dirs.Size(); dirIndex++)
                {
                    RecurseListFiles(dirs[dirIndex], textWriter);
                }
                textWriter->Close();
            }
        }
        app.Close();
    }
    app.Exit();
}

