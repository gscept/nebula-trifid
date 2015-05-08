//------------------------------------------------------------------------------
//  scripting/scriptingfeature.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptingfeature/scriptingfeature.h"
#include "scripting/lua/luaserver.h"
#include "game/entity.h"
#include "scriptingfeature/properties/scriptingproperty.h"
#include "scriptingfeature/properties/scriptingtriggerproperty.h"
#include "managers/entitymanager.h"
#include "managers/factorymanager.h"
#include "io/textreader.h"

namespace ScriptingFeature
{
	__ImplementClass(ScriptingFeature::ScriptingFeatureUnit, 'SRFU' , Game::FeatureUnit);
	__ImplementSingleton(ScriptingFeatureUnit);



//------------------------------------------------------------------------------
/**
*/
ScriptingFeatureUnit::ScriptingFeatureUnit()
{	
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ScriptingFeatureUnit::~ScriptingFeatureUnit()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingFeatureUnit::OnActivate()
{
	n_assert(!this->scriptingServer.isvalid());
	this->scriptingServer = Scripting::LuaServer::Create();
	this->scriptingServer->Open();	
	FeatureUnit::OnActivate();
    this->scriptingServer->CreateFunctionTable("oninit_table");
	this->scriptingServer->CreateFunctionTable("onframe_table");
	this->scriptingServer->CreateFunctionTable("oncollision_table");
	this->scriptingServer->CreateFunctionTable("oninput_table");
	this->scriptingServer->CreateFunctionTable("onanimevent_table");
	this->scriptingServer->CreateFunctionTable("ontriggerenter_table");
	this->scriptingServer->CreateFunctionTable("ontriggerexit_table");
	this->scriptingServer->CreateFunctionTable("ontriggerinside_table");

	this->inputHandler = ScriptingFeature::ScriptingInputHandler::Create();
	Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::DynUi, this->inputHandler.upcast<Input::InputHandler>());
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingFeatureUnit::OnDeactivate()
{
	Input::InputServer::Instance()->RemoveInputHandler(this->inputHandler.upcast<Input::InputHandler>());
	this->inputHandler = 0;
	n_assert(this->scriptingServer.isvalid());
	this->scriptingServer->Close();
	this->scriptingServer = 0;
	FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingFeatureUnit::OnRenderDebug()
{

}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingFeatureUnit::OnFrame()
{    

	FeatureUnit::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
bool
ScriptingFeatureUnit::EvalScriptFile(const IO::URI & file)
{
	n_assert2(IO::IoServer::Instance()->FileExists(file), "file not found");	
	bool ret = false;
	Ptr<IO::TextReader> reader = IO::TextReader::Create();
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(file);
	reader->SetStream(stream);
	stream->SetAccessMode(IO::Stream::ReadAccess);
	if (reader->Open())
	{
		Util::String script = reader->ReadAll();
		ret = this->scriptingServer->Eval(script);
		if (this->scriptingServer->HasError())
		{
			n_printf("Error evaluating script file: %s\n%s\n", file.AsString().AsCharPtr(), this->scriptingServer->GetError().AsCharPtr());
		}
		reader->Close();
	}
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
bool
ScriptingFeatureUnit::EvalScript(const Util::String & scriptString)
{
	bool ret = this->scriptingServer->Eval(scriptString);
	if (this->scriptingServer->HasError())
	{
		n_printf("Error evaluating script\n%s\n", this->scriptingServer->GetError().AsCharPtr());
	}
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
bool
ScriptingFeatureUnit::EvalEntityScript(const Ptr<Game::Entity> & entity, const Util::String & functionName, const Util::String & scriptString)
{
	bool ret = this->scriptingServer->EvalWithParameter(scriptString, functionName, entity->GetUniqueId());
	if (this->scriptingServer->HasError())
	{
		n_printf("Error evaluating entity script function %s\n%s\n", functionName.AsCharPtr(), this->scriptingServer->GetError().AsCharPtr());
	}
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingFeatureUnit::SetAttribute(unsigned int entityid, const Util::String& attr, const Util::String & value)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(entityid);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Attr::AttrId att(attr);
	Attr::Attribute newatt(att);
	newatt.SetValueFromString(value);
	entity->SetAttr(newatt);

}

//------------------------------------------------------------------------------
/**
*/
Util::String ScriptingFeatureUnit::GetAttribute(unsigned int entityid, const Util::String& attr)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(entityid);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	return entity->GetAttr(Attr::AttrId(attr)).ValueAsString();
}

//------------------------------------------------------------------------------
/**
*/
unsigned int 
ScriptingFeatureUnit::CreateEntityByTemplate(const Util::String& category, const Util::String& templ)
{
	Ptr<Game::Entity> ent = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate(category,templ);
	BaseGameFeature::EntityManager::Instance()->AttachEntity(ent);
	return ent->GetUniqueId();
}

}; // namespace Scripting
