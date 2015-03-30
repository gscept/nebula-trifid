#pragma once
//------------------------------------------------------------------------------
/**
    This file was generated with Nebula Trifid's idlc compiler tool.
    DO NOT EDIT
*/
#include "scripting/command.h"
#include "messaging/message.h"
#include "graphics/modelentity.h"
#include "graphics/abstractlightentity.h"
#include "game/entity.h"
#include "util/variant.h"
#include "basegamefeature/managers/entitymanager.h"
#include "graphics/modelentity.h"
#include "graphics/abstractlightentity.h"
#include "game/entity.h"
#include "util/variant.h"

//------------------------------------------------------------------------------
namespace Commands
{
class GraphicsFeatureProtocol
{
public:
    /// register commands
    static void Register();
};
//------------------------------------------------------------------------------
class CameraDistance : public Scripting::Command
{
    __DeclareClass(CameraDistance);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, float RelativeDistanceChange);
};

//------------------------------------------------------------------------------
class CameraFocus : public Scripting::Command
{
    __DeclareClass(CameraFocus);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, bool ObtainFocus);
};

//------------------------------------------------------------------------------
class CameraOrbit : public Scripting::Command
{
    __DeclareClass(CameraOrbit);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, float HorizontalRotation, float VerticalRotation);
};

//------------------------------------------------------------------------------
class CameraReset : public Scripting::Command
{
    __DeclareClass(CameraReset);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId);
};

//------------------------------------------------------------------------------
class InputFocus : public Scripting::Command
{
    __DeclareClass(InputFocus);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, bool ObtainFocus);
};

//------------------------------------------------------------------------------
class SetGraphicsVisible : public Scripting::Command
{
    __DeclareClass(SetGraphicsVisible);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, bool Visible);
};

//------------------------------------------------------------------------------
class SetOverwriteColor : public Scripting::Command
{
    __DeclareClass(SetOverwriteColor);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, const Math::float4& Color, const Util::String& NodeName);
};

//------------------------------------------------------------------------------
class PlayAnimClip : public Scripting::Command
{
    __DeclareClass(PlayAnimClip);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, const Util::String& Clip, float LoopCount, bool Queue);
};

//------------------------------------------------------------------------------
class SetSkinVisible : public Scripting::Command
{
    __DeclareClass(SetSkinVisible);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, const Util::String& Skin, bool Visible);
};

//------------------------------------------------------------------------------
class AnimStop : public Scripting::Command
{
    __DeclareClass(AnimStop);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId);
};

//------------------------------------------------------------------------------
class CreateGraphicsEffectUpVec : public Scripting::Command
{
    __DeclareClass(CreateGraphicsEffectUpVec);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, const Math::float4& Point, const Math::float4& UpVec, const Util::String& Resource, float Duration);
};

