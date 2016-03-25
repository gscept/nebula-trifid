//------------------------------------------------------------------------------
//  leveleditor2/properties/editornavmeshproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/editornavmeshproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "messaging/staticmessagehandler.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegameprotocol.h"
#include "modelutil/modelattributes.h"
#include "modelutil/modelphysics.h"
#include "leveleditor2app.h"
#include "editorproperty.h"

#include "qfiledialog.h"
#include <QPlastiqueStyle>
#include "navigation/navigationserver.h"
#include "leveleditor2entitymanager.h"

using namespace LevelEditor2;
using namespace BaseGameFeature;
using namespace Math;
using namespace ToolkitUtil;
using namespace Game;
using namespace Messaging;


__Handler(EditorNavMeshProperty,CreateNavMesh)
{

    const Util::Array<Ptr<Game::Entity>> & ents = msg->GetEntities();
    
    Util::String guids;    
    Util::String areaGuids;
    for(int i = 0 ; i < ents.Size(); i++)
    {
        if(ents[i]->HasAttr(Attr::NavMeshAreaFlags))
        {
            areaGuids += ents[i]->GetGuid(Attr::EntityGuid).AsString();
            areaGuids += ";";        
        }
        else
        {
            guids += ents[i]->GetGuid(Attr::EntityGuid).AsString();
            guids += ";";        
        }
        
    }
    obj->GetEntity()->SetString(Attr::EntityReferences,guids);
    obj->GetEntity()->SetString(Attr::AreaEntityReferences,areaGuids);
    Util::Guid newGuid;
    newGuid.Generate();
    obj->GetEntity()->SetString(Attr::NavMeshData,"nav:" + newGuid.AsString());
    obj->UpdateMesh();    
}

__Handler(EditorNavMeshProperty,UpdateNavMesh)
{ 
    obj->UpdateMesh();
}
__Handler(EditorNavMeshProperty,SaveNavMesh)
{ 
    obj->SaveMesh();
}
__Dispatcher(EditorNavMeshProperty)
{
    //__Handle(EditorNavMeshProperty,GetAttribute);
    //__Handle(EditorNavMeshProperty,SetAttributes);
    //__Handle(EditorNavMeshProperty,SetTransform);
    __Handle(EditorNavMeshProperty, CreateNavMesh);
    __Handle(EditorNavMeshProperty, UpdateNavMesh);
    __Handle(EditorNavMeshProperty, SaveNavMesh);
}


