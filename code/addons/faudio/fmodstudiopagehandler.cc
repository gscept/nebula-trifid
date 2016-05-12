//------------------------------------------------------------------------------
//  fmodstudiopagehandler.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/fmodstudiopagehandler.h"
#include "faudio/audiodevice.h"

#include <fmod.hpp>
#include <fmod_studio.hpp>

using namespace FAudio;
using namespace Debug;

__ImplementClass(Debug::FmodStudioPageHandler, 'CAPH', Http::HttpRequestHandler);

using namespace Util;
using namespace Http;
using namespace IO;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
FmodStudioPageHandler::FmodStudioPageHandler()
{
    this->SetName("FmodStudio");
    this->SetDesc("show information about fmod studio subsystem");
    this->SetRootLocation("fmodstudio");
}

//------------------------------------------------------------------------------
/**
*/
void
FmodStudioPageHandler::HandleRequest(const Ptr<HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());

    // check if a soundbank data is requested
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("area"))
    {
        const String &area = query["area"];
        if(area == "banklist")
        {
            request->SetStatus(this->HandleBankInfoRequest(request->GetResponseContentStream()));
        }
        else if(area == "memorylist")
        {
            request->SetStatus(this->HandleMemoryInfoRequest(request->GetResponseContentStream()));
        }
        else if(area == "eventlist")
        {
            request->SetStatus(this->HandleEventInfoRequest(request->GetResponseContentStream()));
        }
        return;
    }

    // build HTML page
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT FMOD Studio Subsystem");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "FMOD Studio Subsystem");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");

		FMOD::System * lwlsystem = NULL;
		FMOD::Studio::System * system = NULL;
		system = AudioDevice::Instance()->GetSystem();
		system->getLowLevelSystem(&lwlsystem);

		int currentUsed, maxUsed, totalUsed;
		lwlsystem->getSoundRAM(&currentUsed, &maxUsed, &totalUsed);
                
        htmlWriter->Element(HtmlElement::Heading3, "Statictics");

        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/fmodstudio?area=memorylist");
                    htmlWriter->Element(HtmlElement::Anchor, "Memory");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String("Current in use: ") + String::FromInt(currentUsed) + " bytes");
				htmlWriter->Element(HtmlElement::TableData, String("Max used: ") + String::FromInt(maxUsed) + " bytes");
				htmlWriter->Element(HtmlElement::TableData, String("Available: ") + String::FromInt(totalUsed) + " bytes");
            htmlWriter->End(HtmlElement::TableRow);                
        htmlWriter->End(HtmlElement::Table);

        htmlWriter->LineBreak();

        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "Type");
                htmlWriter->Element(HtmlElement::TableHeader, "Count");
            htmlWriter->End(HtmlElement::TableRow);                

            int tmp = 0;
			system->getBankCount(&tmp);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/fmodstudio?area=banklist");
                    htmlWriter->Element(HtmlElement::Anchor, "Banks");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tmp) );
            htmlWriter->End(HtmlElement::TableRow);

        htmlWriter->End(HtmlElement::Table);

        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
