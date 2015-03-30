//------------------------------------------------------------------------------
//  MACHINE GENERATED, DON'T EDIT!
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scripting/scriptserver.h"
#include "scripting/arg.h"
#include "graphicsfeatureprotocol.h"


__ImplementClass(Commands::CameraDistance, '/CAD', Scripting::Command);
__ImplementClass(Commands::CameraFocus, '/AFC', Scripting::Command);
__ImplementClass(Commands::CameraOrbit, '/CAO', Scripting::Command);
__ImplementClass(Commands::CameraReset, '/ARE', Scripting::Command);
__ImplementClass(Commands::InputFocus, '/NFC', Scripting::Command);
__ImplementClass(Commands::SetGraphicsVisible, '/GFV', Scripting::Command);
__ImplementClass(Commands::SetOverwriteColor, '/EOC', Scripting::Command);
__ImplementClass(Commands::PlayAnimClip, '/ACL', Scripting::Command);
__ImplementClass(Commands::SetSkinVisible, '/SKV', Scripting::Command);
__ImplementClass(Commands::AnimStop, '/NST', Scripting::Command);
__ImplementClass(Commands::CreateGraphicsEffectUpVec, '/GEU', Scripting::Command);
__ImplementClass(Commands::CreateAttachmentEffect, '/RAE', Scripting::Command);
namespace GraphicsFeature
{
    __ImplementClass(GraphicsFeature::CameraDistance, 'MCAD', Messaging::Message);
    __ImplementMsgId(CameraDistance);
    __ImplementClass(GraphicsFeature::CameraFocus, 'CAFC', Messaging::Message);
    __ImplementMsgId(CameraFocus);
    __ImplementClass(GraphicsFeature::CameraOrbit, 'MCAO', Messaging::Message);
    __ImplementMsgId(CameraOrbit);
    __ImplementClass(GraphicsFeature::CameraReset, 'CARE', Messaging::Message);
    __ImplementMsgId(CameraReset);
    __ImplementClass(GraphicsFeature::GetGraphicsEntities, 'GGEN', Messaging::Message);
    __ImplementMsgId(GetGraphicsEntities);
    __ImplementClass(GraphicsFeature::GetLightEntity, 'GLEN', Messaging::Message);
    __ImplementMsgId(GetLightEntity);
    __ImplementClass(GraphicsFeature::InputFocus, 'INFC', Messaging::Message);
    __ImplementMsgId(InputFocus);
    __ImplementClass(GraphicsFeature::SetGraphicsVisible, 'MGFV', Messaging::Message);
    __ImplementMsgId(SetGraphicsVisible);
    __ImplementClass(GraphicsFeature::SetOverwriteColor, 'SEOC', Messaging::Message);
    __ImplementMsgId(SetOverwriteColor);
    __ImplementClass(GraphicsFeature::SetShaderVariable, 'SSHV', Messaging::Message);
    __ImplementMsgId(SetShaderVariable);
    __ImplementClass(GraphicsFeature::SetMaterialVariable, 'SMVB', Messaging::Message);
    __ImplementMsgId(SetMaterialVariable);
    __ImplementClass(GraphicsFeature::PlayAnimClip, 'PACL', Messaging::Message);
    __ImplementMsgId(PlayAnimClip);
    __ImplementClass(GraphicsFeature::SetSkinVisible, 'SSKV', Messaging::Message);
    __ImplementMsgId(SetSkinVisible);
    __ImplementClass(GraphicsFeature::AnimPlay, 'ANPL', Messaging::Message);
    __ImplementMsgId(AnimPlay);
    __ImplementClass(GraphicsFeature::AnimRewind, 'ANRE', Messaging::Message);
    __ImplementMsgId(AnimRewind);
    __ImplementClass(GraphicsFeature::AnimStop, 'ANST', Messaging::Message);
    __ImplementMsgId(AnimStop);
    __ImplementClass(GraphicsFeature::AnimUpdate, 'ANUP', Messaging::Message);
    __ImplementMsgId(AnimUpdate);
    __ImplementClass(GraphicsFeature::CutsceneControl, 'CUCO', Messaging::Message);
    __ImplementMsgId(CutsceneControl);
    __ImplementClass(GraphicsFeature::CreateGraphicsEffectUpVec, 'CGEU', Messaging::Message);
    __ImplementMsgId(CreateGraphicsEffectUpVec);
    __ImplementClass(GraphicsFeature::CreateAttachmentEffect, 'CRAE', Messaging::Message);
    __ImplementMsgId(CreateAttachmentEffect);
    __ImplementClass(GraphicsFeature::AreGraphicsEntityResourcesLoaded, 'AGEL', Messaging::Message);
    __ImplementMsgId(AreGraphicsEntityResourcesLoaded);
    __ImplementClass(GraphicsFeature::GraphicsReady, 'GRRY', Messaging::Message);
    __ImplementMsgId(GraphicsReady);
} // GraphicsFeature