namespace LevelEditor2
{

__ImplementClass(LevelEditor2::EditorNavMeshProperty, 'NVPR', Game::Property);


//------------------------------------------------------------------------------
/**
*/
EditorNavMeshProperty::EditorNavMeshProperty()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::SetupAcceptedMessages()
{	
	this->RegisterMessage(SetAttribute::Id);
	this->RegisterMessage(GetAttribute::Id);
	this->RegisterMessage(SetTransform::Id);
    this->RegisterMessage(LevelEditor2::CreateNavMesh::Id);
    this->RegisterMessage(LevelEditor2::UpdateNavMesh::Id);
    this->RegisterMessage(LevelEditor2::SaveNavMesh::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, Render);
}

//------------------------------------------------------------------------------
/**
*/
void 
EditorNavMeshProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	__Dispatch(EditorNavMeshProperty,this,msg);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::OnRender()
{
    if(this->GetEntity()->GetBool(Attr::IsSelected))
    {        
        Navigation::NavigationServer::Instance()->RenderDebug(this->GetEntity()->GetString(Attr::NavMeshData));
		LevelEditor2App::Instance()->GetWindow()->GetUi().actionUpdateNavMesh->setEnabled(true);
    }
	else
	{
		LevelEditor2App::Instance()->GetWindow()->GetUi().actionUpdateNavMesh->setEnabled(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::OnStart()
{
    Property::OnStart();
	if (entity->GetString(Attr::NavMeshData).IsValid())
	{
		if(IO::IoServer::Instance()->FileExists(IO::URI(entity->GetString(Attr::NavMeshData))))
		{
			Navigation::NavigationServer::Instance()->LoadNavigationData(entity->GetString(Attr::NavMeshData), IO::URI(entity->GetString(Attr::NavMeshData)));
		}
		else
		{
			n_warning("Navmesh %s not found, did you forget to commit? regenerating\n", entity->GetString(Attr::NavMeshData).AsCharPtr());
			this->UpdateMesh();
		}
	}
		
    
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::OnDeactivate()
{
    if(Navigation::NavigationServer::Instance()->HasNavMesh(this->entity->GetString(Attr::NavMeshData)))
    {
        Navigation::NavigationServer::Instance()->DeleteNavMesh(this->entity->GetString(Attr::NavMeshData));
    }   
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::UpdateMesh()
{
    if(Navigation::NavigationServer::Instance()->HasNavMesh(this->entity->GetString(Attr::NavMeshData)))
    {
        Navigation::NavigationServer::Instance()->DeleteNavMesh(this->entity->GetString(Attr::NavMeshData));
    }
    Util::String guids = entity->GetString(Attr::EntityReferences);
    Util::Array<Util::String> guidarray = guids.Tokenize(";");
    
    Util::Array<Ptr<Game::Entity>> ents;
    for(IndexT i = 0 ; i < guidarray.Size() ; i++)
    {        
        Util::Guid g;
        g = guidarray[i];
		Ptr<Game::Entity> ent = BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attr::Attribute(Attr::EntityGuid, g));
		if (ent.isvalid())
		{
			ents.Append(ent);
		}        
		else
		{
			n_warning("Missing entity reference for navmesh\n");
		}
    }
    
    Ptr<Navigation::RecastUtil> recast = Navigation::RecastUtil::Create();

    recast->SetAgentParams(entity->GetFloat(Attr::AgentHeight),entity->GetFloat(Attr::AgentRadius),entity->GetFloat(Attr::AgentMaxClimb));    
    recast->SetBoundingBox(Level::Instance()->GetBoundingBox());
    recast->SetCellHeight(entity->GetFloat(Attr::CellHeight));
    recast->SetCellSize(entity->GetFloat(Attr::CellSize));
    recast->SetMaxSlope(entity->GetFloat(Attr::MaxSlope));
    recast->SetMaxEdgeError(entity->GetFloat(Attr::MaxEdgeError));
    recast->SetDetailSampleDist(entity->GetFloat(Attr::DetailSampleDist));
    recast->SetDetailSampleMaxError(entity->GetFloat(Attr::DetailSampleMaxError));
    recast->SetMaxEdgeLength(entity->GetInt(Attr::MaxEdgeLength));
    recast->SetRegionMinSize(entity->GetInt(Attr::RegionMinSize));    
    recast->SetRegionMergeSize(entity->GetInt(Attr::RegionMergeSize));    

    recast->SetupConfig();
    Util::String meshString;

    for(IndexT i = 0 ; i< ents.Size() ; i++)
    {
        if(ents[i]->HasAttr(Attr::Graphics))
        {
            Util::String uri;
            Util::String modelName = ents[i]->GetString(Attr::Graphics);

            // check if model has a physics mesh first
            IO::URI physUri = "root:work/assets/" + modelName + ".physics";
            Ptr<IO::Stream> phstream = IO::IoServer::Instance()->CreateStream(physUri);
            Ptr<ModelPhysics> phs = ModelPhysics::Create();
            phs->Load(phstream);
            if(phs->GetExportMode() == UsePhysics)
            {		
                uri = phs->GetPhysicsMesh();
                recast->AddMesh(uri,ents[i]->GetMatrix44(Attr::Transform));
            }
            else
            {
                IO::URI grMeshUri = "msh:" + modelName + ".nvx2";
                recast->AddMesh(grMeshUri,ents[i]->GetMatrix44(Attr::Transform));
                uri = "msh:" + modelName + ".nvx2";
            }			
            meshString += "\"" + uri + "\"=\"";             
            meshString.AppendMatrix44(ents[i]->GetMatrix44(Attr::Transform));
            meshString += "\" ";
        }
    }
    this->GetEntity()->SetString(Attr::NavMeshMeshString,meshString);
	this->GetEntity()->SetFloat4(Attr::NavMeshCenter, recast->GetBoundingBox().center());
	this->GetEntity()->SetFloat4(Attr::NavMeshExtends, recast->GetBoundingBox().extents());

    Util::String areaGuids = entity->GetString(Attr::AreaEntityReferences);
    Util::Array<Util::String> areaGuidarray = areaGuids.Tokenize(";");
	Util::String navGuid = entity->GetGuid(Attr::EntityGuid).AsString();
    for(IndexT i = 0 ; i < areaGuidarray.Size() ; i++)
    {        
        Util::Guid g;
        g = areaGuidarray[i];
		Ptr<Game::Entity> ent = BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attr::Attribute(Attr::EntityGuid, g));
		if (ent.isvalid())
		{
			Util::String meshString = ent->GetString(Attr::NavMeshMeshString);
			if (meshString.IsEmpty())
			{
				meshString.Append(navGuid);
				meshString += ";";
			}
			else if (meshString.FindStringIndex(navGuid) == InvalidIndex)
			{				
				meshString += navGuid;
				meshString += ";";				
			}
			ent->SetString(Attr::NavMeshMeshString, meshString);
			recast->AddConvexArea(ent);
		}        
    }

    Util::Blob data = recast->GenerateNavMeshData();
    if(data.IsValid())
    {        
        if(data.Size())
        {
            Navigation::NavigationServer::Instance()->LoadNavigationData(entity->GetString(Attr::NavMeshData),data);            
        }		
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EditorNavMeshProperty::SaveMesh()
{

	if (!Navigation::NavigationServer::Instance()->HasNavMesh(entity->GetString(Attr::NavMeshData)))
	{
		n_warning("NavMesh %s not found, this should never happen\n", entity->GetString(Attr::NavMeshData).AsCharPtr());
		return;
	}
    const Util::Blob * blob = Navigation::NavigationServer::Instance()->GetInternalData(entity->GetString(Attr::NavMeshData));

    IO::URI uri(entity->GetString(Attr::NavMeshData));
	if (!uri.IsValid())
	{
		n_warning("Illegal filename for navmesh: %s\n", uri.AsString().AsCharPtr());
		return;
	}    
    IO::IoServer::Instance()->CreateDirectory(IO::URI("nav:"));
    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(uri);

    Ptr<IO::BinaryWriter> writer = IO::BinaryWriter::Create();
    writer->SetStream(stream);
    writer->SetStreamByteOrder(Platform::GetPlatformByteOrder(ToolkitUtil::Platform::Win32));
    if (writer->Open())
    {
        writer->WriteBlob(*blob);
    }
    writer->Close();

}

}