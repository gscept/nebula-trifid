//------------------------------------------------------------------------------
//  sequenceconverterapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sequenceconverterapp.h"
#include "system/win32/win32registry.h"
#include "io/filestream.h"
#include "io/xmlreader.h"
#include "io/xmlwriter.h"
#include "timing/time.h"

using namespace IO;
using namespace Util;
using namespace System;
using namespace ToolkitUtil;

namespace Toolkit
{

//------------------------------------------------------------------------------
/**
*/
SequenceConverterApp::SequenceConverterApp() :
    waitForKey(false)
{

}
    
//------------------------------------------------------------------------------
/**
*/
SequenceConverterApp::~SequenceConverterApp()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
void SequenceConverterApp::Close()
{
    ConsoleApplication::Close();
    this->modifier = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
SequenceConverterApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        // show help and wait for key
        if(true == this->args.GetBoolFlag("-help"))
        {
            this->waitForKey = true;
            return false;
        }
        this->waitForKey = this->args.GetBool("-waitforkey", "false");
        this->sourceFileName = args.GetString("-sourcefile","");
        // set up config file path
        if (args.HasArg("-configfile"))
        {
            this->specFilePath = args.GetString("-configfile");
            n_printf("using \"%s\" as config file.\n\n", this->specFilePath.AsString().AsCharPtr());
        }
        else
        {
            n_printf("no config file specified!\n");
            return false;
        }
        // set up path to source directory
        if (args.HasArg("-srcdir"))
        {
            this->srcDir = args.GetString("-srcdir");
            n_printf("using \"%s\" as source directory.\n\n", this->srcDir.AsString().AsCharPtr());
        }
        else
        {
            this->srcDir = GetProjDir();
            this->srcDir.AppendLocalPath(String("work\\sequences\\cutscenes"));
            n_printf("no source directory specified! Using default dir:\n%s\n\n", this->srcDir.AsString().AsCharPtr());
        }
        // set up path to destination directory
        if (args.HasArg("-dstdir"))
        {
            this->dstDir = args.GetString("-dstdir");
            n_printf("using \"%s\" as destination directory.\n\n", this->dstDir.AsString().AsCharPtr());
        }
        else
        {
            this->dstDir = GetProjDir();
            this->dstDir.AppendLocalPath(String("work\\sequences\\cutscenes_modified"));
            n_printf("no destination directory specified! Using default dir:\n%s\n\n", this->dstDir.AsString().AsCharPtr());
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SequenceConverterApp::ShowHelp()
{
    n_printf("Nebula3 Sequence and Cutscene Converter.\n"
             "(C) Radon Labs GmbH 2009.\n"
             "-help       -- display this help\n"
             "-waitforkey -- wait for key when complete\n"
             "-configfile -- path to file containing modification rules\n"
             "-sourcefile -- filename of sequence to convert (if not specified, convert all files in srcdir)\n"
             "-srcdir     -- source directory (where the sequences come from)\n"
             "-dstdir     -- destination directory to store converted sequences\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SequenceConverterApp::Run()
{
    n_printf("Nebula3 Sequence and Cutscene Converter.\n\n");
    if(true == this->ParseCmdLineArgs())
    {
        // initialise the modifier
        if(!InitModifier())
        {
            n_error("can't perform modification\n");
        }
        // convert all files in source directory, if no source file was specified
        if ("" == this->sourceFileName)
        {
            Array<String> files = IoServer::Instance()->ListFiles(this->srcDir, "*.xml");
            IndexT i;
            for (i = 0; i < files.Size(); i++)
            {
                this->ConvertSequence(files[i]);
            }
        }
        // only convert this file
        else
        {
            this->ConvertSequence(sourceFileName);
        }
    }
    else
    {
        this->ShowHelp();
    }
    // wait for user input
    if (this->waitForKey)
    {
        n_printf("Press <Enter> to continue!\n");
        while (!Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
    }
}

bool
SequenceConverterApp::InitModifier()
{
    Ptr<Stream> specStream = ioServer->CreateStream(this->specFilePath);
    Ptr<XmlReader> specReader = XmlReader::Create();
    specReader->SetStream(specStream);
    if (!specReader->Open())
    {
        n_printf("Could not open specification file '%s' for reading!\n", this->specFilePath.AsString().AsCharPtr());
        return false;
    }

    if (!specReader->SetToFirstChild("ModifyNode")) {
        n_printf("No modifications specified\n");
        specReader->Close();
        return false;
    }

    this->modifier = XMLNodeModifier::Create();
    if (!this->modifier->Initialise(specReader))
    {
        n_printf("Initilisation of Modifier failed\n");
        specReader->Close();
        return false;
    }
    n_printf("Initilisation of Modifier successfull\n\n");
    specReader->Close();
    return true;
}

bool
SequenceConverterApp::ConvertSequence(Util::String fileName)
{
    URI srcFilePath = this->srcDir;
    URI dstFilePath = this->dstDir;
    srcFilePath.AppendLocalPath(fileName);
    dstFilePath.AppendLocalPath(fileName);

    // make sure the dst directory and file exist
    Ptr<IoServer> ioServer = IoServer::Instance();
    ioServer->CreateDirectory(this->dstDir);

    // setup IO streams and stream readers/writers
    Ptr<Stream> srcStream = ioServer->CreateStream(srcFilePath);
    Ptr<XmlReader> srcReader = XmlReader::Create();
    srcReader->SetStream(srcStream);
    if (!srcReader->Open())
    {
        n_printf("Could not open source file '%s' for reading!\n", srcFilePath.AsString().AsCharPtr());
        return false;
    }
    Ptr<Stream> dstStream = ioServer->CreateStream(dstFilePath);
    Ptr<XmlWriter> dstWriter = XmlWriter::Create();
    dstWriter->SetStream(dstStream);
    if (!dstWriter->Open())
    {
        n_printf("Could not open destination file '%s' for writing!\n", dstFilePath.AsString().AsCharPtr());
        return false;
    }
    n_printf("Starting modification of '%s'...\n", fileName.AsCharPtr());
    // set top node and start converting the xml data structure
    dstWriter->BeginNode(modifier->GetNodeName());
    this->modifier->Modify(dstWriter, srcReader);
    dstWriter->EndNode();
    n_printf("done\n");

    srcReader->Close();
    dstWriter->Close();
    return true;
}

//------------------------------------------------------------------------------
/**
*   gets the directory of working project (read from registry)
*/
IO::URI SequenceConverterApp::GetProjDir(void)
{
    URI projDirectory;
    if (Win32Registry::Exists(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir"))
    {
        String str = "file:///";
        str.Append(Win32Registry::ReadString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir"));
        projDirectory = str;
    }
    else
    {
        projDirectory = "home:";
    }
    return projDirectory;
}

} // namespace Toolkit