FmodStudioPageHandler::HandleMemoryInfoRequest(const Ptr<IO::Stream>& responseStream)
{
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseStream);
    htmlWriter->SetTitle("NebulaT Projects Info");
    if(!htmlWriter->Open()) return HttpStatus::InternalServerError;

    htmlWriter->Element(HtmlElement::Heading1, "Memory");
    htmlWriter->AddAttr("href", "/index.html");
    htmlWriter->Element(HtmlElement::Anchor, "Home");
    htmlWriter->LineBreak();
    htmlWriter->AddAttr("href", "/fmodstudio");
    htmlWriter->Element(HtmlElement::Anchor, "FMOD Studio Home");
    htmlWriter->LineBreak();
    htmlWriter->LineBreak();

	/*
    FMOD::EventSystem *eventSystem = FmodCoreAudio::FmodAudioDevice::Instance()->GetEventSystem();

    unsigned int memoryused;
    unsigned int memoryused_array[FMOD_MEMTYPE_MAX];
    eventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &memoryused, memoryused_array);

    htmlWriter->AddAttr("border", "1");
    htmlWriter->AddAttr("rules", "cols");
    htmlWriter->Begin(HtmlElement::Table);

        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableHeader, "Memory-Type");
            htmlWriter->Element(HtmlElement::TableHeader, "Byte");
            htmlWriter->Element(HtmlElement::TableHeader, "MB");
        htmlWriter->End(HtmlElement::TableRow);                
        
        int index;
        for(index = 0; index < FMOD_MEMTYPE_MAX; ++index)
        {
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, FmodAudioDevice::FmodMemtypeToString((FMOD_MEMTYPE)index));
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(memoryused_array[index]));
                String mb;
                mb.Format("%7.3f", (float)memoryused_array[index]/(1024.0f*1024.0f));
                htmlWriter->Element(HtmlElement::TableData, mb);
            htmlWriter->End(HtmlElement::TableRow);
        }
*/
    htmlWriter->End(HtmlElement::Table);
    htmlWriter->Close();
    return HttpStatus::OK;       
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
FmodStudioPageHandler::HandleBankInfoRequest(const Ptr<Stream>& responseStream)
{
	/*
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseStream);
    htmlWriter->SetTitle("NebulaT Banks Info");
    if(!htmlWriter->Open()) return HttpStatus::InternalServerError;

    htmlWriter->Element(HtmlElement::Heading1, "Banks");
    htmlWriter->AddAttr("href", "/index.html");
    htmlWriter->Element(HtmlElement::Anchor, "Home");
    htmlWriter->LineBreak();
    htmlWriter->AddAttr("href", "/fmodstudio");
    htmlWriter->Element(HtmlElement::Anchor, "FMOD Studio Home");
    htmlWriter->LineBreak();
    htmlWriter->LineBreak();

	FMOD::Studio::System * system = NULL;
	system = AudioDevice::Instance()->GetSystem();
    
    int numProjects;
    eventSystem->getNumProjects(&numProjects);
    int index;
    FMOD::EventProject *project;

    htmlWriter->AddAttr("border", "1");
    htmlWriter->AddAttr("rules", "cols");
    htmlWriter->Begin(HtmlElement::Table);

        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableHeader, "Name");
            htmlWriter->Element(HtmlElement::TableHeader, "Groups");
            htmlWriter->Element(HtmlElement::TableHeader, "Events");
        htmlWriter->End(HtmlElement::TableRow);                
        
        for(index = 0; index < numProjects; ++index)
        {
                        
            eventSystem->getProjectByIndex(index, &project);
			FMOD_EVENT_PROJECTINFO foo;
            project->getInfo(&foo);
            int groups, events;
            project->getNumGroups(&groups);
            project->getNumEvents(&events);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, foo.name);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(groups));
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(events));
            htmlWriter->End(HtmlElement::TableRow);
        }

    htmlWriter->End(HtmlElement::Table);
    htmlWriter->Close();
	*/
    return HttpStatus::OK;       
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodStudioPageHandler::DumpGroup(char *projectName, Util::String groupName, FMOD::EventGroup *group, const Ptr<HtmlPageWriter> &htmlWriter) const
{
	/*
    FMOD::Event *event;
    FMOD_EVENT_INFO info;
    char *cGroupName, *eventName;
    int eventIndex, numEvents, tmp;
    group->getInfo(&tmp, &cGroupName);

    if(groupName != "" )
    {
        groupName = groupName + "/" + cGroupName;
    }
    else
    {
        groupName = cGroupName;
    }

    group->getNumEvents(&numEvents);
    for(eventIndex = 0; eventIndex < numEvents; ++eventIndex)
    {
        group->getEventByIndex(eventIndex, FMOD_EVENT_INFOONLY, &event);
        event->getInfo(&tmp, &eventName, &info);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, projectName);
            htmlWriter->Element(HtmlElement::TableData, groupName);
            htmlWriter->Element(HtmlElement::TableData, eventName);
            htmlWriter->Element(HtmlElement::TableData, String::FromInt(info.channelsplaying));
            htmlWriter->Element(HtmlElement::TableData, String::FromInt(info.instancesactive));
            htmlWriter->Element(HtmlElement::TableData, String::FromInt(info.lengthms));
        htmlWriter->End(HtmlElement::TableRow);
    }

    int numGroups;
    group->getNumGroups(&numGroups);
    for(int groupIndex = 0; groupIndex < numGroups; ++groupIndex)
    {
        FMOD::EventGroup *g;
        group->getGroupByIndex(groupIndex, false, &g);
        DumpGroup(projectName, groupName, g, htmlWriter);
    }
	*/
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
FmodStudioPageHandler::HandleEventInfoRequest(const Ptr<Stream>& responseStream)
{
	/*
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseStream);
    htmlWriter->SetTitle("NebulaT Projects Info");
    if(!htmlWriter->Open()) return HttpStatus::InternalServerError;

    htmlWriter->Element(HtmlElement::Heading1, "Events");
    htmlWriter->AddAttr("href", "/index.html");
    htmlWriter->Element(HtmlElement::Anchor, "Home");
    htmlWriter->LineBreak();
    htmlWriter->AddAttr("href", "/fmodcoreaudio");
    htmlWriter->Element(HtmlElement::Anchor, "CoreAudio Home");
    htmlWriter->LineBreak();
    htmlWriter->LineBreak();

    FMOD::EventSystem *eventSystem = FmodCoreAudio::FmodAudioDevice::Instance()->GetEventSystem();
    
    int numProjects, numGroups;
    int projectIndex, groupIndex;
    eventSystem->getNumProjects(&numProjects);
    FMOD::EventProject *project;
    FMOD::EventGroup *group;

    htmlWriter->AddAttr("border", "1");
    htmlWriter->AddAttr("rules", "cols");
    htmlWriter->Begin(HtmlElement::Table);

        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableHeader, "Project");
            htmlWriter->Element(HtmlElement::TableHeader, "Group");
            htmlWriter->Element(HtmlElement::TableHeader, "Event");
            htmlWriter->Element(HtmlElement::TableHeader, "Channels Playing");
            htmlWriter->Element(HtmlElement::TableHeader, "Instances Active");
            htmlWriter->Element(HtmlElement::TableHeader, "Length ms");
        htmlWriter->End(HtmlElement::TableRow);                
        
        for(projectIndex = 0; projectIndex < numProjects; ++projectIndex)
        {
            eventSystem->getProjectByIndex(projectIndex, &project);
			FMOD_EVENT_PROJECTINFO foo;
			project->getInfo(&foo);

            project->getNumGroups(&numGroups);
            for(groupIndex = 0; groupIndex < numGroups; ++groupIndex)
            {
                project->getGroupByIndex(groupIndex, false, &group);
                DumpGroup(foo.name, "", group, htmlWriter);
            }
        }
		
    htmlWriter->End(HtmlElement::Table);
    htmlWriter->Close();
	*/
    return HttpStatus::OK;       
}