namespace Commands
{
//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureProtocol::Register()
{
    Scripting::ScriptServer* scriptServer = Scripting::ScriptServer::Instance();
    scriptServer->RegisterCommand("cameradistance", CameraDistance::Create());
    scriptServer->RegisterCommand("camerafocus", CameraFocus::Create());
    scriptServer->RegisterCommand("cameraorbit", CameraOrbit::Create());
    scriptServer->RegisterCommand("camerareset", CameraReset::Create());
    scriptServer->RegisterCommand("inputfocus", InputFocus::Create());
    scriptServer->RegisterCommand("setgraphicsvisible", SetGraphicsVisible::Create());
    scriptServer->RegisterCommand("setoverwritecolor", SetOverwriteColor::Create());
    scriptServer->RegisterCommand("playanimclip", PlayAnimClip::Create());
    scriptServer->RegisterCommand("setskinvisible", SetSkinVisible::Create());
    scriptServer->RegisterCommand("animstop", AnimStop::Create());
    scriptServer->RegisterCommand("creategraphicseffectupvec", CreateGraphicsEffectUpVec::Create());
    scriptServer->RegisterCommand("createattachmenteffect", CreateAttachmentEffect::Create());
}

//------------------------------------------------------------------------------
/**
*/
void
CameraDistance::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("RelativeDistanceChange", Scripting::Arg::Float);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CameraDistance::GetHelp() const
{
    return "Sets the camera distance.";
}
//------------------------------------------------------------------------------
/**
*/
bool
CameraDistance::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    float RelativeDistanceChange = this->args.GetArgValue(1).GetFloat();
    this->Callback(___entityId, RelativeDistanceChange);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CameraDistance::Callback(uint ___entityId, float RelativeDistanceChange)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CameraDistance> msg = GraphicsFeature::CameraDistance::Create();
	msg->SetRelativeDistanceChange(RelativeDistanceChange);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
CameraFocus::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("ObtainFocus", Scripting::Arg::Bool);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CameraFocus::GetHelp() const
{
    return "Gives Camera focus to the game entity. The entity needs to have a cameraproperty on it which will be activated.";
}
//------------------------------------------------------------------------------
/**
*/
bool
CameraFocus::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    bool ObtainFocus = this->args.GetArgValue(1).GetBool();
    this->Callback(___entityId, ObtainFocus);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CameraFocus::Callback(uint ___entityId, bool ObtainFocus)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CameraFocus> msg = GraphicsFeature::CameraFocus::Create();
	msg->SetObtainFocus(ObtainFocus);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
CameraOrbit::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("HorizontalRotation", Scripting::Arg::Float);
    this->args.AddArg("VerticalRotation", Scripting::Arg::Float);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CameraOrbit::GetHelp() const
{
    return "";
}
//------------------------------------------------------------------------------
/**
*/
bool
CameraOrbit::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    float HorizontalRotation = this->args.GetArgValue(1).GetFloat();
    float VerticalRotation = this->args.GetArgValue(2).GetFloat();
    this->Callback(___entityId, HorizontalRotation, VerticalRotation);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CameraOrbit::Callback(uint ___entityId, float HorizontalRotation, float VerticalRotation)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CameraOrbit> msg = GraphicsFeature::CameraOrbit::Create();
	msg->SetHorizontalRotation(HorizontalRotation);
	msg->SetVerticalRotation(VerticalRotation);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
CameraReset::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CameraReset::GetHelp() const
{
    return "Resets the camera";
}
//------------------------------------------------------------------------------
/**
*/
bool
CameraReset::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    this->Callback(___entityId);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CameraReset::Callback(uint ___entityId)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CameraReset> msg = GraphicsFeature::CameraReset::Create();
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
InputFocus::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("ObtainFocus", Scripting::Arg::Bool);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
InputFocus::GetHelp() const
{
    return "Sets input focus on specified game entity";
}
//------------------------------------------------------------------------------
/**
*/
bool
InputFocus::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    bool ObtainFocus = this->args.GetArgValue(1).GetBool();
    this->Callback(___entityId, ObtainFocus);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void InputFocus::Callback(uint ___entityId, bool ObtainFocus)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::InputFocus> msg = GraphicsFeature::InputFocus::Create();
	msg->SetObtainFocus(ObtainFocus);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
SetGraphicsVisible::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Visible", Scripting::Arg::Bool);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
SetGraphicsVisible::GetHelp() const
{
    return "Shows or hides all graphics entities of a game entity.";
}
//------------------------------------------------------------------------------
/**
*/
bool
SetGraphicsVisible::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    bool Visible = this->args.GetArgValue(1).GetBool();
    this->Callback(___entityId, Visible);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void SetGraphicsVisible::Callback(uint ___entityId, bool Visible)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::SetGraphicsVisible> msg = GraphicsFeature::SetGraphicsVisible::Create();
	msg->SetVisible(Visible);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
SetOverwriteColor::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Color", Scripting::Arg::Float4);
    this->args.AddArg("NodeName", Scripting::Arg::String);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
SetOverwriteColor::GetHelp() const
{
    return "";
}
//------------------------------------------------------------------------------
/**
*/
bool
SetOverwriteColor::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    const Math::float4& Color = this->args.GetArgValue(1).GetFloat4();
    const Util::String& NodeName = this->args.GetArgValue(2).GetString();
    this->Callback(___entityId, Color, NodeName);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void SetOverwriteColor::Callback(uint ___entityId, const Math::float4& Color, const Util::String& NodeName)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::SetOverwriteColor> msg = GraphicsFeature::SetOverwriteColor::Create();
	msg->SetColor(Color);
	msg->SetNodeName(NodeName);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
PlayAnimClip::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Clip", Scripting::Arg::String);
    this->args.AddArg("LoopCount", Scripting::Arg::Float);
    this->args.AddArg("Queue", Scripting::Arg::Bool);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
PlayAnimClip::GetHelp() const
{
    return "Play animation clip";
}
//------------------------------------------------------------------------------
/**
*/
bool
PlayAnimClip::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    const Util::String& Clip = this->args.GetArgValue(1).GetString();
    float LoopCount = this->args.GetArgValue(2).GetFloat();
    bool Queue = this->args.GetArgValue(3).GetBool();
    this->Callback(___entityId, Clip, LoopCount, Queue);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void PlayAnimClip::Callback(uint ___entityId, const Util::String& Clip, float LoopCount, bool Queue)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::PlayAnimClip> msg = GraphicsFeature::PlayAnimClip::Create();
	msg->SetClip(Clip);
	msg->SetLoopCount(LoopCount);
	msg->SetQueue(Queue);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
SetSkinVisible::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Skin", Scripting::Arg::String);
    this->args.AddArg("Visible", Scripting::Arg::Bool);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
SetSkinVisible::GetHelp() const
{
    return "Show a skin for a character model";
}
//------------------------------------------------------------------------------
/**
*/
bool
SetSkinVisible::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    const Util::String& Skin = this->args.GetArgValue(1).GetString();
    bool Visible = this->args.GetArgValue(2).GetBool();
    this->Callback(___entityId, Skin, Visible);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void SetSkinVisible::Callback(uint ___entityId, const Util::String& Skin, bool Visible)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::SetSkinVisible> msg = GraphicsFeature::SetSkinVisible::Create();
	msg->SetSkin(Skin);
	msg->SetVisible(Visible);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
AnimStop::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
AnimStop::GetHelp() const
{
    return "";
}
//------------------------------------------------------------------------------
/**
*/
bool
AnimStop::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    this->Callback(___entityId);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void AnimStop::Callback(uint ___entityId)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::AnimStop> msg = GraphicsFeature::AnimStop::Create();
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
CreateGraphicsEffectUpVec::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Point", Scripting::Arg::Float4);
    this->args.AddArg("UpVec", Scripting::Arg::Float4);
    this->args.AddArg("Resource", Scripting::Arg::String);
    this->args.AddArg("Duration", Scripting::Arg::Float);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CreateGraphicsEffectUpVec::GetHelp() const
{
    return "Create a graphics effect on a point with an incident up vector.<br /> Resource is the full resource name to attach, i.e 'mdl:system/placeholder.n3'.<br />";
}
//------------------------------------------------------------------------------
/**
*/
bool
CreateGraphicsEffectUpVec::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    const Math::float4& Point = this->args.GetArgValue(1).GetFloat4();
    const Math::float4& UpVec = this->args.GetArgValue(2).GetFloat4();
    const Util::String& Resource = this->args.GetArgValue(3).GetString();
    float Duration = this->args.GetArgValue(4).GetFloat();
    this->Callback(___entityId, Point, UpVec, Resource, Duration);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CreateGraphicsEffectUpVec::Callback(uint ___entityId, const Math::float4& Point, const Math::float4& UpVec, const Util::String& Resource, float Duration)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CreateGraphicsEffectUpVec> msg = GraphicsFeature::CreateGraphicsEffectUpVec::Create();
	msg->SetPoint(Point);
	msg->SetUpVec(UpVec);
	msg->SetResource(Resource);
	msg->SetDuration(Duration);
	entity->SendSync(msg.cast<Messaging::Message>());

}