//------------------------------------------------------------------------------
class CreateAttachmentEffect : public Scripting::Command
{
    __DeclareClass(CreateAttachmentEffect);
public:
    virtual void OnRegister();
    virtual bool OnExecute();
    virtual Util::String GetHelp() const;
private:
    void Callback(uint ___entityId, const Util::String& Resource, const Util::String& Joint, uint Duration, uint Delay, bool KeepLocal, int RotationMode);
};

} // namespace Commands
//------------------------------------------------------------------------------
namespace GraphicsFeature
{
//------------------------------------------------------------------------------
class CameraDistance : public Messaging::Message
{
    __DeclareClass(CameraDistance);
    __DeclareMsgId;
public:
    CameraDistance() :
        relativedistancechange(0.0f)
    { };
public:
    void SetRelativeDistanceChange(float val)
    {
        n_assert(!this->handled);
        this->relativedistancechange = val;
    };
    float GetRelativeDistanceChange() const
    {
        return this->relativedistancechange;
    };
private:
    float relativedistancechange;
};
//------------------------------------------------------------------------------
class CameraFocus : public Messaging::Message
{
    __DeclareClass(CameraFocus);
    __DeclareMsgId;
public:
    CameraFocus() 
    { };
public:
    void SetObtainFocus(bool val)
    {
        n_assert(!this->handled);
        this->obtainfocus = val;
    };
    bool GetObtainFocus() const
    {
        return this->obtainfocus;
    };
private:
    bool obtainfocus;
};
//------------------------------------------------------------------------------
class CameraOrbit : public Messaging::Message
{
    __DeclareClass(CameraOrbit);
    __DeclareMsgId;
public:
    CameraOrbit() :
        horizontalrotation(0.0f),
        verticalrotation(0.0f)
    { };
public:
    void SetHorizontalRotation(float val)
    {
        n_assert(!this->handled);
        this->horizontalrotation = val;
    };
    float GetHorizontalRotation() const
    {
        return this->horizontalrotation;
    };
private:
    float horizontalrotation;
public:
    void SetVerticalRotation(float val)
    {
        n_assert(!this->handled);
        this->verticalrotation = val;
    };
    float GetVerticalRotation() const
    {
        return this->verticalrotation;
    };
private:
    float verticalrotation;
};
//------------------------------------------------------------------------------
class CameraReset : public Messaging::Message
{
    __DeclareClass(CameraReset);
    __DeclareMsgId;
public:
    CameraReset() 
    { };
};
//------------------------------------------------------------------------------
class GetGraphicsEntities : public Messaging::Message
{
    __DeclareClass(GetGraphicsEntities);
    __DeclareMsgId;
public:
    GetGraphicsEntities() 
    { };
public:
    void SetEntities(const Util::Array<Ptr<Graphics::ModelEntity> >& val)
    {
        n_assert(!this->handled);
        this->entities = val;
    };
    const Util::Array<Ptr<Graphics::ModelEntity> >& GetEntities() const
    {
        return this->entities;
    };
private:
    Util::Array<Ptr<Graphics::ModelEntity> > entities;
public:
    void SetEntity(const Ptr<Graphics::ModelEntity>& val)
    {
        n_assert(!this->handled);
        this->entity = val;
    };
    const Ptr<Graphics::ModelEntity>& GetEntity() const
    {
        return this->entity;
    };
private:
    Ptr<Graphics::ModelEntity> entity;
};
//------------------------------------------------------------------------------
class GetLightEntity : public Messaging::Message
{
    __DeclareClass(GetLightEntity);
    __DeclareMsgId;
public:
    GetLightEntity() 
    { };
public:
    void SetEntity(const Ptr<Graphics::AbstractLightEntity>& val)
    {
        n_assert(!this->handled);
        this->entity = val;
    };
    const Ptr<Graphics::AbstractLightEntity>& GetEntity() const
    {
        return this->entity;
    };
private:
    Ptr<Graphics::AbstractLightEntity> entity;
};
//------------------------------------------------------------------------------
class InputFocus : public Messaging::Message
{
    __DeclareClass(InputFocus);
    __DeclareMsgId;
public:
    InputFocus() :
        obtainfocus(true)
    { };
public:
    void SetObtainFocus(bool val)
    {
        n_assert(!this->handled);
        this->obtainfocus = val;
    };
    bool GetObtainFocus() const
    {
        return this->obtainfocus;
    };
private:
    bool obtainfocus;
};
//------------------------------------------------------------------------------
class SetGraphicsVisible : public Messaging::Message
{
    __DeclareClass(SetGraphicsVisible);
    __DeclareMsgId;
public:
    SetGraphicsVisible() 
    { };
public:
    void SetVisible(bool val)
    {
        n_assert(!this->handled);
        this->visible = val;
    };
    bool GetVisible() const
    {
        return this->visible;
    };
private:
    bool visible;
};
//------------------------------------------------------------------------------
class SetOverwriteColor : public Messaging::Message
{
    __DeclareClass(SetOverwriteColor);
    __DeclareMsgId;
public:
    SetOverwriteColor() 
    { };
public:
    void SetColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->color = val;
    };
    const Math::float4& GetColor() const
    {
        return this->color;
    };
private:
    Math::float4 color;
public:
    void SetNodeName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->nodename = val;
    };
    const Util::String& GetNodeName() const
    {
        return this->nodename;
    };
