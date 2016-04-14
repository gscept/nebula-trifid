//------------------------------------------------------------------------------
//  scriptingfeature/properties/scriptproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptingfeature/properties/scriptingproperty.h"
#include "scriptingfeature/scriptingfeature.h"
#include "scripting/scriptserver.h"
#include "physicsfeature/physicsprotocol.h"
#include "scriptingfeature/scriptingprotocol.h"
#include "io/ioserver.h"
#include "input/inputserver.h"
#include "input/keyboard.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"


namespace ScriptingFeature
{
	__ImplementClass(ScriptingFeature::ScriptingProperty, 'SFPR', Game::Property);

using namespace Scripting;
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ScriptingProperty::ScriptingProperty():
	onFrame(false),	
	onCollision(false),
	onInput(false),
	onAnimEvent(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ScriptingProperty::~ScriptingProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(PhysicsFeature::Collision::Id);
	this->RegisterMessage(ScriptingFeature::OverrideScript::Id);
	this->RegisterMessage(ScriptingFeature::LoadScript::Id);
	this->RegisterMessage(GraphicsFeature::AnimEventTriggered::Id);
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will check attached scripts
*/
void
ScriptingProperty::OnDeactivate()
{
    Property::OnDeactivate();
	if (this->onFrame)
	{
		ScriptServer::Instance()->UnregisterFunction("onframe_table",this->entity->GetUniqueId());
	}
	if (this->onCollision)
	{
		ScriptServer::Instance()->UnregisterFunction("oncollision_table",this->entity->GetUniqueId());
	}
	if (this->onInput)
	{
		ScriptServer::Instance()->UnregisterFunction("oninput_table", this->entity->GetUniqueId());
	}

	if (this->onAnimEvent)
	{
		ScriptServer::Instance()->UnregisterFunction("onanimevent_table", this->entity->GetUniqueId());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::OnActivate()
{
	Property::OnActivate();

	// get attribute
	String script = "scr:" + this->GetEntity()->GetString(Attr::EntityScript) + ".lua";	
	this->SetupScript(script);	
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::OnStart()
{
	Property::OnStart();
	if(this->entityScript.Length() > 0 && ScriptServer::Instance()->ScriptHasFunction(this->entityScript,"oninit"))
	{
		ScriptingFeature::ScriptingFeatureUnit::Instance()->EvalEntityScript(this->GetEntity(), "oninit", this->entityScript);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptingProperty::SetupScript( const Util::String& script )
{
	n_assert(script.IsValid());
	
	// open file stream
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(script);

	// open file
	if (stream->Open())
	{
		// read all data
		void* data = stream->Map();
		SizeT size = stream->GetSize();

		// map to string
		this->entityScript.Clear();
		this->entityScript.AppendRange((char*)data, size);

		if (this->entityScript.IsValid())
		{
			this->DetectDefaultFunctions();
		}

		// close stream
		stream->Close();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::DetectDefaultFunctions()
{
    ScriptServer* server = ScriptServer::Instance();

    // register onframe callback if it exists
	this->onFrame = server->ScriptHasFunction(this->entityScript, "onframe");
	SCRIPT_ERROR_CHECK("RegisterFunction", "onframe");
	if (this->onFrame)
	{
		server->RegisterFunction(this->entityScript, "onframe", "onframe_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}

    // register oncollision callback if it exists
	this->onCollision = server->ScriptHasFunction(this->entityScript, "oncollision");
	SCRIPT_ERROR_CHECK("RegisterFunction", "oncollision");
	if (this->onCollision)
	{
		server->RegisterFunction(this->entityScript, "oncollision", "oncollision_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}

    // register oninput callback if it exists
	this->onInput = server->ScriptHasFunction(this->entityScript, "oninput");
	SCRIPT_ERROR_CHECK("RegisterFunction", "oninput");
	if (this->onInput)
	{
		server->RegisterFunction(this->entityScript, "oninput", "oninput_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}

	// register onanimevent callback if it exists
	this->onAnimEvent = server->ScriptHasFunction(this->entityScript, "onanimevent");
	SCRIPT_ERROR_CHECK("RegisterFunction", "onanimevent");
	if (this->onAnimEvent)
	{
		server->RegisterFunction(this->entityScript, "onanimevent", "onanimevent_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}
}

//------------------------------------------------------------------------------
/**
*/
void ScriptingProperty::RegisterScriptfunction(const Util::String& script, const Util::String& function )
{
	Util::String table = function + "_table";
	ScriptServer::Instance()->RegisterFunction(script,function,table,this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::CallRegisteredfunction(const Util::String & table, const Util::String& function)
{
	ScriptServer::Instance()->CallFunction(table,this->entity->GetUniqueId());			
	SCRIPT_ERROR_CHECK("CallRegisteredfunction", "");
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::SetupCallbacks()
{
	this->entity->RegisterPropertyCallback(this, BeginFrame);		
}

//------------------------------------------------------------------------------
/**    
*/
void
ScriptingProperty::OnBeginFrame()
{
	if (this->onFrame)
	{
		ScriptServer::Instance()->CallFunction("onframe_table", this->entity->GetUniqueId());
	}    
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	// handle collision message
	if (msg->CheckId(PhysicsFeature::Collision::Id))
	{
		if (this->onCollision)
		{ 			
			const Ptr<PhysicsFeature::Collision> cmsg = msg.cast<PhysicsFeature::Collision>();
            SizeT numPoints = cmsg->GetContact()->GetPoints().Size();
            IndexT pointIndex;
            if (cmsg->GetContact()->GetDepth() > this->entity->GetFloat(Attr::CollisionDepthThreshold))
            {
                for (pointIndex = 0; pointIndex < numPoints; pointIndex++)
                {
                    Util::Array<Util::Variant> args;
                    args.Append(cmsg->GetOtherEntity()->GetUniqueId());
                    args.Append(cmsg->GetContact()->GetPoints()[pointIndex]);
                    args.Append(cmsg->GetContact()->GetNormalVectors()[pointIndex]);
                    ScriptServer::Instance()->CallFunction("oncollision_table", this->entity->GetUniqueId(), args);
					SCRIPT_ERROR_CHECK("HandleMessage", "PhysicsFeature::Collision");
                }   
            }                     
		}
	}
	else if (msg->CheckId(ScriptingFeature::LoadScript::Id))
	{
		// cast message
		Ptr<ScriptingFeature::LoadScript> message = msg.downcast<ScriptingFeature::LoadScript>();

		// get attribute
		String script = "scr:" + this->GetEntity()->GetString(message->GetScript()) + ".lua";
		this->SetupScript(script);
	}
	else if (msg->CheckId(ScriptingFeature::OverrideScript::Id))
	{
		// cast message
		Ptr<ScriptingFeature::OverrideScript> message = msg.downcast<ScriptingFeature::OverrideScript>();

		// setup script directly from message
		this->entityScript = message->GetScript();
		this->DetectDefaultFunctions();
	}
	else if (msg->CheckId(GraphicsFeature::AnimEventTriggered::Id))
	{
		if (this->onAnimEvent)
		{
			Ptr<GraphicsFeature::AnimEventTriggered> message = msg.downcast<GraphicsFeature::AnimEventTriggered>();
			Util::String eventname = message->GetName();
			ScriptServer::Instance()->CallFunction("onanimevent_table", this->entity->GetUniqueId(), eventname);
			SCRIPT_ERROR_CHECK("HandleMessage", "AnimEventTriggered");

		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::OnKeyDown(Input::Key::Code key)
{
	if (this->onInput)
	{
		Util::Array<Util::Variant> args;
		args.Append(Input::Key::ToString(key));
		args.Append(false);
				
		ScriptServer::Instance()->CallFunction("oninput_table", this->entity->GetUniqueId(), args);				
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::OnKeyUp(Input::Key::Code key)
{
	if (this->onInput)
	{
		Util::Array<Util::Variant> args;
		args.Append(Input::Key::ToString(key));
		args.Append(true);

		ScriptServer::Instance()->CallFunction("oninput_table", this->entity->GetUniqueId(), args);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingProperty::OnMouseDown(Input::MouseButton::Code button)
{
	if (this->onInput)
	{
		Util::Array<Util::Variant> args;
		args.Append(Input::MouseButton::ToString(button));
		args.Append(false);

		ScriptServer::Instance()->CallFunction("oninput_table", this->entity->GetUniqueId(), args);
	}
}
}; // namespace ScriptingFeature
