//------------------------------------------------------------------------------
//  distributedtesttoolapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "distributedtesttoolapp.h"
#include "io/ioserver.h"
#include "io/uri.h"
#include "io/assignregistry.h"

using namespace IO;
//------------------------------------------------------------------------------
namespace DistributedTools
{
//------------------------------------------------------------------------------
/**
	Constructor
*/
DistributedTestToolApp::DistributedTestToolApp()
{
}

//------------------------------------------------------------------------------
/**
    Convert text files	
*/
void
DistributedTestToolApp::DoWork()
{
    // get file list
    Util::Array<Util::String> fileList = this->CreateFileList();

    // copy all files to output dir
    IndexT i;
    for (i=0;i<fileList.Size();i++)
    {
        URI src;
        src.Set(fileList[i]);
        URI dst;
        dst.Set("dst:");
        if(!IoServer::Instance()->DirectoryExists(dst))
        {
            IoServer::Instance()->CreateDirectory(dst);
        }
        dst.AppendLocalPath(src.LocalPath().ExtractFileName());
        // need to remove optional readonly flag of file
        bool isReadOnly = IoServer::Instance()->IsReadOnly(src);
        if(isReadOnly)
        {
            IoServer::Instance()->SetReadOnly(src,false);
        }
        IoServer::Instance()->CopyFile(src,dst);
        n_printf("* Copy file: %s\n",src.LocalPath().ExtractFileName().AsCharPtr());
        if(isReadOnly)
        {
            IoServer::Instance()->SetReadOnly(src,true);
        }
        // make the tool need more time for debugging
        n_sleep(1);
    }
}

//------------------------------------------------------------------------------
/**
    Show help text
*/
void
DistributedTestToolApp::ShowHelp()
{
    n_printf("Nebula3 Distributed Testtool.\n"
        "(C) 2009 Radon Labs GmbH\n%s",
        this->GetArgumentDescriptionString().AsCharPtr()
        );
}
} // namespace DistributedTools