//------------------------------------------------------------------------------
//  mayanebula3runtime.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayanebula3runtime.h"
#include "io/logfileconsolehandler.h"

namespace Maya
{
__ImplementClass(Maya::MayaNebula3Runtime, 'MN3R', Core::RefCounted);
__ImplementSingleton(Maya::MayaNebula3Runtime);

using namespace Util;
using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MayaNebula3Runtime::MayaNebula3Runtime() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaNebula3Runtime::~MayaNebula3Runtime()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNebula3Runtime::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;

    // setup the core server
    this->coreServer = CoreServer::Create();
    this->coreServer->SetCompanyName(StringAtom("Radon Labs GmbH"));
    this->coreServer->SetAppName(StringAtom("N3 Maya Plugin"));
    this->coreServer->Open();

    // initialize IO subsystem
    this->ioServer = IoServer::Create();

    // setup a logfile console handler
    Ptr<LogFileConsoleHandler> logFileHandler = LogFileConsoleHandler::Create();
    Console::Instance()->AttachHandler(logFileHandler.upcast<ConsoleHandler>());
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNebula3Runtime::Discard()
{
    n_assert(this->IsValid());
    this->ioServer = 0;
    this->coreServer = 0;
    this->isValid = false;
}

} // namespace Maya