private:
    Util::String nodename;
};
//------------------------------------------------------------------------------
class SetShaderVariable : public Messaging::Message
{
    __DeclareClass(SetShaderVariable);
    __DeclareMsgId;
public:
    SetShaderVariable() 
    { };
public:
    void SetShaderVarName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->shadervarname = val;
    };
    const Util::String& GetShaderVarName() const
    {
        return this->shadervarname;
    };
private:
    Util::String shadervarname;
public:
    void SetValue(const Util::Variant& val)
    {
        n_assert(!this->handled);
        this->value = val;
    };
    const Util::Variant& GetValue() const
    {
        return this->value;
    };
private:
    Util::Variant value;
public:
    void SetNodeName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->nodename = val;
    };
    const Util::String& GetNodeName() const
    {
        return this->nodename;
    };
private:
    Util::String nodename;
};
//------------------------------------------------------------------------------
class SetMaterialVariable : public Messaging::Message
{
    __DeclareClass(SetMaterialVariable);
    __DeclareMsgId;
public:
    SetMaterialVariable() 
    { };
public:
    void SetMaterialVarName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->materialvarname = val;
    };
    const Util::String& GetMaterialVarName() const
    {
        return this->materialvarname;
    };
private:
    Util::String materialvarname;
public:
    void SetValue(const Util::Variant& val)
    {
        n_assert(!this->handled);
        this->value = val;
    };
    const Util::Variant& GetValue() const
    {
        return this->value;
    };
private:
    Util::Variant value;
public:
    void SetNodeName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->nodename = val;
    };
    const Util::String& GetNodeName() const
    {
        return this->nodename;
    };
