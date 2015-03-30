#pragma once
//------------------------------------------------------------------------------
/**
	@class DistributedTools::DistributedJobServiceApp
    
    A distributed job service application is designed to run for a undefined
    time. It stays idle and listens to the network for incoming jobs.
    These jobs are stored in a list. If a Run command arrives, all jobs
    are started on this machine. While jobs are running no new jobs can be
    attached and started.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/ptr.h"
#include "toolkitutil/toolkitapp.h"
#include "net/socket/ipaddress.h"
#include "distributedtools/distributedjobscheduler.h"
#include "distributedtools/distributedjobs/distributedappjob.h"
#include "distributedtools/shareddircreator.h"
#include "util/guid.h"
#include "net/tcpserver.h"

namespace Net
{
    class TcpClientConnection;
}
namespace IO
{
    class XmlReader;
    class XmlWriter;
}

//------------------------------------------------------------------------------
namespace DistributedTools
{
class DistributedJobServiceApp : public ToolkitUtil::ToolkitApp
{
public:
    
    /// constructor
    DistributedJobServiceApp();

    /// open the application
    virtual bool Open();
    /// run the application
    virtual void Run();
    /// handle the received data
    virtual void HandleCommands();
    /// close the application
    virtual void Close();
    /// parse command line arguments
    virtual bool ParseCmdLineArgs();

protected:

    Ptr<SharedDirCreator> sharedDirCreator;

private:

    enum ServiceState
    {
        Busy,
        Ready,
        Reserved,
        Inactive
    };

    /// structure of job with the origin ip adress
    struct ClientJob
    {
        Ptr<DistributedJob> job;
        Net::IpAddress origin;
    };

    /// handle command xml node
    virtual void HandleCommandNode(const Ptr<IO::XmlReader> & reader, const Ptr<Net::TcpClientConnection> & connection);
    /// checks given job buffer for finished jobs, removes finished job elements
    virtual void CheckJobBuffer(Util::Array<ClientJob> & buffer, const Ptr<Net::TcpClientConnection> & connection, const Ptr<IO::XmlWriter> & writer,  bool notifyAboutLastJob = false);
    /// returns current state as String
    Util::StringAtom GetServiceStateAsString();
    /// updates the service state 
    virtual void UpdateServiceState();

    bool verboseFlag;

    DistributedJobScheduler scheduler;
    Ptr<Net::TcpServer> tcpserver;
    Ptr<IO::Console> console;
    Util::Array<ClientJob> jobBuffer;
    Util::Array<ClientJob> initializeJobBuffer;
    Util::Array<ClientJob> finalizeJobBuffer;

    Util::Array<Util::StringAtom> xmlServiceStateNames;
    ServiceState state;
    Util::Guid exlusiveClientID;
};
} // namespace DistributedTools