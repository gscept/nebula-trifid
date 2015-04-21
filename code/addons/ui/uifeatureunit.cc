//------------------------------------------------------------------------------
//  ui/uifeatureunit.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/uifeatureunit.h"
#include "graphics/graphicsserver.h"
#include "framesync/framesynctimer.h"
#include "input/inputserver.h"
#include "uiinputhandler.h"
#include "uirtplugin.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "uiattrs/uiattributes.h"
#include "db/reader.h"
#include "db/dbserver.h"
#include "io/ioserver.h"
#include "scripting/scriptserver.h"
#include "audioprotocol.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace UI
{
__ImplementClass(UiFeatureUnit, 'SUFU' , Game::FeatureUnit);
__ImplementSingleton(UiFeatureUnit);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
UiFeatureUnit::UiFeatureUnit() :autoLoad(true)
{	
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
UiFeatureUnit::~UiFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::OnActivate()
{
    // create ui server
    this->server = UI::UiServer::Create();
    this->server->Setup();

	RenderModules::RTPluginRegistry::Instance()->RegisterRTPlugin(&UI::UiRTPlugin::RTTI);

    // create and attach input handler
	this->inputHandler = UI::UiInputHandler::Create();
	Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::Gui, this->inputHandler.cast<Input::InputHandler>());

    // create event hander
    this->uiEventHandler = UiEventHandler::Create();

	// load default ui fonts and layouts
	if (this->autoLoad)
	{
		this->LoadUITables();
	}


    FeatureUnit::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::LoadUITables()
{
	// open database
	Ptr<Db::Reader> reader = Db::Reader::Create();
	reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());

	// try to load any fonts listed in db table
	// table is not required to exist
	reader->SetTableName("_Template__Ui_Fonts");
	if (reader->Open())
	{
		// table exists 
		int numRows = reader->GetNumRows();		
		int index;
		for (index = 0; index < numRows; index++)
		{
			reader->SetToRow(index);

			Util::String fontFile = reader->GetString(Attr::Id);
			Util::String fontFamily = reader->GetString(Attr::UIFontFamily);
			UI::FontStyle fontStyle = (UI::FontStyle)reader->GetInt(Attr::UIFontStyle);
			UI::FontWeight fontWeight = (UI::FontWeight)reader->GetInt(Attr::UIFontWeight);
            bool autoLoad = reader->GetBool(Attr::AutoLoad);
            if(autoLoad)
            {
			    if (fontFamily.Length())
			    {
				    this->server->LoadFont(fontFile, fontFamily, fontStyle, fontWeight);
			    }
			    else
			    {
				    this->server->LoadFont(fontFile);
			    }
            }
		}
		reader->Close();
	}
	Ptr<Db::Reader> lreader = Db::Reader::Create();
	lreader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
	lreader->SetTableName("_Template__Ui_Layouts");
	if (lreader->Open())
	{
		// table exists 
		int numRows = lreader->GetNumRows();
		int index;
		for (index = 0; index < numRows; index++)
		{
			lreader->SetToRow(index);

			Util::String layoutFile = lreader->GetString(Attr::Id);
			Util::String layoutName = lreader->GetString(Attr::Name);            
            bool autoLoad = lreader->GetBool(Attr::AutoLoad);
            if(autoLoad)
            {
			    this->server->CreateLayout(layoutName, layoutFile); 
            }
		}
		lreader->Close();
	}
	Ptr<Db::Reader> sreader = Db::Reader::Create();
	sreader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
	sreader->SetTableName("_Template__Ui_Scripts");
	if (sreader->Open())
	{
		// table exists 
		int numRows = sreader->GetNumRows();
		int index;
		for (index = 0; index < numRows; index++)
		{
			sreader->SetToRow(index);

			Util::String layoutFile = sreader->GetString(Attr::Id);
            bool autoLoad = sreader->GetBool(Attr::AutoLoad);
            if(autoLoad)
            {
			    Scripting::ScriptServer::Instance()->EvalScript(layoutFile);	
			    if (Scripting::ScriptServer::Instance()->HasError())
			    {
				    n_printf("Error evaluating %s:\n%s\n", layoutFile.AsCharPtr(), Scripting::ScriptServer::Instance()->GetError().AsCharPtr());
			    }
            }
		}
		sreader->Close();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::OnDeactivate()
{
	Input::InputServer::Instance()->RemoveInputHandler(this->inputHandler.cast<Input::InputHandler>());
	this->inputHandler = 0;

	RenderModules::RTPluginRegistry::Instance()->UnregisterRTPlugin(&UI::UiRTPlugin::RTTI);

	// discard render module
    this->server->Discard();
    this->server = 0;

    this->groups.Clear();

    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::OnFrame()
{    
    // perform on-frame action on render module
	this->server->Update();

    FeatureUnit::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<UI::UiLayout>
UiFeatureUnit::CreateLayout( const Util::String& layoutId, const Resources::ResourceId& resId)
{
    Ptr<UI::UiLayout> layout = UiServer::Instance()->CreateLayout(layoutId, resId);
    return layout;
}


//------------------------------------------------------------------------------
/**
*/
const Ptr<UI::UiLayout>& 
UiFeatureUnit::GetLayout( const Util::String& layoutId )
{
    return this->server->GetLayoutById(layoutId);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::FreeLayout(const Util::String& layoutId)
{
	return this->server->FreeLayout(layoutId);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::SetUIEventHandler( const Ptr<UiEventHandler>& handler )
{
	this->uiEventHandler = handler;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::ProcessEvent( const UiEvent& event )
{
    n_assert(this->uiEventHandler.isvalid());
    this->uiEventHandler->HandleEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::LoadFont(const Util::String& resource, const Util::String& family, FontStyle style, FontWeight weight)
{
	this->server->LoadFont(resource,family,style,weight);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::LoadFont(const Util::String& resource)
{
	this->server->LoadFont(resource);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::LoadAllFonts(const Util::String& root)
{
	Util::Array<Util::String> directories = IO::IoServer::Instance()->ListDirectories(root, "*",true);

	IndexT dirIndex, fileIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		Util::String directory = directories[dirIndex];
		Util::Array<Util::String> files = IO::IoServer::Instance()->ListFiles(directory, "*.otf", true);

		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			this->server->LoadFont(files[fileIndex]);
		}
		files = IO::IoServer::Instance()->ListFiles(directory, "*.ttf", true);

		for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
		{
			this->server->LoadFont(files[fileIndex]);
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::RegisterUIRenderPlugin( const Ptr<UI::UiPlugin> & uiRt )
{
	this->plugins.Append(uiRt);
	uiRt->OnRegister();
}

//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::UnregisterUIRenderPlugin( const Ptr<UI::UiPlugin> & uiRt )
{	
	IndexT idx = this->plugins.FindIndex(uiRt);
	n_assert(idx != InvalidIndex);
	uiRt->OnUnregister();
	this->plugins.EraseIndex(idx);
}

//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::RenderPlugins( const Ptr<Frame::FrameBatch>& frameBatch )
{
	for(Util::Array<Ptr<UI::UiPlugin>>::Iterator iter = this->plugins.Begin() ; iter != this->plugins.End() ; iter++)
	{
		(*iter)->OnRenderFrameBatch(frameBatch);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::AddGroup( uint key, const Ptr<UiElementGroup>& group )
{
    n_assert(!this->groups.Contains(key));
    this->groups.Add(key, group);
}

//------------------------------------------------------------------------------
/**
*/
void 
UiFeatureUnit::RemoveGroup( uint key, const Ptr<UiElementGroup>& group )
{
    n_assert(this->groups.Contains(key));
    this->groups.Erase(key);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<UiElementGroup>& 
UiFeatureUnit::GetGroup( uint key ) const
{
    return this->groups[key];
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::SetAutoload(bool enable)
{
	this->autoLoad = enable;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<UiElement>
UiFeatureUnit::GetLayoutElement(const Util::String & layout, const Util::String & element)
{
	n_assert(UiFeatureUnit::Instance()->HasLayout(layout));
	Ptr<UiLayout> lay = UiFeatureUnit::Instance()->GetLayout(layout);
	return lay->GetElement(element);
}

//------------------------------------------------------------------------------
/**
*/
void
UiFeatureUnit::SetLayoutVisible(const Util::String & layoutid, bool visible)
{
	n_assert(UiFeatureUnit::Instance()->HasLayout(layoutid));
	if (visible)
	{
		UiFeatureUnit::Instance()->GetLayout(layoutid)->Show();
	}
	else
	{
		UiFeatureUnit::Instance()->GetLayout(layoutid)->Hide();
	}
	
}
}; // namespace UI