private:
    Util::String nodename;
};
//------------------------------------------------------------------------------
class PlayAnimClip : public Messaging::Message
{
    __DeclareClass(PlayAnimClip);
    __DeclareMsgId;
public:
    PlayAnimClip() 
    { };
public:
    void SetClip(const Util::String& val)
    {
        n_assert(!this->handled);
        this->clip = val;
    };
    const Util::String& GetClip() const
    {
        return this->clip;
    };
private:
    Util::String clip;
public:
    void SetLoopCount(float val)
    {
        n_assert(!this->handled);
        this->loopcount = val;
    };
    float GetLoopCount() const
    {
        return this->loopcount;
    };
private:
    float loopcount;
public:
    void SetQueue(bool val)
    {
        n_assert(!this->handled);
        this->queue = val;
    };
    bool GetQueue() const
    {
        return this->queue;
    };
private:
    bool queue;
};
//------------------------------------------------------------------------------
class SetSkinVisible : public Messaging::Message
{
    __DeclareClass(SetSkinVisible);
    __DeclareMsgId;
public:
    SetSkinVisible() 
    { };
public:
    void SetSkin(const Util::String& val)
    {
        n_assert(!this->handled);
        this->skin = val;
    };
    const Util::String& GetSkin() const
    {
        return this->skin;
    };
private:
    Util::String skin;
public:
    void SetVisible(bool val)
    {
        n_assert(!this->handled);
        this->visible = val;
    };
    bool GetVisible() const
    {
        return this->visible;
    };
private:
    bool visible;
};
//------------------------------------------------------------------------------
class AnimPlay : public Messaging::Message
{
    __DeclareClass(AnimPlay);
    __DeclareMsgId;
public:
    AnimPlay() 
    { };
};
//------------------------------------------------------------------------------
class AnimRewind : public Messaging::Message
{
    __DeclareClass(AnimRewind);
    __DeclareMsgId;
public:
    AnimRewind() 
    { };
};
//------------------------------------------------------------------------------
class AnimStop : public Messaging::Message
{
    __DeclareClass(AnimStop);
    __DeclareMsgId;
public:
    AnimStop() 
    { };
};
//------------------------------------------------------------------------------
class AnimUpdate : public Messaging::Message
{
    __DeclareClass(AnimUpdate);
    __DeclareMsgId;
public:
    AnimUpdate() 
    { };
public:
    void SetAnimPath(const Util::String& val)
    {
        n_assert(!this->handled);
        this->animpath = val;
    };
    const Util::String& GetAnimPath() const
    {
        return this->animpath;
    };
private:
    Util::String animpath;
};
//------------------------------------------------------------------------------
class CutsceneControl : public Messaging::Message
{
    __DeclareClass(CutsceneControl);
    __DeclareMsgId;
public:
    CutsceneControl() 
    { };
public:
    void SetCommand(int val)
    {
        n_assert(!this->handled);
        this->command = val;
    };
    int GetCommand() const
    {
        return this->command;
    };
private:
    int command;
};
//------------------------------------------------------------------------------
class CreateGraphicsEffectUpVec : public Messaging::Message
{
    __DeclareClass(CreateGraphicsEffectUpVec);
    __DeclareMsgId;
public:
    CreateGraphicsEffectUpVec() 
    { };
public:
    void SetPoint(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->point = val;
    };
    const Math::float4& GetPoint() const
    {
        return this->point;
    };
private:
    Math::float4 point;
public:
    void SetUpVec(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->upvec = val;
    };
    const Math::float4& GetUpVec() const
    {
        return this->upvec;
    };
private:
    Math::float4 upvec;
public:
    void SetResource(const Util::String& val)
    {
        n_assert(!this->handled);
        this->resource = val;
    };
    const Util::String& GetResource() const
    {
        return this->resource;
    };
private:
    Util::String resource;
public:
    void SetDuration(float val)
    {
        n_assert(!this->handled);
        this->duration = val;
    };
    float GetDuration() const
    {
        return this->duration;
    };
private:
    float duration;
};
//------------------------------------------------------------------------------
class CreateAttachmentEffect : public Messaging::Message
{
    __DeclareClass(CreateAttachmentEffect);
    __DeclareMsgId;
public:
    CreateAttachmentEffect() 
    { };
public:
    void SetResource(const Util::String& val)
    {
        n_assert(!this->handled);
        this->resource = val;
    };
    const Util::String& GetResource() const
    {
        return this->resource;
    };
private:
    Util::String resource;
public:
    void SetJoint(const Util::String& val)
    {
        n_assert(!this->handled);
        this->joint = val;
    };
    const Util::String& GetJoint() const
    {
        return this->joint;
    };
private:
    Util::String joint;
public:
    void SetDuration(uint val)
    {
        n_assert(!this->handled);
        this->duration = val;
    };
    uint GetDuration() const
    {
        return this->duration;
    };
private:
    uint duration;
public:
    void SetDelay(uint val)
    {
        n_assert(!this->handled);
        this->delay = val;
    };
    uint GetDelay() const
    {
        return this->delay;
    };
private:
    uint delay;
public:
    void SetKeepLocal(bool val)
    {
        n_assert(!this->handled);
        this->keeplocal = val;
    };
    bool GetKeepLocal() const
    {
        return this->keeplocal;
    };
private:
    bool keeplocal;
public:
    void SetRotationMode(int val)
    {
        n_assert(!this->handled);
        this->rotationmode = val;
    };
    int GetRotationMode() const
    {
        return this->rotationmode;
    };
private:
    int rotationmode;
};
//------------------------------------------------------------------------------
class AreGraphicsEntityResourcesLoaded : public Messaging::Message
{
    __DeclareClass(AreGraphicsEntityResourcesLoaded);
    __DeclareMsgId;
public:
    AreGraphicsEntityResourcesLoaded() 
    { };
public:
    void SetResourcesLoaded(bool val)
    {
        n_assert(!this->handled);
        this->resourcesloaded = val;
    };
    bool GetResourcesLoaded() const
    {
        return this->resourcesloaded;
    };
private:
    bool resourcesloaded;
};
//------------------------------------------------------------------------------
class GraphicsReady : public Messaging::Message
{
    __DeclareClass(GraphicsReady);
    __DeclareMsgId;
public:
    GraphicsReady() :
        entityid(0)
    { };
public:
    void SetEntityId(const Game::Entity::EntityId& val)
    {
        n_assert(!this->handled);
        this->entityid = val;
    };
    const Game::Entity::EntityId& GetEntityId() const
    {
        return this->entityid;
    };
private:
    Game::Entity::EntityId entityid;
};
} // namespace GraphicsFeature
//------------------------------------------------------------------------------
