//------------------------------------------------------------------------------
//  countlinesapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "countlinesapp.h"
#include "io/textreader.h"
#include "io/textwriter.h"

namespace Toolkit
{
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
CountLinesApp::Run()
{
    // check command line args
    if (this->args.GetBoolFlag("-help"))
    {
        n_printf("Nebula3 project line counter.\n"
                 "-help    -- display this help\n"
                 "-project -- path to N3 project (e.g. 'c:/nebula3')\n"
                 "-dirs    -- optional: comma separated list of code directories under project/code\n",
                 "-out     -- name of output file\n");
        return;
    }
    String projPath = this->args.GetString("-project", "c:/nebula3");
    Array<String> codeDirs = this->args.GetString("-dirs", "").Tokenize(", ");
    String outFile = this->args.GetString("-out", "linecounts.csv");

    // if no subdirectories have been provided, just scan all
    String sourceDir = projPath + "/code";
    if (codeDirs.IsEmpty())
    {
        codeDirs = IoServer::Instance()->ListDirectories(sourceDir, "*");
    }

    // for each directory...
    IndexT i;
    for (i = 0; i < codeDirs.Size(); i++)
    {
        // ignore some obvious non-source dirs
        if (!this->IsIgnoreDir(codeDirs[i]))
        {
            String curPath = sourceDir + "/" + codeDirs[i];        
            if (ioServer->DirectoryExists(curPath))
            {
                n_printf("> %s\n", codeDirs[i].AsCharPtr());
                this->CountLinesLayer(curPath);
            }
            else
            {
                n_printf("! %s does not exist!\n", curPath.AsCharPtr());
            }
        }
    }

    // write a comma-separated-values file with the result
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(outFile);
    Ptr<TextWriter> textWriter = TextWriter::Create();
    textWriter->SetStream(stream);
    if (textWriter->Open())
    {
        textWriter->WriteLine("Layer;Dir;All;General;Win32;Xbox360;Win360;Wii;PS3");
        IndexT layerIndex;
        for (layerIndex = 0; layerIndex < this->layers.Size(); layerIndex++)
        {
            const Layer& curLayer = this->layers[layerIndex];
            IndexT dirIndex;
            for (dirIndex = 0; dirIndex < curLayer.dirs.Size(); dirIndex++)
            {
                const LineCounts& curLineCounts = curLayer.dirs[dirIndex];
                textWriter->WriteFormatted("%s;%s;%d;%d;%d;%d;%d;%d;%d\n",
                    curLayer.name.AsCharPtr(),
                    curLineCounts.dir.AsCharPtr(),
                    curLineCounts.all,
                    curLineCounts.general,
                    curLineCounts.win32,
                    curLineCounts.xbox360,
                    curLineCounts.win360,
                    curLineCounts.wii,
                    curLineCounts.ps3);
            }
        }
        textWriter->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
CountLinesApp::IsIgnoreDir(const String& dir) const
{
    return ((dir == "cvs") ||
            (dir == "CVS") ||
            (dir == ".svn") ||
            (dir == ".SVN") ||
            (dir == "cw") ||
            (dir == "vs9.0") ||
            (dir == "vs8.0"));
}

//------------------------------------------------------------------------------
/**
    Count line numbers in all *.h and *.cc files in the current
    directory.
*/
SizeT
CountLinesApp::CountLinesFlat(const String& dir)
{
    // list all *.cc and *.h files in directory
    IoServer* ioServer = IoServer::Instance();
    Array<String> files = ioServer->ListFiles(dir, "*.h");
    files.AppendArray(ioServer->ListFiles(dir, "*.cc"));

    // count line number in each file
    SizeT numLines = 0;
    IndexT i;
    for (i = 0; i < files.Size(); i++)
    {
        Ptr<Stream> stream = ioServer->CreateStream(dir + "/" + files[i]);
        Ptr<TextReader> textReader = TextReader::Create();
        textReader->SetStream(stream);
        if (textReader->Open())
        {
            Array<String> lines = textReader->ReadAllLines();
            numLines += lines.Size();
            textReader->Close();
        }
    }
    return numLines;
}

//------------------------------------------------------------------------------
/**
    Assumes that the current directory is a subsystem directory, counts
    the line numbers, and splits them into platform-specific line counts.
*/
CountLinesApp::LineCounts
CountLinesApp::CountLinesSubsystem(const String& dir)
{
    LineCounts lineCounts;
    lineCounts.dir = dir.ExtractFileName();
    
    // first count the line number in the subsystem dir itself
    lineCounts.general += this->CountLinesFlat(dir);

    // iterate through subdirectories (ignore version control dirs) 
    // and filter line counts by platform
    Array<String> subDirs = IoServer::Instance()->ListDirectories(dir, "*");
    IndexT i;
    for (i = 0; i < subDirs.Size(); i++)
    {
        // ignore version control dirs
        if (!this->IsIgnoreDir(subDirs[i]))
        {
            String curPath = dir + "/" + subDirs[i];
            SizeT numLines = this->CountLinesFlat(curPath);

            // split by platform
            if (subDirs[i] == "win32")        lineCounts.win32   += numLines;
            else if (subDirs[i] == "xbox360") lineCounts.xbox360 += numLines;
            else if (subDirs[i] == "win360")  lineCounts.win360  += numLines;
            else if (subDirs[i] == "wii")     lineCounts.wii     += numLines;
            else if (subDirs[i] == "ps3")     lineCounts.ps3     += numLines;
            else if (subDirs[i] == "d3d9")    lineCounts.win32   += numLines;   // not a bug
            else if (subDirs[i] == "d3dx9")   lineCounts.win32   += numLines;   // not a bug
            else                              lineCounts.general += numLines;
        }
    }
    lineCounts.all = lineCounts.general + 
                     lineCounts.win32 + 
                     lineCounts.xbox360 + 
                     lineCounts.win360 +
                     lineCounts.wii +
                     lineCounts.ps3;

    n_printf("  %s\n", lineCounts.dir.AsCharPtr());
    if (lineCounts.all > 0)
    {
        n_printf("    all: %d\n", lineCounts.all);
    }
    if (lineCounts.general > 0)
    {
        n_printf("    gen: %d\n", lineCounts.general);
    }
    if (lineCounts.win32 > 0)
    {
        n_printf("    win32: %d\n", lineCounts.win32);
    }
    if (lineCounts.xbox360 > 0)
    {
        n_printf("    xbox360: %d\n", lineCounts.xbox360);
    }
    if (lineCounts.win360 > 0)
    {
        n_printf("    win360: %d\n", lineCounts.win360);
    }
    if (lineCounts.wii > 0)
    {
        n_printf("    wii: %d\n", lineCounts.wii);
    }
    if (lineCounts.ps3 > 0)
    {
        n_printf("    ps3: %d\n", lineCounts.ps3);
    }
    return lineCounts;
}

//------------------------------------------------------------------------------
/**
    Counts the line numbers for a N3 layer and adds them to the layers array.
*/
void
CountLinesApp::CountLinesLayer(const String& dir)
{
    Layer layer;
    layer.name = dir.ExtractFileName();
    this->layers.Append(layer);

    // iterate through subdirectories
    Array<String> subDirs = IoServer::Instance()->ListDirectories(dir, "*");
    IndexT i;
    for (i = 0; i < subDirs.Size(); i++)
    {
        if (!this->IsIgnoreDir(subDirs[i]))
        {
            String curPath = dir + "/" + subDirs[i];
            this->layers.Back().dirs.Append(this->CountLinesSubsystem(curPath));
        }
    }    
}

} // namespace Toolkit