//------------------------------------------------------------------------------
/**
*/
void
CreateAttachmentEffect::OnRegister()
{
    Scripting::Command::OnRegister();
    this->args.AddArg("___entityId", Scripting::Arg::UInt);
    this->args.AddArg("Resource", Scripting::Arg::String);
    this->args.AddArg("Joint", Scripting::Arg::String);
    this->args.AddArg("Duration", Scripting::Arg::UInt);
    this->args.AddArg("Delay", Scripting::Arg::UInt);
    this->args.AddArg("KeepLocal", Scripting::Arg::Bool);
    this->args.AddArg("RotationMode", Scripting::Arg::Int);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
CreateAttachmentEffect::GetHelp() const
{
    return "Create an attachment effect which uses this entity and attaches an effect to a joint with a name.<br /> Resource is the full resource name to attach, i.e 'mdl:system/placeholder.n3'.<br /> Joint is a string matching the name of a joint.<br /> Duration is the time during which the effect should be active.<br /> Delay is the time it takes until the effect itself is applied.<br /> RotationMode can be one of the three:<br /> 1 = Local.<br /> 2 = World.<br /> 3 = Entity.<br />";
}
//------------------------------------------------------------------------------
/**
*/
bool
CreateAttachmentEffect::OnExecute()
{
    uint ___entityId = this->args.GetArgValue(0).GetUInt();
    const Util::String& Resource = this->args.GetArgValue(1).GetString();
    const Util::String& Joint = this->args.GetArgValue(2).GetString();
    uint Duration = this->args.GetArgValue(3).GetUInt();
    uint Delay = this->args.GetArgValue(4).GetUInt();
    bool KeepLocal = this->args.GetArgValue(5).GetBool();
    int RotationMode = this->args.GetArgValue(6).GetInt();
    this->Callback(___entityId, Resource, Joint, Duration, Delay, KeepLocal, RotationMode);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void CreateAttachmentEffect::Callback(uint ___entityId, const Util::String& Resource, const Util::String& Joint, uint Duration, uint Delay, bool KeepLocal, int RotationMode)
{
	Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(___entityId);
	n_assert2(entity.isvalid(),"Provided invalid entity id or entity not found!");
	Ptr<GraphicsFeature::CreateAttachmentEffect> msg = GraphicsFeature::CreateAttachmentEffect::Create();
	msg->SetResource(Resource);
	msg->SetJoint(Joint);
	msg->SetDuration(Duration);
	msg->SetDelay(Delay);
	msg->SetKeepLocal(KeepLocal);
	msg->SetRotationMode(RotationMode);
	entity->SendSync(msg.cast<Messaging::Message>());

}

} // namespace Commands
//------------------------------------------------------------------------------
