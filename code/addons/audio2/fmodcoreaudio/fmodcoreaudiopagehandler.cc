//------------------------------------------------------------------------------
//  fmodcoreaudiopagehandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audio2/fmodcoreaudio/fmodcoreaudiopagehandler.h"
#include "audio2/fmodcoreaudio/fmodaudiodevice.h"

#include <fmod_event.hpp>

using namespace FmodCoreAudio;
using namespace Debug;

__ImplementClass(Debug::FmodCoreAudioPageHandler, 'CAPH', Http::HttpRequestHandler);

using namespace Util;
using namespace Http;
using namespace IO;
using namespace Resources;
using namespace FmodCoreAudio;

//------------------------------------------------------------------------------
/**
*/
FmodCoreAudioPageHandler::FmodCoreAudioPageHandler()
{
    this->SetName("FmodCoreAudio");
    this->SetDesc("show information about CoreAudio fmod subsystem");
    this->SetRootLocation("fmodcoreaudio");
}

//------------------------------------------------------------------------------
/**
*/
void
FmodCoreAudioPageHandler::HandleRequest(const Ptr<HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());

    // check if a soundbank data is requested
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("area"))
    {
        const String &area = query["area"];
        if(area == "projectlist")
        {
            request->SetStatus(this->HandleProjectsInfoRequest(request->GetResponseContentStream()));
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
    htmlWriter->SetTitle("NebulaT FMOD CoreAudio Subsystem");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "FMOD CoreAudio Subsystem");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");

        FMOD::EventSystem *eventSystem = FmodCoreAudio::FmodAudioDevice::Instance()->GetEventSystem();

        unsigned int memoryused;
        eventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &memoryused, NULL);
        float mb = (float)memoryused / (1024.0f * 1024.0f);

        htmlWriter->Element(HtmlElement::Heading3, "Statictics");

        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/fmodcoreaudio?area=memorylist");
                    htmlWriter->Element(HtmlElement::Anchor, "Memory");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(memoryused) + " bytes");
                htmlWriter->Element(HtmlElement::TableData, String::FromFloat(mb) + " MB");
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

            int tmp;
            eventSystem->getNumProjects(&tmp);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/fmodcoreaudio?area=projectlist");
                    htmlWriter->Element(HtmlElement::Anchor, "Projects");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tmp) );
            htmlWriter->End(HtmlElement::TableRow);

            eventSystem->getNumEvents(&tmp);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/fmodcoreaudio?area=eventlist");
                    htmlWriter->Element(HtmlElement::Anchor, "Events");
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tmp) );
            htmlWriter->End(HtmlElement::TableRow);

            eventSystem->getNumCategories(&tmp);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Categories");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tmp) );
            htmlWriter->End(HtmlElement::TableRow);

            eventSystem->getNumReverbPresets(&tmp);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Reverb Presets");
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
FmodCoreAudioPageHandler::HandleMemoryInfoRequest(const Ptr<IO::Stream>& responseStream)
{
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseStream);
    htmlWriter->SetTitle("NebulaT Projects Info");
    if(!htmlWriter->Open()) return HttpStatus::InternalServerError;

    htmlWriter->Element(HtmlElement::Heading1, "Memory");
    htmlWriter->AddAttr("href", "/index.html");
    htmlWriter->Element(HtmlElement::Anchor, "Home");
    htmlWriter->LineBreak();
    htmlWriter->AddAttr("href", "/fmodcoreaudio");
    htmlWriter->Element(HtmlElement::Anchor, "CoreAudio Home");
    htmlWriter->LineBreak();
    htmlWriter->LineBreak();

    FMOD::EventSystem *eventSystem = FmodCoreAudio::FmodAudioDevice::Instance()->GetEventSystem();
/*
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
FmodCoreAudioPageHandler::HandleProjectsInfoRequest(const Ptr<Stream>& responseStream)
{
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseStream);
    htmlWriter->SetTitle("NebulaT Projects Info");
    if(!htmlWriter->Open()) return HttpStatus::InternalServerError;

    htmlWriter->Element(HtmlElement::Heading1, "Projects");
    htmlWriter->AddAttr("href", "/index.html");
    htmlWriter->Element(HtmlElement::Anchor, "Home");
    htmlWriter->LineBreak();
    htmlWriter->AddAttr("href", "/fmodcoreaudio");
    htmlWriter->Element(HtmlElement::Anchor, "CoreAudio Home");
    htmlWriter->LineBreak();
    htmlWriter->LineBreak();

    FMOD::EventSystem *eventSystem = FmodCoreAudio::FmodAudioDevice::Instance()->GetEventSystem();
    
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
    return HttpStatus::OK;       
}

//------------------------------------------------------------------------------
/**
*/
void 
FmodCoreAudioPageHandler::DumpGroup(char *projectName, Util::String groupName, FMOD::EventGroup *group, const Ptr<HtmlPageWriter> &htmlWriter) const
{
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
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
FmodCoreAudioPageHandler::HandleEventInfoRequest(const Ptr<Stream>& responseStream)
{
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
    return HttpStatus::OK;       
}
