//------------------------------------------------------------------------------
//  idlcompiler.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "toolkit/idlcompiler/idlcompiler.h"
#include "toolkitutil/idldocument/idlcodegenerator.h"

namespace Tools
{
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
IDLCompiler::IDLCompiler() :
    errorLineNumber(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Open the application.
*/
bool
IDLCompiler::Open()
{
    if (ConsoleApplication::Open())
    {
        bool helpArg = this->args.HasArg("-help");

        // parse command line args
        if (helpArg)
        {
            n_printf("NebulaT IDL Compiler.\n"
                     "Compiles NebulaT IDL XML files into C++ source and header files\n"
					 "Syntax: {-output folder} NIDLFile1 NIDLFile2 ...\n"
					 "-output specifies optional output folder for the generated files");
            return false;
        }

		// check if output directory was provided
		int argsOffset = 0;
		Util::String output = this->args.GetString("-output");
		if(!output.IsEmpty())
		{
			this->outputDir = output;
			// strip away the first to arguments since they are -output and the folder and not nidl files
			argsOffset = 2;
		}
		

        // gather files from command line
        this->fileUris.Clear();
        IndexT argIndex;
        SizeT numArgs = this->args.GetNumArgs();
        for (argIndex = argsOffset; argIndex < numArgs; argIndex++)
        {
            this->fileUris.Append(this->args.GetStringAtIndex(argIndex));
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
IDLCompiler::Close()
{
    this->document = 0;
    ConsoleApplication::Close();
}

//------------------------------------------------------------------------------
/**
    Run the application.
*/
void
IDLCompiler::Run()
{
    IndexT i;
    for (i = 0; i < this->fileUris.Size(); i++)
    {
        if (!this->CompileFile(this->fileUris[i]))
        {
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Compiles the provided IDL file into a set of C++ header and source files.
*/
bool
IDLCompiler::CompileFile(const URI& uri)
{
    n_printf("Compiling '%s'\n", uri.AsString().AsCharPtr());

    // prepare a code generator object
    Ptr<IDLCodeGenerator> codeGenerator = IDLCodeGenerator::Create();
    codeGenerator->SetURI(uri);
	
	// specify optional output directory
	if(!this->outputDir.IsEmpty())
		codeGenerator->SetOutputURI(URI(this->outputDir));

    // first do an existance time stamp check on the files
    IoServer* ioServer = IoServer::Instance();
    URI dstHeaderURI = codeGenerator->BuildHeaderUri();
    URI dstSourceURI = codeGenerator->BuildSourceUri();
    bool needsRebuild = false;
    if (!(ioServer->FileExists(dstHeaderURI) && ioServer->FileExists(dstSourceURI)))
    {
        needsRebuild = true;
    }
    else
    {
        FileTime nidlFileTime = ioServer->GetFileWriteTime(uri);
        FileTime dstHeaderFileTime = ioServer->GetFileWriteTime(dstHeaderURI);
        FileTime dstSourceFileTime = ioServer->GetFileWriteTime(dstSourceURI);
        if ((nidlFileTime > dstHeaderFileTime) || (nidlFileTime > dstSourceFileTime))
        {
            needsRebuild = true;
        }
    }

    // only compile if actually necessary
    if (needsRebuild)
    {
        // first parse the file into a C++ tree
        if (!this->ParseFile(uri))
        {
            n_printf("ERROR in file %s line %d: %s\n", uri.AsString().AsCharPtr(), this->errorLineNumber, this->error.AsCharPtr());
            return false;
        }
        
        // generate C++ output
        codeGenerator->SetDocument(this->document);
        if (!codeGenerator->GenerateIncludeFile())
        {
            n_printf("ERROR building header file for %s\n", uri.AsString().AsCharPtr());
            return false;
        }
        if (!codeGenerator->GenerateSourceFile())
        {
            n_printf("ERROR building source file for %s\n", uri.AsString().AsCharPtr());
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parses the XML source file into a tree of C++ objects.
*/
bool
IDLCompiler::ParseFile(const URI& uri)
{
    // no source file given?
    if (uri.IsEmpty())
    {
        this->error = "Source filename is empty.";
        return false;
    }

    // parse the source file into a C++ object tree
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
        Ptr<XmlReader> xmlReader = XmlReader::Create();
        xmlReader->SetStream(stream);
        if (xmlReader->Open())
        {
            // check if it is a valid IDL file
            if (xmlReader->GetCurrentNodeName() == "Nebula3")
            {
                this->document = IDLDocument::Create();
                String docName = uri.LocalPath().ExtractFileName();
                docName.StripFileExtension();
                this->document->SetName(docName);
                if (!this->document->Parse(xmlReader))
                {
                    this->error = this->document->GetError();
                    this->errorLineNumber = xmlReader->GetCurrentNodeLineNumber();
                    return false;
                }
            }
            else
            {
                this->error = "XML file has invalid format (Nebula3 root element expected).";
                this->errorLineNumber = xmlReader->GetCurrentNodeLineNumber();
                return false;
            }
            xmlReader->Close();
        }
        else
        {
            this->error.Format("Failed to open '%s' as XML file!", uri.AsString().AsCharPtr());
            return false;
        }
        stream->Close();
    }   
    else
    {
        this->error.Format("Failed to open source file '%s'!", uri.AsString().AsCharPtr());
        return false;
    }
    return true;
}

} // namespace Tools