//------------------------------------------------------------------------------
// distributedjobserviceapp.cc
// (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "distributedjobserviceapp.h"
#include "timing/timer.h"
#include "io/xmlwriter.h"
#include "io/xmlreader.h"
#include "io/memorystream.h"
#include "distributedtools/distributedjobs/distributeddeletedirectorycontentjob.h"
#include "net/messageclientconnection.h"

using namespace ToolkitUtil;
using namespace Net;
using namespace IO;
using namespace Util;

namespace DistributedTools
{
//------------------------------------------------------------------------------
/**
    Constructor	
*/
DistributedJobServiceApp::DistributedJobServiceApp():
    state(Inactive)
{
    this->xmlServiceStateNames.Clear();
    this->xmlServiceStateNames.Append("busy");
    this->xmlServiceStateNames.Append("ready");
    this->xmlServiceStateNames.Append("reserved");
    this->xmlServiceStateNames.Append("inactive");
}

//------------------------------------------------------------------------------
/**
    Open the application	
*/
bool
DistributedJobServiceApp::Open()
{
    if (!ToolkitApp::Open())
    {
        return false;
    }
    if (!this->ParseCmdLineArgs())
    {
        return false;
    }
    if (!this->SetupProjectInfo())
    {
        return false;
    }

    // setup a tcp server
    this->tcpserver = TcpServer::Create();
    IpAddress ipAdress;
    ipAdress.SetHostName("any");
    ipAdress.SetPort(2200);
    this->tcpserver->SetAddress(ipAdress);
    this->tcpserver->SetClientConnectionClass(MessageClientConnection::RTTI);
    this->tcpserver->Open();
    
    // setup singleton for creation of SharedDirControl objects 
    this->sharedDirCreator = SharedDirCreator::Create();
    this->sharedDirCreator->SetProjectInfo(this->projectInfo);

    return true;
}


//------------------------------------------------------------------------------
/**
	Parse the commandline args and setup application parameters
*/
bool
DistributedJobServiceApp::ParseCmdLineArgs()
{
    if (!ToolkitApp::ParseCmdLineArgs())
    {
        return false;
    }

    this->verboseFlag = this->args.GetBoolFlag("-verbose");

    // setup scheduler
    this->scheduler.SetMaxParallelLocalJobs(this->args.GetInt("-processors",1));
    this->scheduler.PrintLocalJobOutput(false);
    if (!this->scheduler.Open())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Run the application	
*/
void
DistributedJobServiceApp::Run()
{
    ToolkitApp::Run();
    bool shutDownRequested = false;
    while (!shutDownRequested)
    {
        // handle incoming commands
        this->HandleCommands();
        
        // update scheduler
        if (this->scheduler.IsOpen())
        {
            if (this->scheduler.HasActiveJobs())
            {
                this->scheduler.Update();
            }
        }
        this->UpdateServiceState();
        Timing::Sleep(0.01);
    }
    if (this->waitForKey)
    {
        n_printf("Press <Enter> to continue!\n");
        while (!Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
    }
}
//------------------------------------------------------------------------------
/**
    Updates the state of the service	
*/
void
DistributedJobServiceApp::UpdateServiceState()
{
    if (this->scheduler.IsOpen())
    {
        if (!this->scheduler.HasActiveJobs())
        {
            if (this->state != Reserved)
            {
                this->state = Ready;
            }
        }
        else
        {
            this->state = Busy;
        }
    }
    else
    {
        this->state = Inactive;
    }
}

//------------------------------------------------------------------------------
/**
    Handle the commands that arrives from the tcp server	
*/
void
DistributedJobServiceApp::HandleCommands()
{
    n_assert(this->tcpserver.isvalid());
    n_assert(this->tcpserver->IsOpen());
    
    const Array<Ptr<TcpClientConnection>> connections = this->tcpserver->Recv();
    IndexT i;
    for (i = 0; i < connections.Size(); i++)
    {
        Ptr<XmlReader> reader = XmlReader::Create();
        reader->SetStream(connections[i]->GetRecvStream());
        if (reader->Open())
        {    
            // check if incoming data is a service command
            if (reader->HasNode("/Command"))
            {
                reader->SetToNode("/Command");
                this->HandleCommandNode(reader,connections[i]);
            }
            reader->Close();
        }
        reader = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Close the application	
*/
void
DistributedJobServiceApp::Close()
{
    this->tcpserver->Close();
    this->tcpserver = 0;
    
    this->scheduler.Close();

    ToolkitApp::Close();
}

//------------------------------------------------------------------------------
/**
    Handles a command node in the given xml reader that points to this node.
*/
void
DistributedJobServiceApp::HandleCommandNode(const Ptr<XmlReader> & reader,const Ptr<TcpClientConnection> & connection)
{
    n_assert("Command" == reader->GetCurrentNodeName());
    if (reader->HasAttr("name"))
    {
        Guid receivedID = Guid::FromString(reader->GetString("clientID"));
        // handle different commands here
        if ("DetailedStateRequest" == reader->GetString("name"))    
        {
            if (this->verboseFlag)
            {
                n_printf("Received Command: 'DetailedStateRequest' from %s ( %s )\n",connection->GetClientAddress().GetHostName().AsCharPtr(),connection->GetClientAddress().GetHostAddr().AsCharPtr());
            }

            // get guid from command, or generate one
            Guid guid;
            if (reader->HasAttr("guid"))
            {
                guid = Guid::FromString(reader->GetString("guid"));
            }
            else
            {
                guid.Generate();
            }

            StringAtom state = this->GetServiceStateAsString();
            if (this->state == Reserved
                && this->exlusiveClientID == receivedID)
            {
                state = "exclusiveReady";
            }
            Ptr<XmlWriter> writer = XmlWriter::Create();
            writer->SetStream(connection->GetSendStream());
            if (writer->Open())
            {
                writer->BeginNode("Command");
                writer->SetString("name","DetailedState");
                writer->SetString("value",state.Value());
                writer->SetString("guid",guid.AsString());
                    writer->BeginNode("Processorcount");
                    writer->SetInt("value",this->args.GetInt("-processors",1));
                    writer->EndNode();
                    // check job buffer for finished jobs
                    this->CheckJobBuffer(this->initializeJobBuffer,connection,writer);
                    if (this->finalizeJobBuffer.Size() > 0)
                    {
                        this->CheckJobBuffer(this->jobBuffer,connection,writer);
                    }
                    else
                    {
                        this->CheckJobBuffer(this->jobBuffer,connection,writer,true);
                    }
                    this->CheckJobBuffer(this->finalizeJobBuffer,connection,writer,true);
                writer->EndNode();
                writer->Close();
                connection->Send();
            }
            writer = 0;
        }
        else if ("StateRequest" == reader->GetString("name"))
        {
            if (this->verboseFlag)
            {
                n_printf("Received Command: 'StateRequest' from %s ( %s )\n",connection->GetClientAddress().GetHostName().AsCharPtr(),connection->GetClientAddress().GetHostAddr().AsCharPtr());
            }

            // get guid from command, or generate one
            Guid guid;
            if(reader->HasAttr("guid"))
            {
                guid = Guid::FromString(reader->GetString("guid"));
            }
            else
            {
                guid.Generate();
            }
            
            StringAtom state = this->GetServiceStateAsString();
            if (this->state == Reserved
                && this->exlusiveClientID == receivedID)
            {
                state = "exclusiveReady";
            }
            Ptr<XmlWriter> writer = XmlWriter::Create();
            writer->SetStream(connection->GetSendStream());
            if (writer->Open())
            {
                writer->BeginNode("Command");
                    writer->SetString("name","CurrentState");
                    writer->SetString("value",state.Value());
                    writer->SetString("guid",guid.AsString());
                    // check job buffer for finished jobs
                    this->CheckJobBuffer(this->initializeJobBuffer,connection,writer);
                    if (this->finalizeJobBuffer.Size() > 0)
                    {
                        this->CheckJobBuffer(this->jobBuffer,connection,writer);
                    }
                    else
                    {
                        this->CheckJobBuffer(this->jobBuffer,connection,writer,true);
                    }
                    this->CheckJobBuffer(this->finalizeJobBuffer,connection,writer,true);
                writer->EndNode();
                writer->Close();
                connection->Send();
            }
            writer = 0;
        }
        else if ("JobCommand" == reader->GetString("name"))
        {
            n_printf("Received Command: 'JobCommand' from %s ( %s )\n",connection->GetClientAddress().GetHostName().AsCharPtr(),connection->GetClientAddress().GetHostAddr().AsCharPtr());
            Ptr<DistributedJob> job = DistributedJob::Create();
            bool failed = false;
            if (this->state == Reserved)
            {
                failed = (receivedID != this->exlusiveClientID);
            }
            else if (this->state != Ready)
            {
                failed = true;
            }
            if (reader->SetToFirstChild())
            {
                if (reader->GetCurrentNodeName() == "AppJob")
                {
                    job = DistributedAppJob::Create();
                    job = job.upcast<DistributedJob>();
                }
                else if (reader->GetCurrentNodeName() == "DeleteDirectoryContentJob")
                {
                    job = DistributedDeleteDirectoryContentJob::Create();
                    job = job.upcast<DistributedJob>();
                }
                job->SetupFromXml(reader);
                reader->SetToParent();
            }
            else
            {
                failed = true;
            }
            if (job->IsAbleToRun() && scheduler.IsOpen() && !scheduler.HasActiveJobs() && !failed)
            {
                if (this->state == Ready)
                {
                    this->exlusiveClientID = receivedID;
                    this->state = Reserved;
                }
                // tell the job, that it is running on a slave machine
                // before adding it to the scheduler
                job->SetRunningOnSlave(true);

                ClientJob clientJob;
                clientJob.job = job;
                clientJob.origin = connection->GetClientAddress();
                this->jobBuffer.Append(clientJob); 
                this->scheduler.AttachJob(job);

                // send success message
                Ptr<XmlWriter> writer = XmlWriter::Create();
                writer->SetStream(connection->GetSendStream());
                if(writer->Open())
                {
                    writer->BeginNode("Command");
                    writer->SetString("name","JobAppendSuccess");
                    writer->SetString("jobGuid",job->GetGuid().AsString());
                    writer->EndNode();
                    writer->Close();
                    connection->Send();
                }
                writer = 0;
            }
            else
            {
                n_printf("[Job #%s] Failed to add to scheduler.\n",job->GetIdentifier().AsCharPtr());
                n_printf("\tJob is able to run: %s\n",String::FromBool(job->IsAbleToRun()).AsCharPtr());
                n_printf("\tScheduler is open: %s\n",String::FromBool(scheduler.IsOpen()).AsCharPtr());
                n_printf("\tScheduler is running: %s\n",String::FromBool(scheduler.HasActiveJobs()).AsCharPtr());
                // send failed message
                Ptr<XmlWriter> writer = XmlWriter::Create();
                writer->SetStream(connection->GetSendStream());
                if (writer->Open())
                {
                    writer->BeginNode("Command");
                    writer->SetString("name","JobAppendFailed");
                    writer->SetString("jobGuid",job->GetGuid().AsString());
                    StringAtom reason("unknown");
                    if (this->state == Busy)
                    {
                        reason = "busy";
                    }
                    else if (receivedID != this->exlusiveClientID && Reserved == this->state)
                    {
                        reason = "reserved";
                    }
                    writer->SetString("reason",reason.Value());
                    writer->EndNode();
                    writer->Close();
                    connection->Send();
                }
                writer = 0;
            }
        }
        else if ("RunJobs" == reader->GetString("name"))
        {
            n_printf("Received Command: 'RunJobs' from %s ( %s )\n",connection->GetClientAddress().GetHostName().AsCharPtr(),connection->GetClientAddress().GetHostAddr().AsCharPtr());
            if (receivedID == this->exlusiveClientID)
            {
                this->state = Busy;
                if (!this->scheduler.HasActiveJobs())
                {
                    // parse the initialize and finalize jobs and add them to the service
                    if (reader->HasNode("InitializeJobs"))
                    {
                        if (reader->SetToFirstChild("InitializeJobs"))
                        {
                            if (reader->SetToFirstChild())
                            {
                                do 
                                {
                                    Ptr<DistributedJob> job = DistributedJob::Create();
                                    if (reader->GetCurrentNodeName() == "AppJob")
                                    {
                                        Ptr<DistributedAppJob> tmpjob = DistributedAppJob::Create();
                                        job = tmpjob.cast<DistributedJob>();
                                    }
                                    else if (reader->GetCurrentNodeName() == "DeleteDirectoryContentJob")
                                    {
                                        Ptr<DistributedDeleteDirectoryContentJob> tmpjob = DistributedDeleteDirectoryContentJob::Create();
                                        job = tmpjob.cast<DistributedJob>();
                                    }
                                    else if (reader->GetCurrentNodeName() == "Job")
                                    {
                                        job = DistributedJob::Create();
                                    }
                                    job->SetupFromXml(reader);
                                    // tell the job, that it is running on a slave machine
                                    // before adding it to the scheduler
                                    job->SetRunningOnSlave(true);
                                    this->scheduler.AppendInitializeJob(job);
                                    ClientJob clientJob;
                                    clientJob.job = job;
                                    clientJob.origin = connection->GetClientAddress();
                                    this->initializeJobBuffer.Append(clientJob);
                                    job = 0;
                                } while (reader->SetToNextChild());
                                reader->SetToParent();
                            }
                        }
                    }
                    if (reader->HasNode("FinalizeJobs"))
                    {
                        if (reader->SetToFirstChild("FinalizeJobs"))
                        {
                            if (reader->SetToFirstChild())
                            {
                                do 
                                {
                                    Ptr<DistributedJob> job = DistributedJob::Create();
                                    if (reader->GetCurrentNodeName() == "AppJob")
                                    {
                                        Ptr<DistributedAppJob> tmpjob = DistributedAppJob::Create();
                                        job = tmpjob.cast<DistributedJob>();
                                    }
                                    else if (reader->GetCurrentNodeName() == "DeleteDirectoryContentJob")
                                    {
                                        Ptr<DistributedDeleteDirectoryContentJob> tmpjob = DistributedDeleteDirectoryContentJob::Create();
                                        job = tmpjob.cast<DistributedJob>();
                                    }
                                    else if (reader->GetCurrentNodeName() == "Job")
                                    {
                                        Ptr<DistributedJob> job = DistributedJob::Create();
                                    }
                                    job->SetupFromXml(reader);
                                    // tell the job, that it is running on a slave machine
                                    // before adding it to the scheduler
                                    job->SetRunningOnSlave(true);
                                    this->scheduler.AppendFinalizeJob(job);
                                    ClientJob clientJob;
                                    clientJob.job = job;
                                    clientJob.origin = connection->GetClientAddress();
                                    this->finalizeJobBuffer.Append(clientJob);
                                    job = 0;
                                } while (reader->SetToNextChild());
                                reader->SetToParent();
                            }
                        }
                    }
                    this->scheduler.RunJobs();
                }
            }
        }
        else if ("Banned" == reader->GetString("name"))
        {
            n_printf("Received Command: 'Banned' from %s ( %s )\n",connection->GetClientAddress().GetHostName().AsCharPtr(),connection->GetClientAddress().GetHostAddr().AsCharPtr());
            // Check the service state.
            // Cleanup the service if the state is ready, or is currently reserved
            // for this connection.
            if (Ready == this->state || receivedID == this->exlusiveClientID)
            {
                this->exlusiveClientID = Guid();
                this->finalizeJobBuffer.Clear();
                this->initializeJobBuffer.Clear();
                this->jobBuffer.Clear();
                this->scheduler.Close();
                this->scheduler.Open();
                this->state = Ready;

                // Cleaned everything up. Send an answer, that cleanup was successful
                // send failed message
                Ptr<XmlWriter> writer = XmlWriter::Create();
                writer->SetStream(connection->GetSendStream());
                if (writer->Open())
                {
                    writer->BeginNode("Command");
                    writer->SetString("name","CleanedUp");
                    writer->EndNode();
                    writer->Close();
                    connection->Send();
                }
                writer = 0;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Checks all jobs of job buffer. If a job has finished, append a finished node
    to the given writer
*/
void
DistributedJobServiceApp::CheckJobBuffer(Util::Array<ClientJob> & buffer, const Ptr<Net::TcpClientConnection> & connection,  const Ptr<IO::XmlWriter> & writer, bool notifyAboutLastJob)
{
    n_assert(writer->IsOpen());
    IndexT i = 0;
    while (i < buffer.Size())
    {
    	if (buffer[i].origin == connection->GetClientAddress())
        {
            // first write the job output if there is any
            if (buffer[i].job->HasOutputContent())
            {
                writer->BeginNode("JobOutput");
                writer->SetString("jobGuid",buffer[i].job->GetGuid().AsString());
                writer->WriteContent(buffer[i].job->DequeueOutputContent());
                writer->EndNode();
            }
            if (DistributedJob::Finished == buffer[i].job->GetCurrentState())
            {
                this->UpdateServiceState();
                
                writer->BeginNode("JobFinished");
                writer->SetString("jobGuid",buffer[i].job->GetGuid().AsString());
                if (notifyAboutLastJob)
                {
                    writer->SetBool("wasLastJob",buffer.Size()==1);
                }
                writer->EndNode();

                buffer.EraseIndex(i);
            }
            else if (DistributedJob::JobError == buffer[i].job->GetCurrentState())
            {
                this->UpdateServiceState();
                
                writer->BeginNode("JobError");
                writer->SetString("jobGuid",buffer[i].job->GetGuid().AsString());
                writer->EndNode();
                
                buffer.EraseIndex(i);
            }
            else
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Get the current service state as string.
*/
StringAtom
DistributedJobServiceApp::GetServiceStateAsString()
{
    return this->xmlServiceStateNames[this->state];
}

} // namespace DistributedTools