#pragma once
//------------------------------------------------------------------------------
/**
    This file was generated with Nebula Trifid's idlc compiler tool.
    DO NOT EDIT
*/
#include "messaging/message.h"
#include "util/stringatom.h"
#include "util/variant.h"
#include "util/blob.h"
#include "core/rtti.h"
#include "coregraphics/displaymode.h"
#include "coregraphics/adapter.h"
#include "events/event.h"
#include "coregraphics/antialiasquality.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/adapterinfo.h"
#include "coregraphics/displayeventhandler.h"
#include "coregraphics/rendereventhandler.h"
#include "graphics/attachmentserver.h"
#include "graphics/graphicsentity.h"
#include "visibility/visibilitysystems/visibilitysystembase.h"
#include "resources/resourceid.h"
#include "math/matrix44.h"
#include "math/float4.h"
#include "math/rectangle.h"
#include "math/quaternion.h"
#include "math/clipstatus.h"
#include "threading/objectref.h"
#include "timing/time.h"
#include "animation/animeventhandlerbase.h"
#include "coregraphics/mousepointer.h"
#include "resources/resourcemapper.h"
#include "graphics/camerasettings.h"
#include "animation/animjobenqueuemode.h"
#include "materials/materialvariable.h"
#include "materials/material.h"
#include "util/dictionary.h"
#include "models/modelnode.h"
#include "resources/resourceid.h"
#include "particles/emitterattrs.h"

//------------------------------------------------------------------------------
namespace Graphics
{
//------------------------------------------------------------------------------
class RegisterRTPlugin : public Messaging::Message
{
    __DeclareClass(RegisterRTPlugin);
    __DeclareMsgId;
public:
    RegisterRTPlugin() 
    { };
public:
    void SetType(const Core::Rtti* val)
    {
        n_assert(!this->handled);
        this->type = val;
    };
    const Core::Rtti* GetType() const
    {
        return this->type;
    };
private:
    const Core::Rtti* type;
};
//------------------------------------------------------------------------------
class UnregisterRTPlugin : public Messaging::Message
{
    __DeclareClass(UnregisterRTPlugin);
    __DeclareMsgId;
public:
    UnregisterRTPlugin() 
    { };
public:
    void SetType(const Core::Rtti* val)
    {
        n_assert(!this->handled);
        this->type = val;
    };
    const Core::Rtti* GetType() const
    {
        return this->type;
    };
private:
    const Core::Rtti* type;
};
//------------------------------------------------------------------------------
class SetupGraphics : public Messaging::Message
{
    __DeclareClass(SetupGraphics);
    __DeclareMsgId;
public:
    SetupGraphics() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetDisplayMode(const CoreGraphics::DisplayMode& val)
    {
        n_assert(!this->handled);
        this->displaymode = val;
    };
    const CoreGraphics::DisplayMode& GetDisplayMode() const
    {
        return this->displaymode;
    };
private:
    CoreGraphics::DisplayMode displaymode;
public:
    void SetAntiAliasQuality(const CoreGraphics::AntiAliasQuality::Code& val)
    {
        n_assert(!this->handled);
        this->antialiasquality = val;
    };
    const CoreGraphics::AntiAliasQuality::Code& GetAntiAliasQuality() const
    {
        return this->antialiasquality;
    };
private:
    CoreGraphics::AntiAliasQuality::Code antialiasquality;
public:
    void SetFullscreen(bool val)
    {
        n_assert(!this->handled);
        this->fullscreen = val;
    };
    bool GetFullscreen() const
    {
        return this->fullscreen;
    };
private:
    bool fullscreen;
public:
    void SetDisplayModeSwitchEnabled(bool val)
    {
        n_assert(!this->handled);
        this->displaymodeswitchenabled = val;
    };
    bool GetDisplayModeSwitchEnabled() const
    {
        return this->displaymodeswitchenabled;
    };
private:
    bool displaymodeswitchenabled;
public:
    void SetTripleBufferingEnabled(bool val)
    {
        n_assert(!this->handled);
        this->triplebufferingenabled = val;
    };
    bool GetTripleBufferingEnabled() const
    {
        return this->triplebufferingenabled;
    };
private:
    bool triplebufferingenabled;
public:
    void SetAlwaysOnTop(bool val)
    {
        n_assert(!this->handled);
        this->alwaysontop = val;
    };
    bool GetAlwaysOnTop() const
    {
        return this->alwaysontop;
    };
private:
    bool alwaysontop;
public:
    void SetVerticalSyncEnabled(bool val)
    {
        n_assert(!this->handled);
        this->verticalsyncenabled = val;
    };
    bool GetVerticalSyncEnabled() const
    {
        return this->verticalsyncenabled;
    };
private:
    bool verticalsyncenabled;
public:
    void SetIconName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->iconname = val;
    };
    const Util::String& GetIconName() const
    {
        return this->iconname;
    };
private:
    Util::String iconname;
public:
    void SetWindowTitle(const Util::String& val)
    {
        n_assert(!this->handled);
        this->windowtitle = val;
    };
    const Util::String& GetWindowTitle() const
    {
        return this->windowtitle;
    };
private:
    Util::String windowtitle;
public:
    void SetWindowData(const Util::Blob& val)
    {
        n_assert(!this->handled);
        this->windowdata = val;
    };
    const Util::Blob& GetWindowData() const
    {
        return this->windowdata;
    };
private:
    Util::Blob windowdata;
public:
    void SetEmbedded(bool val)
    {
        n_assert(!this->handled);
        this->embedded = val;
    };
    bool GetEmbedded() const
    {
        return this->embedded;
    };
private:
    bool embedded;
public:
    void SetDecorated(bool val)
    {
        n_assert(!this->handled);
        this->decorated = val;
    };
    bool GetDecorated() const
    {
        return this->decorated;
    };
private:
    bool decorated;
public:
    void SetResizable(bool val)
    {
        n_assert(!this->handled);
        this->resizable = val;
    };
    bool GetResizable() const
    {
        return this->resizable;
    };
private:
    bool resizable;
public:
    void SetMultithreadedRendering(bool val)
    {
        n_assert(!this->handled);
        this->multithreadedrendering = val;
    };
    bool GetMultithreadedRendering() const
    {
        return this->multithreadedrendering;
    };
private:
    bool multithreadedrendering;
public:
    void SetResourceMappers(const Util::Array<Ptr<Resources::ResourceMapper> >& val)
    {
        n_assert(!this->handled);
        this->resourcemappers = val;
    };
    const Util::Array<Ptr<Resources::ResourceMapper> >& GetResourceMappers() const
    {
        return this->resourcemappers;
    };
private:
    Util::Array<Ptr<Resources::ResourceMapper> > resourcemappers;
public:
    void SetSuccess(bool val)
    {
        n_assert(!this->handled);
        this->success = val;
    };
    bool GetSuccess() const
    {
        n_assert(this->handled);
        return this->success;
    };
private:
    bool success;
public:
    void SetActualDisplayMode(const CoreGraphics::DisplayMode& val)
    {
        n_assert(!this->handled);
        this->actualdisplaymode = val;
    };
    const CoreGraphics::DisplayMode& GetActualDisplayMode() const
    {
        n_assert(this->handled);
        return this->actualdisplaymode;
    };
private:
    CoreGraphics::DisplayMode actualdisplaymode;
public:
    void SetActualFullscreen(bool val)
    {
        n_assert(!this->handled);
        this->actualfullscreen = val;
    };
    bool GetActualFullscreen() const
    {
        n_assert(this->handled);
        return this->actualfullscreen;
    };
private:
    bool actualfullscreen;
public:
    void SetActualAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->actualadapter = val;
    };
    const CoreGraphics::Adapter::Code& GetActualAdapter() const
    {
        n_assert(this->handled);
        return this->actualadapter;
    };
private:
    CoreGraphics::Adapter::Code actualadapter;
};
//------------------------------------------------------------------------------
class UpdateDisplay : public Messaging::Message
{
    __DeclareClass(UpdateDisplay);
    __DeclareMsgId;
public:
    UpdateDisplay() :
        antialiasquality(CoreGraphics::AntiAliasQuality::None),
        windowdata(0)
    { };
public:
    void SetDisplayMode(const CoreGraphics::DisplayMode& val)
    {
        n_assert(!this->handled);
        this->displaymode = val;
    };
    const CoreGraphics::DisplayMode& GetDisplayMode() const
    {
        return this->displaymode;
    };
private:
    CoreGraphics::DisplayMode displaymode;
public:
    void SetAntiAliasQuality(const CoreGraphics::AntiAliasQuality::Code& val)
    {
        n_assert(!this->handled);
        this->antialiasquality = val;
    };
    const CoreGraphics::AntiAliasQuality::Code& GetAntiAliasQuality() const
    {
        return this->antialiasquality;
    };
private:
    CoreGraphics::AntiAliasQuality::Code antialiasquality;
public:
    void SetFullscreen(bool val)
    {
        n_assert(!this->handled);
        this->fullscreen = val;
    };
    bool GetFullscreen() const
    {
        return this->fullscreen;
    };
private:
    bool fullscreen;
public:
    void SetWindowData(const Util::Blob& val)
    {
        n_assert(!this->handled);
        this->windowdata = val;
    };
    const Util::Blob& GetWindowData() const
    {
        return this->windowdata;
    };
private:
    Util::Blob windowdata;
public:
    void SetTripleBufferingEnabled(bool val)
    {
        n_assert(!this->handled);
        this->triplebufferingenabled = val;
    };
    bool GetTripleBufferingEnabled() const
    {
        return this->triplebufferingenabled;
    };
private:
    bool triplebufferingenabled;
public:
    void SetSuccess(bool val)
    {
        n_assert(!this->handled);
        this->success = val;
    };
    bool GetSuccess() const
    {
        n_assert(this->handled);
        return this->success;
    };
private:
    bool success;
};
//------------------------------------------------------------------------------
class PostWindowEvent : public Messaging::Message
{
    __DeclareClass(PostWindowEvent);
    __DeclareMsgId;
public:
    PostWindowEvent() 
    { };
public:
    void SetWindowEvent(Event val)
    {
        n_assert(!this->handled);
        this->windowevent = val;
    };
    Event GetWindowEvent() const
    {
        return this->windowevent;
    };
private:
    Event windowevent;
};
//------------------------------------------------------------------------------
class EnableResourcesAsync : public Messaging::Message
{
    __DeclareClass(EnableResourcesAsync);
    __DeclareMsgId;
public:
    EnableResourcesAsync() 
    { };
public:
    void SetEnabled(bool val)
    {
        n_assert(!this->handled);
        this->enabled = val;
    };
    bool GetEnabled() const
    {
        return this->enabled;
    };
private:
    bool enabled;
};
//------------------------------------------------------------------------------
class ReloadResource : public Messaging::Message
{
    __DeclareClass(ReloadResource);
    __DeclareMsgId;
public:
    ReloadResource() 
    { };
public:
    void SetResourceName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->resourcename = val;
    };
    const Util::String& GetResourceName() const
    {
        return this->resourcename;
    };
private:
    Util::String resourcename;
public:
    void Encode(const Ptr<IO::BinaryWriter>& writer)
    {
        writer->WriteString(this->GetResourceName());
        Messaging::Message::Encode(writer);
    };
public:
    void Decode(const Ptr<IO::BinaryReader>& reader)
    {
        this->SetResourceName(reader->ReadString());
        Messaging::Message::Decode(reader);
    };
};
//------------------------------------------------------------------------------
class ReloadResourceIfExists : public Messaging::Message
{
    __DeclareClass(ReloadResourceIfExists);
    __DeclareMsgId;
public:
    ReloadResourceIfExists() 
    { };
public:
    void SetResourceName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->resourcename = val;
    };
    const Util::String& GetResourceName() const
    {
        return this->resourcename;
    };
private:
    Util::String resourcename;
public:
    void Encode(const Ptr<IO::BinaryWriter>& writer)
    {
        writer->WriteString(this->GetResourceName());
        Messaging::Message::Encode(writer);
    };
public:
    void Decode(const Ptr<IO::BinaryReader>& reader)
    {
        this->SetResourceName(reader->ReadString());
        Messaging::Message::Decode(reader);
    };
};
//------------------------------------------------------------------------------
class EnablePicking : public Messaging::Message
{
    __DeclareClass(EnablePicking);
    __DeclareMsgId;
public:
    EnablePicking() 
    { };
public:
    void SetEnabled(bool val)
    {
        n_assert(!this->handled);
        this->enabled = val;
    };
    bool GetEnabled() const
    {
        return this->enabled;
    };
private:
    bool enabled;
};
//------------------------------------------------------------------------------
class SetEnvironmentAttached : public Messaging::Message
{
    __DeclareClass(SetEnvironmentAttached);
    __DeclareMsgId;
public:
    SetEnvironmentAttached() 
    { };
public:
    void SetAttached(bool val)
    {
        n_assert(!this->handled);
        this->attached = val;
    };
    bool GetAttached() const
    {
        return this->attached;
    };
private:
    bool attached;
};
//------------------------------------------------------------------------------
class ItemAtPosition : public Messaging::Message
{
    __DeclareClass(ItemAtPosition);
    __DeclareMsgId;
public:
    ItemAtPosition() 
    { };
public:
    void SetPosition(const Math::float2& val)
    {
        n_assert(!this->handled);
        this->position = val;
    };
    const Math::float2& GetPosition() const
    {
        return this->position;
    };
private:
    Math::float2 position;
public:
    void SetItem(IndexT val)
    {
        n_assert(!this->handled);
        this->item = val;
    };
    IndexT GetItem() const
    {
        n_assert(this->handled);
        return this->item;
    };
private:
    IndexT item;
};
//------------------------------------------------------------------------------
class ItemsAtPosition : public Messaging::Message
{
    __DeclareClass(ItemsAtPosition);
    __DeclareMsgId;
public:
    ItemsAtPosition() 
    { };
public:
    void SetRectangle(const Math::rectangle<float>& val)
    {
        n_assert(!this->handled);
        this->rectangle = val;
    };
    const Math::rectangle<float>& GetRectangle() const
    {
        return this->rectangle;
    };
private:
    Math::rectangle<float> rectangle;
public:
    void SetItems(const Util::Array<IndexT>& val)
    {
        n_assert(!this->handled);
        this->items = val;
    };
    const Util::Array<IndexT>& GetItems() const
    {
        n_assert(this->handled);
        return this->items;
    };
private:
    Util::Array<IndexT> items;
};
//------------------------------------------------------------------------------
class DepthAtPosition : public Messaging::Message
{
    __DeclareClass(DepthAtPosition);
    __DeclareMsgId;
public:
    DepthAtPosition() 
    { };
public:
    void SetPosition(const Math::float2& val)
    {
        n_assert(!this->handled);
        this->position = val;
    };
    const Math::float2& GetPosition() const
    {
        return this->position;
    };
private:
    Math::float2 position;
public:
    void SetDepth(float val)
    {
        n_assert(!this->handled);
        this->depth = val;
    };
    float GetDepth() const
    {
        n_assert(this->handled);
        return this->depth;
    };
private:
    float depth;
};
//------------------------------------------------------------------------------
class NormalAtPosition : public Messaging::Message
{
    __DeclareClass(NormalAtPosition);
    __DeclareMsgId;
public:
    NormalAtPosition() 
    { };
public:
    void SetPosition(const Math::float2& val)
    {
        n_assert(!this->handled);
        this->position = val;
    };
    const Math::float2& GetPosition() const
    {
        return this->position;
    };
private:
    Math::float2 position;
public:
    void SetNormal(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->normal = val;
    };
    const Math::float4& GetNormal() const
    {
        n_assert(this->handled);
        return this->normal;
    };
private:
    Math::float4 normal;
};
//------------------------------------------------------------------------------
class EnableWireframe : public Messaging::Message
{
    __DeclareClass(EnableWireframe);
    __DeclareMsgId;
public:
    EnableWireframe() 
    { };
public:
    void SetEnabled(bool val)
    {
        n_assert(!this->handled);
        this->enabled = val;
    };
    bool GetEnabled() const
    {
        return this->enabled;
    };
private:
    bool enabled;
};
//------------------------------------------------------------------------------
class GetRenderMaterials : public Messaging::Message
{
    __DeclareClass(GetRenderMaterials);
    __DeclareMsgId;
public:
    GetRenderMaterials() 
    { };
public:
    void SetMaterials(const Util::Array<Ptr<Materials::Material> >& val)
    {
        n_assert(!this->handled);
        this->materials = val;
    };
    const Util::Array<Ptr<Materials::Material> >& GetMaterials() const
    {
        n_assert(this->handled);
        return this->materials;
    };
private:
    Util::Array<Ptr<Materials::Material> > materials;
};
//------------------------------------------------------------------------------
class SetModelNodeTexture : public Messaging::Message
{
    __DeclareClass(SetModelNodeTexture);
    __DeclareMsgId;
public:
    SetModelNodeTexture() 
    { };
public:
    void SetTextureName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->texturename = val;
    };
    const Util::String& GetTextureName() const
    {
        return this->texturename;
    };
private:
    Util::String texturename;
public:
    void SetTextureResource(const Util::String& val)
    {
        n_assert(!this->handled);
        this->textureresource = val;
    };
    const Util::String& GetTextureResource() const
    {
        return this->textureresource;
    };
private:
    Util::String textureresource;
public:
    void SetModelName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->modelname = val;
    };
    const Util::String& GetModelName() const
    {
        return this->modelname;
    };
private:
    Util::String modelname;
public:
    void SetModelNodeName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->modelnodename = val;
    };
    const Util::String& GetModelNodeName() const
    {
        return this->modelnodename;
    };
private:
    Util::String modelnodename;
};
//------------------------------------------------------------------------------
class SetModelNodeVariable : public Messaging::Message
{
    __DeclareClass(SetModelNodeVariable);
    __DeclareMsgId;
public:
    SetModelNodeVariable() 
    { };
public:
    void SetVariableName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->variablename = val;
    };
    const Util::String& GetVariableName() const
    {
        return this->variablename;
    };
private:
    Util::String variablename;
public:
    void SetVariableValue(const Util::Variant& val)
    {
        n_assert(!this->handled);
        this->variablevalue = val;
    };
    const Util::Variant& GetVariableValue() const
    {
        return this->variablevalue;
    };
private:
    Util::Variant variablevalue;
public:
    void SetModelName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->modelname = val;
    };
    const Util::String& GetModelName() const
    {
        return this->modelname;
    };
private:
    Util::String modelname;
public:
    void SetModelNodeName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->modelnodename = val;
    };
    const Util::String& GetModelNodeName() const
    {
        return this->modelnodename;
    };
private:
    Util::String modelnodename;
};
//------------------------------------------------------------------------------
class AdapterExists : public Messaging::Message
{
    __DeclareClass(AdapterExists);
    __DeclareMsgId;
public:
    AdapterExists() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetResult(bool val)
    {
        n_assert(!this->handled);
        this->result = val;
    };
    bool GetResult() const
    {
        n_assert(this->handled);
        return this->result;
    };
private:
    bool result;
};
//------------------------------------------------------------------------------
class GetAvailableDisplayModes : public Messaging::Message
{
    __DeclareClass(GetAvailableDisplayModes);
    __DeclareMsgId;
public:
    GetAvailableDisplayModes() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetPixelFormat(const CoreGraphics::PixelFormat::Code& val)
    {
        n_assert(!this->handled);
        this->pixelformat = val;
    };
    const CoreGraphics::PixelFormat::Code& GetPixelFormat() const
    {
        return this->pixelformat;
    };
private:
    CoreGraphics::PixelFormat::Code pixelformat;
public:
    void SetResult(const Util::Array<CoreGraphics::DisplayMode>& val)
    {
        n_assert(!this->handled);
        this->result = val;
    };
    const Util::Array<CoreGraphics::DisplayMode>& GetResult() const
    {
        n_assert(this->handled);
        return this->result;
    };
private:
    Util::Array<CoreGraphics::DisplayMode> result;
};
//------------------------------------------------------------------------------
class SupportsDisplayMode : public Messaging::Message
{
    __DeclareClass(SupportsDisplayMode);
    __DeclareMsgId;
public:
    SupportsDisplayMode() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetDisplayMode(const CoreGraphics::DisplayMode& val)
    {
        n_assert(!this->handled);
        this->displaymode = val;
    };
    const CoreGraphics::DisplayMode& GetDisplayMode() const
    {
        return this->displaymode;
    };
private:
    CoreGraphics::DisplayMode displaymode;
public:
    void SetResult(bool val)
    {
        n_assert(!this->handled);
        this->result = val;
    };
    bool GetResult() const
    {
        n_assert(this->handled);
        return this->result;
    };
private:
    bool result;
};
//------------------------------------------------------------------------------
class GetCurrentAdapterDisplayMode : public Messaging::Message
{
    __DeclareClass(GetCurrentAdapterDisplayMode);
    __DeclareMsgId;
public:
    GetCurrentAdapterDisplayMode() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetResult(const CoreGraphics::DisplayMode& val)
    {
        n_assert(!this->handled);
        this->result = val;
    };
    const CoreGraphics::DisplayMode& GetResult() const
    {
        n_assert(this->handled);
        return this->result;
    };
private:
    CoreGraphics::DisplayMode result;
};
//------------------------------------------------------------------------------
class GetAdapterInfo : public Messaging::Message
{
    __DeclareClass(GetAdapterInfo);
    __DeclareMsgId;
public:
    GetAdapterInfo() 
    { };
public:
    void SetAdapter(const CoreGraphics::Adapter::Code& val)
    {
        n_assert(!this->handled);
        this->adapter = val;
    };
    const CoreGraphics::Adapter::Code& GetAdapter() const
    {
        return this->adapter;
    };
private:
    CoreGraphics::Adapter::Code adapter;
public:
    void SetResult(const CoreGraphics::AdapterInfo& val)
    {
        n_assert(!this->handled);
        this->result = val;
    };
    const CoreGraphics::AdapterInfo& GetResult() const
    {
        n_assert(this->handled);
        return this->result;
    };
private:
    CoreGraphics::AdapterInfo result;
};
//------------------------------------------------------------------------------
class AttachDisplayEventHandler : public Messaging::Message
{
    __DeclareClass(AttachDisplayEventHandler);
    __DeclareMsgId;
public:
    AttachDisplayEventHandler() 
    { };
public:
    void SetHandler(const Ptr<CoreGraphics::DisplayEventHandler>& val)
    {
        n_assert(!this->handled);
        this->handler = val;
    };
    const Ptr<CoreGraphics::DisplayEventHandler>& GetHandler() const
    {
        return this->handler;
    };
private:
    Ptr<CoreGraphics::DisplayEventHandler> handler;
};
//------------------------------------------------------------------------------
class RemoveDisplayEventHandler : public Messaging::Message
{
    __DeclareClass(RemoveDisplayEventHandler);
    __DeclareMsgId;
public:
    RemoveDisplayEventHandler() 
    { };
public:
    void SetHandler(const Ptr<CoreGraphics::DisplayEventHandler>& val)
    {
        n_assert(!this->handled);
        this->handler = val;
    };
    const Ptr<CoreGraphics::DisplayEventHandler>& GetHandler() const
    {
        return this->handler;
    };
private:
    Ptr<CoreGraphics::DisplayEventHandler> handler;
};
//------------------------------------------------------------------------------
class AttachRenderEventHandler : public Messaging::Message
{
    __DeclareClass(AttachRenderEventHandler);
    __DeclareMsgId;
public:
    AttachRenderEventHandler() 
    { };
public:
    void SetHandler(const Ptr<CoreGraphics::RenderEventHandler>& val)
    {
        n_assert(!this->handled);
        this->handler = val;
    };
    const Ptr<CoreGraphics::RenderEventHandler>& GetHandler() const
    {
        return this->handler;
    };
private:
    Ptr<CoreGraphics::RenderEventHandler> handler;
};
//------------------------------------------------------------------------------
class RemoveRenderEventHandler : public Messaging::Message
{
    __DeclareClass(RemoveRenderEventHandler);
    __DeclareMsgId;
public:
    RemoveRenderEventHandler() 
    { };
public:
    void SetHandler(const Ptr<CoreGraphics::RenderEventHandler>& val)
    {
        n_assert(!this->handled);
        this->handler = val;
    };
    const Ptr<CoreGraphics::RenderEventHandler>& GetHandler() const
    {
        return this->handler;
    };
private:
    Ptr<CoreGraphics::RenderEventHandler> handler;
};
//------------------------------------------------------------------------------
class CreateGraphicsStage : public Messaging::Message
{
    __DeclareClass(CreateGraphicsStage);
    __DeclareMsgId;
public:
    CreateGraphicsStage() 
    { };
public:
    void SetName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->name = val;
    };
    const Util::StringAtom& GetName() const
    {
        return this->name;
    };
private:
    Util::StringAtom name;
public:
    void SetVisibleSystems(const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& val)
    {
        n_assert(!this->handled);
        this->visiblesystems = val;
    };
    const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& GetVisibleSystems() const
    {
        return this->visiblesystems;
    };
private:
    Util::Array<Ptr<Visibility::VisibilitySystemBase> > visiblesystems;
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class DiscardGraphicsStage : public Messaging::Message
{
    __DeclareClass(DiscardGraphicsStage);
    __DeclareMsgId;
public:
    DiscardGraphicsStage() 
    { };
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class CreateGraphicsView : public Messaging::Message
{
    __DeclareClass(CreateGraphicsView);
    __DeclareMsgId;
public:
    CreateGraphicsView() :
        viewclass(0),
        defaultview(false),
        useresolverect(false)
    { };
public:
    void SetViewClass(const Core::Rtti* val)
    {
        n_assert(!this->handled);
        this->viewclass = val;
    };
    const Core::Rtti* GetViewClass() const
    {
        return this->viewclass;
    };
private:
    const Core::Rtti* viewclass;
public:
    void SetName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->name = val;
    };
    const Util::StringAtom& GetName() const
    {
        return this->name;
    };
private:
    Util::StringAtom name;
public:
    void SetStageName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->stagename = val;
    };
    const Util::StringAtom& GetStageName() const
    {
        return this->stagename;
    };
private:
    Util::StringAtom stagename;
public:
    void SetFrameShaderName(const Resources::ResourceId& val)
    {
        n_assert(!this->handled);
        this->frameshadername = val;
    };
    const Resources::ResourceId& GetFrameShaderName() const
    {
        return this->frameshadername;
    };
private:
    Resources::ResourceId frameshadername;
public:
    void SetDefaultView(bool val)
    {
        n_assert(!this->handled);
        this->defaultview = val;
    };
    bool GetDefaultView() const
    {
        return this->defaultview;
    };
private:
    bool defaultview;
public:
    void SetUseResolveRect(bool val)
    {
        n_assert(!this->handled);
        this->useresolverect = val;
    };
    bool GetUseResolveRect() const
    {
        return this->useresolverect;
    };
private:
    bool useresolverect;
public:
    void SetResolveRect(const Math::rectangle<int>& val)
    {
        n_assert(!this->handled);
        this->resolverect = val;
    };
    const Math::rectangle<int>& GetResolveRect() const
    {
        return this->resolverect;
    };
private:
    Math::rectangle<int> resolverect;
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class DiscardGraphicsView : public Messaging::Message
{
    __DeclareClass(DiscardGraphicsView);
    __DeclareMsgId;
public:
    DiscardGraphicsView() 
    { };
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class SetDefaultGraphicsView : public Messaging::Message
{
    __DeclareClass(SetDefaultGraphicsView);
    __DeclareMsgId;
public:
    SetDefaultGraphicsView() 
    { };
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class CreateGraphicsEntity : public Messaging::Message
{
    __DeclareClass(CreateGraphicsEntity);
    __DeclareMsgId;
public:
    CreateGraphicsEntity() :
        visible(true)
    { };
public:
    void SetStageName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->stagename = val;
    };
    const Util::StringAtom& GetStageName() const
    {
        return this->stagename;
    };
private:
    Util::StringAtom stagename;
public:
    void SetTransform(const Math::matrix44& val)
    {
        n_assert(!this->handled);
        this->transform = val;
    };
    const Math::matrix44& GetTransform() const
    {
        return this->transform;
    };
private:
    Math::matrix44 transform;
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
public:
    void SetObjectRef(const Ptr<Threading::ObjectRef>& val)
    {
        n_assert(!this->handled);
        this->objectref = val;
    };
    const Ptr<Threading::ObjectRef>& GetObjectRef() const
    {
        return this->objectref;
    };
private:
    Ptr<Threading::ObjectRef> objectref;
};
//------------------------------------------------------------------------------
class GetModelNodes : public Messaging::Message
{
    __DeclareClass(GetModelNodes);
    __DeclareMsgId;
public:
    GetModelNodes() 
    { };
public:
    void SetModelName(const Util::String& val)
    {
        n_assert(!this->handled);
        this->modelname = val;
    };
    const Util::String& GetModelName() const
    {
        return this->modelname;
    };
private:
    Util::String modelname;
public:
    void SetNodeList(const Util::Array<Util::String>& val)
    {
        n_assert(!this->handled);
        this->nodelist = val;
    };
    const Util::Array<Util::String>& GetNodeList() const
    {
        n_assert(this->handled);
        return this->nodelist;
    };
private:
    Util::Array<Util::String> nodelist;
};
//------------------------------------------------------------------------------
class GetBoundingBox : public Messaging::Message
{
    __DeclareClass(GetBoundingBox);
    __DeclareMsgId;
public:
    GetBoundingBox() 
    { };
public:
    void SetBox(const Math::bbox& val)
    {
        n_assert(!this->handled);
        this->box = val;
    };
    const Math::bbox& GetBox() const
    {
        n_assert(this->handled);
        return this->box;
    };
private:
    Math::bbox box;
};
//------------------------------------------------------------------------------
class CreateModelEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreateModelEntity);
    __DeclareMsgId;
public:
    CreateModelEntity() :
        animdrivenmotionenabled(false),
        animeventsenabled(false),
        animeventsonlydominatingclip(true),
        charjointtrackingenabled(false),
        instanced(false),
        instancecode(-1)
    { };
public:
    void SetResourceId(const Resources::ResourceId& val)
    {
        n_assert(!this->handled);
        this->resourceid = val;
    };
    const Resources::ResourceId& GetResourceId() const
    {
        return this->resourceid;
    };
private:
    Resources::ResourceId resourceid;
public:
    void SetRootNodePath(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->rootnodepath = val;
    };
    const Util::StringAtom& GetRootNodePath() const
    {
        return this->rootnodepath;
    };
private:
    Util::StringAtom rootnodepath;
public:
    void SetRootNodeOffsetMatrix(const Math::matrix44& val)
    {
        n_assert(!this->handled);
        this->rootnodeoffsetmatrix = val;
    };
    const Math::matrix44& GetRootNodeOffsetMatrix() const
    {
        return this->rootnodeoffsetmatrix;
    };
private:
    Math::matrix44 rootnodeoffsetmatrix;
public:
    void SetAnimDrivenMotionEnabled(bool val)
    {
        n_assert(!this->handled);
        this->animdrivenmotionenabled = val;
    };
    bool GetAnimDrivenMotionEnabled() const
    {
        return this->animdrivenmotionenabled;
    };
private:
    bool animdrivenmotionenabled;
public:
    void SetAnimDrivenMotionJointName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->animdrivenmotionjointname = val;
    };
    const Util::StringAtom& GetAnimDrivenMotionJointName() const
    {
        return this->animdrivenmotionjointname;
    };
private:
    Util::StringAtom animdrivenmotionjointname;
public:
    void SetAnimEventsEnabled(bool val)
    {
        n_assert(!this->handled);
        this->animeventsenabled = val;
    };
    bool GetAnimEventsEnabled() const
    {
        return this->animeventsenabled;
    };
private:
    bool animeventsenabled;
public:
    void SetAnimEventsOnlyDominatingClip(bool val)
    {
        n_assert(!this->handled);
        this->animeventsonlydominatingclip = val;
    };
    bool GetAnimEventsOnlyDominatingClip() const
    {
        return this->animeventsonlydominatingclip;
    };
private:
    bool animeventsonlydominatingclip;
public:
    void SetCharJointTrackingEnabled(bool val)
    {
        n_assert(!this->handled);
        this->charjointtrackingenabled = val;
    };
    bool GetCharJointTrackingEnabled() const
    {
        return this->charjointtrackingenabled;
    };
private:
    bool charjointtrackingenabled;
public:
    void SetTrackedCharJointNames(const Util::Array<Util::StringAtom>& val)
    {
        n_assert(!this->handled);
        this->trackedcharjointnames = val;
    };
    const Util::Array<Util::StringAtom>& GetTrackedCharJointNames() const
    {
        return this->trackedcharjointnames;
    };
private:
    Util::Array<Util::StringAtom> trackedcharjointnames;
public:
    void SetPickingId(IndexT val)
    {
        n_assert(!this->handled);
        this->pickingid = val;
    };
    IndexT GetPickingId() const
    {
        return this->pickingid;
    };
private:
    IndexT pickingid;
public:
    void SetInstanced(bool val)
    {
        n_assert(!this->handled);
        this->instanced = val;
    };
    bool GetInstanced() const
    {
        return this->instanced;
    };
private:
    bool instanced;
public:
    void SetInstanceCode(IndexT val)
    {
        n_assert(!this->handled);
        this->instancecode = val;
    };
    IndexT GetInstanceCode() const
    {
        return this->instancecode;
    };
private:
    IndexT instancecode;
};
//------------------------------------------------------------------------------
class CreateCameraEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreateCameraEntity);
    __DeclareMsgId;
public:
    CreateCameraEntity() 
    { };
public:
    void SetCameraSettings(const Graphics::CameraSettings& val)
    {
        n_assert(!this->handled);
        this->camerasettings = val;
    };
    const Graphics::CameraSettings& GetCameraSettings() const
    {
        return this->camerasettings;
    };
private:
    Graphics::CameraSettings camerasettings;
};
//------------------------------------------------------------------------------
class CreateGlobalLightEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreateGlobalLightEntity);
    __DeclareMsgId;
public:
    CreateGlobalLightEntity() 
    { };
public:
    void SetLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->lightcolor = val;
    };
    const Math::float4& GetLightColor() const
    {
        return this->lightcolor;
    };
private:
    Math::float4 lightcolor;
public:
    void SetProjMapUvOffsetAndScale(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->projmapuvoffsetandscale = val;
    };
    const Math::float4& GetProjMapUvOffsetAndScale() const
    {
        return this->projmapuvoffsetandscale;
    };
private:
    Math::float4 projmapuvoffsetandscale;
public:
    void SetCastShadows(bool val)
    {
        n_assert(!this->handled);
        this->castshadows = val;
    };
    bool GetCastShadows() const
    {
        return this->castshadows;
    };
private:
    bool castshadows;
public:
    void SetBackLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->backlightcolor = val;
    };
    const Math::float4& GetBackLightColor() const
    {
        return this->backlightcolor;
    };
private:
    Math::float4 backlightcolor;
public:
    void SetAmbientLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->ambientlightcolor = val;
    };
    const Math::float4& GetAmbientLightColor() const
    {
        return this->ambientlightcolor;
    };
private:
    Math::float4 ambientlightcolor;
public:
    void SetBackLightOffset(float val)
    {
        n_assert(!this->handled);
        this->backlightoffset = val;
    };
    float GetBackLightOffset() const
    {
        return this->backlightoffset;
    };
private:
    float backlightoffset;
public:
    void SetShadowIntensity(float val)
    {
        n_assert(!this->handled);
        this->shadowintensity = val;
    };
    float GetShadowIntensity() const
    {
        return this->shadowintensity;
    };
private:
    float shadowintensity;
public:
    void SetVolumetric(bool val)
    {
        n_assert(!this->handled);
        this->volumetric = val;
    };
    bool GetVolumetric() const
    {
        return this->volumetric;
    };
private:
    bool volumetric;
public:
    void SetVolumetricScale(float val)
    {
        n_assert(!this->handled);
        this->volumetricscale = val;
    };
    float GetVolumetricScale() const
    {
        return this->volumetricscale;
    };
private:
    float volumetricscale;
public:
    void SetVolumetricIntensity(float val)
    {
        n_assert(!this->handled);
        this->volumetricintensity = val;
    };
    float GetVolumetricIntensity() const
    {
        return this->volumetricintensity;
    };
private:
    float volumetricintensity;
};
//------------------------------------------------------------------------------
class CreateSpotLightEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreateSpotLightEntity);
    __DeclareMsgId;
public:
    CreateSpotLightEntity() 
    { };
public:
    void SetLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->lightcolor = val;
    };
    const Math::float4& GetLightColor() const
    {
        return this->lightcolor;
    };
private:
    Math::float4 lightcolor;
public:
    void SetProjMapUvOffsetAndScale(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->projmapuvoffsetandscale = val;
    };
    const Math::float4& GetProjMapUvOffsetAndScale() const
    {
        return this->projmapuvoffsetandscale;
    };
private:
    Math::float4 projmapuvoffsetandscale;
public:
    void SetCastShadows(bool val)
    {
        n_assert(!this->handled);
        this->castshadows = val;
    };
    bool GetCastShadows() const
    {
        return this->castshadows;
    };
private:
    bool castshadows;
public:
    void SetShadowIntensity(float val)
    {
        n_assert(!this->handled);
        this->shadowintensity = val;
    };
    float GetShadowIntensity() const
    {
        return this->shadowintensity;
    };
private:
    float shadowintensity;
public:
    void SetVolumetric(bool val)
    {
        n_assert(!this->handled);
        this->volumetric = val;
    };
    bool GetVolumetric() const
    {
        return this->volumetric;
    };
private:
    bool volumetric;
public:
    void SetVolumetricScale(float val)
    {
        n_assert(!this->handled);
        this->volumetricscale = val;
    };
    float GetVolumetricScale() const
    {
        return this->volumetricscale;
    };
private:
    float volumetricscale;
public:
    void SetVolumetricIntensity(float val)
    {
        n_assert(!this->handled);
        this->volumetricintensity = val;
    };
    float GetVolumetricIntensity() const
    {
        return this->volumetricintensity;
    };
private:
    float volumetricintensity;
};
//------------------------------------------------------------------------------
class CreatePointLightEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreatePointLightEntity);
    __DeclareMsgId;
public:
    CreatePointLightEntity() 
    { };
public:
    void SetLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->lightcolor = val;
    };
    const Math::float4& GetLightColor() const
    {
        return this->lightcolor;
    };
private:
    Math::float4 lightcolor;
public:
    void SetProjMapUvOffsetAndScale(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->projmapuvoffsetandscale = val;
    };
    const Math::float4& GetProjMapUvOffsetAndScale() const
    {
        return this->projmapuvoffsetandscale;
    };
private:
    Math::float4 projmapuvoffsetandscale;
public:
    void SetCastShadows(bool val)
    {
        n_assert(!this->handled);
        this->castshadows = val;
    };
    bool GetCastShadows() const
    {
        return this->castshadows;
    };
private:
    bool castshadows;
public:
    void SetShadowIntensity(float val)
    {
        n_assert(!this->handled);
        this->shadowintensity = val;
    };
    float GetShadowIntensity() const
    {
        return this->shadowintensity;
    };
private:
    float shadowintensity;
public:
    void SetVolumetric(bool val)
    {
        n_assert(!this->handled);
        this->volumetric = val;
    };
    bool GetVolumetric() const
    {
        return this->volumetric;
    };
private:
    bool volumetric;
public:
    void SetVolumetricScale(float val)
    {
        n_assert(!this->handled);
        this->volumetricscale = val;
    };
    float GetVolumetricScale() const
    {
        return this->volumetricscale;
    };
private:
    float volumetricscale;
public:
    void SetVolumetricIntensity(float val)
    {
        n_assert(!this->handled);
        this->volumetricintensity = val;
    };
    float GetVolumetricIntensity() const
    {
        return this->volumetricintensity;
    };
private:
    float volumetricintensity;
};
//------------------------------------------------------------------------------
class CreateBillboardEntity : public CreateGraphicsEntity
{
    __DeclareClass(CreateBillboardEntity);
    __DeclareMsgId;
public:
    CreateBillboardEntity() 
    { };
public:
    void SetSize(const Math::float2& val)
    {
        n_assert(!this->handled);
        this->size = val;
    };
    const Math::float2& GetSize() const
    {
        return this->size;
    };
private:
    Math::float2 size;
public:
    void SetViewAligned(bool val)
    {
        n_assert(!this->handled);
        this->viewaligned = val;
    };
    bool GetViewAligned() const
    {
        return this->viewaligned;
    };
private:
    bool viewaligned;
public:
    void SetPickingId(IndexT val)
    {
        n_assert(!this->handled);
        this->pickingid = val;
    };
    IndexT GetPickingId() const
    {
        return this->pickingid;
    };
private:
    IndexT pickingid;
public:
    void SetResourceId(const Resources::ResourceId& val)
    {
        n_assert(!this->handled);
        this->resourceid = val;
    };
    const Resources::ResourceId& GetResourceId() const
    {
        return this->resourceid;
    };
private:
    Resources::ResourceId resourceid;
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
};
//------------------------------------------------------------------------------
class GraphicsEntityMessage : public Messaging::Message
{
    __DeclareClass(GraphicsEntityMessage);
    __DeclareMsgId;
public:
    GraphicsEntityMessage() 
    { };
public:
    void SetEntity(const Ptr<Graphics::GraphicsEntity>& val)
    {
        n_assert(!this->handled);
        this->entity = val;
    };
    const Ptr<Graphics::GraphicsEntity>& GetEntity() const
    {
        return this->entity;
    };
private:
    Ptr<Graphics::GraphicsEntity> entity;
};
//------------------------------------------------------------------------------
class DiscardGraphicsEntity : public GraphicsEntityMessage
{
    __DeclareClass(DiscardGraphicsEntity);
    __DeclareMsgId;
public:
    DiscardGraphicsEntity() 
    { };
};
//------------------------------------------------------------------------------
class AddTrackedCharJoint : public GraphicsEntityMessage
{
    __DeclareClass(AddTrackedCharJoint);
    __DeclareMsgId;
public:
    AddTrackedCharJoint() 
    { };
public:
    void SetJointName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->jointname = val;
    };
    const Util::StringAtom& GetJointName() const
    {
        return this->jointname;
    };
private:
    Util::StringAtom jointname;
};
//------------------------------------------------------------------------------
class SetLightColor : public GraphicsEntityMessage
{
    __DeclareClass(SetLightColor);
    __DeclareMsgId;
public:
    SetLightColor() 
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
};
//------------------------------------------------------------------------------
class SetLightCastShadows : public GraphicsEntityMessage
{
    __DeclareClass(SetLightCastShadows);
    __DeclareMsgId;
public:
    SetLightCastShadows() 
    { };
public:
    void SetCastShadows(bool val)
    {
        n_assert(!this->handled);
        this->castshadows = val;
    };
    bool GetCastShadows() const
    {
        return this->castshadows;
    };
private:
    bool castshadows;
};
//------------------------------------------------------------------------------
class SetLightShadowIntensity : public GraphicsEntityMessage
{
    __DeclareClass(SetLightShadowIntensity);
    __DeclareMsgId;
public:
    SetLightShadowIntensity() 
    { };
public:
    void SetIntensity(float val)
    {
        n_assert(!this->handled);
        this->intensity = val;
    };
    float GetIntensity() const
    {
        return this->intensity;
    };
private:
    float intensity;
};
//------------------------------------------------------------------------------
class SetLightShadowBias : public GraphicsEntityMessage
{
    __DeclareClass(SetLightShadowBias);
    __DeclareMsgId;
public:
    SetLightShadowBias() 
    { };
public:
    void SetBias(float val)
    {
        n_assert(!this->handled);
        this->bias = val;
    };
    float GetBias() const
    {
        return this->bias;
    };
private:
    float bias;
};
//------------------------------------------------------------------------------
class SetGlobalBackLightColor : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalBackLightColor);
    __DeclareMsgId;
public:
    SetGlobalBackLightColor() 
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
};
//------------------------------------------------------------------------------
class SetGlobalAmbientLightColor : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalAmbientLightColor);
    __DeclareMsgId;
public:
    SetGlobalAmbientLightColor() 
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
};
//------------------------------------------------------------------------------
class SetGlobalBackLightOffset : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalBackLightOffset);
    __DeclareMsgId;
public:
    SetGlobalBackLightOffset() 
    { };
public:
    void SetOffset(float val)
    {
        n_assert(!this->handled);
        this->offset = val;
    };
    float GetOffset() const
    {
        return this->offset;
    };
private:
    float offset;
};
//------------------------------------------------------------------------------
class SetGlobalLightShadowArea : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalLightShadowArea);
    __DeclareMsgId;
public:
    SetGlobalLightShadowArea() 
    { };
public:
    void SetArea(const Math::vector& val)
    {
        n_assert(!this->handled);
        this->area = val;
    };
    const Math::vector& GetArea() const
    {
        return this->area;
    };
private:
    Math::vector area;
};
//------------------------------------------------------------------------------
class SetLightVolumetric : public GraphicsEntityMessage
{
    __DeclareClass(SetLightVolumetric);
    __DeclareMsgId;
public:
    SetLightVolumetric() 
    { };
public:
    void SetEnabled(bool val)
    {
        n_assert(!this->handled);
        this->enabled = val;
    };
    bool GetEnabled() const
    {
        return this->enabled;
    };
private:
    bool enabled;
};
//------------------------------------------------------------------------------
class SetLightVolumetricScale : public GraphicsEntityMessage
{
    __DeclareClass(SetLightVolumetricScale);
    __DeclareMsgId;
public:
    SetLightVolumetricScale() 
    { };
public:
    void SetScale(float val)
    {
        n_assert(!this->handled);
        this->scale = val;
    };
    float GetScale() const
    {
        return this->scale;
    };
private:
    float scale;
};
//------------------------------------------------------------------------------
class SetLightVolumetricIntensity : public GraphicsEntityMessage
{
    __DeclareClass(SetLightVolumetricIntensity);
    __DeclareMsgId;
public:
    SetLightVolumetricIntensity() 
    { };
public:
    void SetIntensity(float val)
    {
        n_assert(!this->handled);
        this->intensity = val;
    };
    float GetIntensity() const
    {
        return this->intensity;
    };
private:
    float intensity;
};
//------------------------------------------------------------------------------
class SetGlobalLightParams : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalLightParams);
    __DeclareMsgId;
public:
    SetGlobalLightParams() 
    { };
public:
    void SetLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->lightcolor = val;
    };
    const Math::float4& GetLightColor() const
    {
        return this->lightcolor;
    };
private:
    Math::float4 lightcolor;
public:
    void SetBackLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->backlightcolor = val;
    };
    const Math::float4& GetBackLightColor() const
    {
        return this->backlightcolor;
    };
private:
    Math::float4 backlightcolor;
public:
    void SetAmbientLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->ambientlightcolor = val;
    };
    const Math::float4& GetAmbientLightColor() const
    {
        return this->ambientlightcolor;
    };
private:
    Math::float4 ambientlightcolor;
public:
    void SetBackLightOffset(float val)
    {
        n_assert(!this->handled);
        this->backlightoffset = val;
    };
    float GetBackLightOffset() const
    {
        return this->backlightoffset;
    };
private:
    float backlightoffset;
public:
    void SetCastShadows(bool val)
    {
        n_assert(!this->handled);
        this->castshadows = val;
    };
    bool GetCastShadows() const
    {
        return this->castshadows;
    };
private:
    bool castshadows;
};
//------------------------------------------------------------------------------
class SetRenderSkeleton : public GraphicsEntityMessage
{
    __DeclareClass(SetRenderSkeleton);
    __DeclareMsgId;
public:
    SetRenderSkeleton() 
    { };
public:
    void SetRender(bool val)
    {
        n_assert(!this->handled);
        this->render = val;
    };
    bool GetRender() const
    {
        return this->render;
    };
private:
    bool render;
};
//------------------------------------------------------------------------------
class SetGlobalLightColor : public GraphicsEntityMessage
{
    __DeclareClass(SetGlobalLightColor);
    __DeclareMsgId;
public:
    SetGlobalLightColor() 
    { };
public:
    void SetBackLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->backlightcolor = val;
    };
    const Math::float4& GetBackLightColor() const
    {
        return this->backlightcolor;
    };
private:
    Math::float4 backlightcolor;
public:
    void SetAmbientLightColor(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->ambientlightcolor = val;
    };
    const Math::float4& GetAmbientLightColor() const
    {
        return this->ambientlightcolor;
    };
private:
    Math::float4 ambientlightcolor;
public:
    void SetBackLightOffset(const Math::float4& val)
    {
        n_assert(!this->handled);
        this->backlightoffset = val;
    };
    const Math::float4& GetBackLightOffset() const
    {
        return this->backlightoffset;
    };
private:
    Math::float4 backlightoffset;
};
//------------------------------------------------------------------------------
class SetLightProjectionTexture : public GraphicsEntityMessage
{
    __DeclareClass(SetLightProjectionTexture);
    __DeclareMsgId;
public:
    SetLightProjectionTexture() 
    { };
public:
    void SetTexture(const Resources::ResourceId& val)
    {
        n_assert(!this->handled);
        this->texture = val;
    };
    const Resources::ResourceId& GetTexture() const
    {
        return this->texture;
    };
private:
    Resources::ResourceId texture;
};
//------------------------------------------------------------------------------
class UpdateTransform : public GraphicsEntityMessage
{
    __DeclareClass(UpdateTransform);
    __DeclareMsgId;
public:
    UpdateTransform() 
    { };
public:
    void SetTransform(const Math::matrix44& val)
    {
        n_assert(!this->handled);
        this->transform = val;
    };
    const Math::matrix44& GetTransform() const
    {
        return this->transform;
    };
private:
    Math::matrix44 transform;
};
//------------------------------------------------------------------------------
class SetVisibility : public GraphicsEntityMessage
{
    __DeclareClass(SetVisibility);
    __DeclareMsgId;
public:
    SetVisibility() 
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
class AttachCameraToView : public GraphicsEntityMessage
{
    __DeclareClass(AttachCameraToView);
    __DeclareMsgId;
public:
    AttachCameraToView() 
    { };
public:
    void SetViewName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->viewname = val;
    };
    const Util::StringAtom& GetViewName() const
    {
        return this->viewname;
    };
private:
    Util::StringAtom viewname;
};
//------------------------------------------------------------------------------
class RemoveCameraFromView : public GraphicsEntityMessage
{
    __DeclareClass(RemoveCameraFromView);
    __DeclareMsgId;
public:
    RemoveCameraFromView() 
    { };
public:
    void SetViewName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->viewname = val;
    };
    const Util::StringAtom& GetViewName() const
    {
        return this->viewname;
    };
private:
    Util::StringAtom viewname;
};
//------------------------------------------------------------------------------
class UpdModelNodeInstanceMaterialVariable : public GraphicsEntityMessage
{
    __DeclareClass(UpdModelNodeInstanceMaterialVariable);
    __DeclareMsgId;
public:
    UpdModelNodeInstanceMaterialVariable() 
    { };
public:
    void SetModelNodeInstanceName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->modelnodeinstancename = val;
    };
    const Util::StringAtom& GetModelNodeInstanceName() const
    {
        return this->modelnodeinstancename;
    };
private:
    Util::StringAtom modelnodeinstancename;
public:
    void SetSemantic(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->semantic = val;
    };
    const Util::StringAtom& GetSemantic() const
    {
        return this->semantic;
    };
private:
    Util::StringAtom semantic;
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
};
//------------------------------------------------------------------------------
class UpdModelNodeInstanceVisibility : public GraphicsEntityMessage
{
    __DeclareClass(UpdModelNodeInstanceVisibility);
    __DeclareMsgId;
public:
    UpdModelNodeInstanceVisibility() 
    { };
public:
    void SetModelNodeInstanceName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->modelnodeinstancename = val;
    };
    const Util::StringAtom& GetModelNodeInstanceName() const
    {
        return this->modelnodeinstancename;
    };
private:
    Util::StringAtom modelnodeinstancename;
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
class UpdateSharedShaderVariable : public Messaging::Message
{
    __DeclareClass(UpdateSharedShaderVariable);
    __DeclareMsgId;
public:
    UpdateSharedShaderVariable() 
    { };
public:
    void SetSemantic(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->semantic = val;
    };
    const Util::StringAtom& GetSemantic() const
    {
        return this->semantic;
    };
private:
    Util::StringAtom semantic;
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
};
//------------------------------------------------------------------------------
class SetOverlayColor : public GraphicsEntityMessage
{
    __DeclareClass(SetOverlayColor);
    __DeclareMsgId;
public:
    SetOverlayColor() 
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
};
//------------------------------------------------------------------------------
class AnimPlayClip : public GraphicsEntityMessage
{
    __DeclareClass(AnimPlayClip);
    __DeclareMsgId;
public:
    AnimPlayClip() :
        trackindex(0),
        loopcount(1.0f),
        starttime(0),
        fadeintime(200),
        fadeouttime(200),
        timeoffset(0),
        timefactor(1.0f),
        blendweight(1.0f),
        exclusivetag(InvalidIndex),
        enqueuemode(Animation::AnimJobEnqueueMode::Intercept)
    { };
public:
    void SetClipName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->clipname = val;
    };
    const Util::StringAtom& GetClipName() const
    {
        return this->clipname;
    };
private:
    Util::StringAtom clipname;
public:
    void SetTrackIndex(IndexT val)
    {
        n_assert(!this->handled);
        this->trackindex = val;
    };
    IndexT GetTrackIndex() const
    {
        return this->trackindex;
    };
private:
    IndexT trackindex;
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
    void SetStartTime(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->starttime = val;
    };
    const Timing::Tick& GetStartTime() const
    {
        return this->starttime;
    };
private:
    Timing::Tick starttime;
public:
    void SetFadeInTime(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->fadeintime = val;
    };
    const Timing::Tick& GetFadeInTime() const
    {
        return this->fadeintime;
    };
private:
    Timing::Tick fadeintime;
public:
    void SetFadeOutTime(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->fadeouttime = val;
    };
    const Timing::Tick& GetFadeOutTime() const
    {
        return this->fadeouttime;
    };
private:
    Timing::Tick fadeouttime;
public:
    void SetTimeOffset(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->timeoffset = val;
    };
    const Timing::Tick& GetTimeOffset() const
    {
        return this->timeoffset;
    };
private:
    Timing::Tick timeoffset;
public:
    void SetTimeFactor(float val)
    {
        n_assert(!this->handled);
        this->timefactor = val;
    };
    float GetTimeFactor() const
    {
        return this->timefactor;
    };
private:
    float timefactor;
public:
    void SetBlendWeight(float val)
    {
        n_assert(!this->handled);
        this->blendweight = val;
    };
    float GetBlendWeight() const
    {
        return this->blendweight;
    };
private:
    float blendweight;
public:
    void SetExclusiveTag(IndexT val)
    {
        n_assert(!this->handled);
        this->exclusivetag = val;
    };
    IndexT GetExclusiveTag() const
    {
        return this->exclusivetag;
    };
private:
    IndexT exclusivetag;
public:
    void SetEnqueueMode(const Animation::AnimJobEnqueueMode::Code& val)
    {
        n_assert(!this->handled);
        this->enqueuemode = val;
    };
    const Animation::AnimJobEnqueueMode::Code& GetEnqueueMode() const
    {
        return this->enqueuemode;
    };
private:
    Animation::AnimJobEnqueueMode::Code enqueuemode;
public:
    void Encode(const Ptr<IO::BinaryWriter>& writer)
    {
        writer->WriteString(this->GetClipName().Value());
        writer->WriteInt(this->GetTrackIndex());
        writer->WriteFloat(this->GetLoopCount());
        writer->WriteInt(this->GetStartTime());
        writer->WriteInt(this->GetFadeInTime());
        writer->WriteInt(this->GetFadeOutTime());
        writer->WriteInt(this->GetTimeOffset());
        writer->WriteFloat(this->GetTimeFactor());
        writer->WriteFloat(this->GetBlendWeight());
        writer->WriteInt(this->GetExclusiveTag());
        writer->WriteInt((int)this->GetEnqueueMode());
        GraphicsEntityMessage::Encode(writer);
    };
public:
    void Decode(const Ptr<IO::BinaryReader>& reader)
    {
        this->SetClipName(reader->ReadString());
        this->SetTrackIndex(reader->ReadInt());
        this->SetLoopCount(reader->ReadFloat());
        this->SetStartTime(reader->ReadInt());
        this->SetFadeInTime(reader->ReadInt());
        this->SetFadeOutTime(reader->ReadInt());
        this->SetTimeOffset(reader->ReadInt());
        this->SetTimeFactor(reader->ReadFloat());
        this->SetBlendWeight(reader->ReadFloat());
        this->SetExclusiveTag(reader->ReadInt());
        this->SetEnqueueMode((Animation::AnimJobEnqueueMode::Code)reader->ReadInt());
        GraphicsEntityMessage::Decode(reader);
    };
};
//------------------------------------------------------------------------------
class FetchClips : public GraphicsEntityMessage
{
    __DeclareClass(FetchClips);
    __DeclareMsgId;
public:
    FetchClips() 
    { };
public:
    void SetClips(const Util::Array<Util::StringAtom>& val)
    {
        n_assert(!this->handled);
        this->clips = val;
    };
    const Util::Array<Util::StringAtom>& GetClips() const
    {
        n_assert(this->handled);
        return this->clips;
    };
private:
    Util::Array<Util::StringAtom> clips;
public:
    void SetLenghts(const Util::Array<SizeT>& val)
    {
        n_assert(!this->handled);
        this->lenghts = val;
    };
    const Util::Array<SizeT>& GetLenghts() const
    {
        n_assert(this->handled);
        return this->lenghts;
    };
private:
    Util::Array<SizeT> lenghts;
};
//------------------------------------------------------------------------------
class AnimStopTrack : public GraphicsEntityMessage
{
    __DeclareClass(AnimStopTrack);
    __DeclareMsgId;
public:
    AnimStopTrack() :
        trackindex(0),
        allowfadeout(true)
    { };
public:
    void SetTrackIndex(IndexT val)
    {
        n_assert(!this->handled);
        this->trackindex = val;
    };
    IndexT GetTrackIndex() const
    {
        return this->trackindex;
    };
private:
    IndexT trackindex;
public:
    void SetAllowFadeOut(bool val)
    {
        n_assert(!this->handled);
        this->allowfadeout = val;
    };
    bool GetAllowFadeOut() const
    {
        return this->allowfadeout;
    };
private:
    bool allowfadeout;
public:
    void Encode(const Ptr<IO::BinaryWriter>& writer)
    {
        writer->WriteInt(this->GetTrackIndex());
        GraphicsEntityMessage::Encode(writer);
    };
public:
    void Decode(const Ptr<IO::BinaryReader>& reader)
    {
        this->SetTrackIndex(reader->ReadInt());
        GraphicsEntityMessage::Decode(reader);
    };
};
//------------------------------------------------------------------------------
class AnimStopAllTracks : public GraphicsEntityMessage
{
    __DeclareClass(AnimStopAllTracks);
    __DeclareMsgId;
public:
    AnimStopAllTracks() :
        allowfadeout(true)
    { };
public:
    void SetAllowFadeOut(bool val)
    {
        n_assert(!this->handled);
        this->allowfadeout = val;
    };
    bool GetAllowFadeOut() const
    {
        return this->allowfadeout;
    };
private:
    bool allowfadeout;
};
//------------------------------------------------------------------------------
class AnimPauseTrack : public GraphicsEntityMessage
{
    __DeclareClass(AnimPauseTrack);
    __DeclareMsgId;
public:
    AnimPauseTrack() :
        trackindex(0),
        allowfadeout(true)
    { };
public:
    void SetTrackIndex(IndexT val)
    {
        n_assert(!this->handled);
        this->trackindex = val;
    };
    IndexT GetTrackIndex() const
    {
        return this->trackindex;
    };
private:
    IndexT trackindex;
public:
    void SetAllowFadeOut(bool val)
    {
        n_assert(!this->handled);
        this->allowfadeout = val;
    };
    bool GetAllowFadeOut() const
    {
        return this->allowfadeout;
    };
private:
    bool allowfadeout;
};
//------------------------------------------------------------------------------
class AnimPauseAllTracks : public GraphicsEntityMessage
{
    __DeclareClass(AnimPauseAllTracks);
    __DeclareMsgId;
public:
    AnimPauseAllTracks() 
    { };
};
//------------------------------------------------------------------------------
class AnimSeek : public GraphicsEntityMessage
{
    __DeclareClass(AnimSeek);
    __DeclareMsgId;
public:
    AnimSeek() 
    { };
public:
    void SetTime(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->time = val;
    };
    const Timing::Tick& GetTime() const
    {
        return this->time;
    };
private:
    Timing::Tick time;
};
//------------------------------------------------------------------------------
class AnimTime : public GraphicsEntityMessage
{
    __DeclareClass(AnimTime);
    __DeclareMsgId;
public:
    AnimTime() 
    { };
public:
    void SetTime(const Timing::Tick& val)
    {
        n_assert(!this->handled);
        this->time = val;
    };
    const Timing::Tick& GetTime() const
    {
        return this->time;
    };
private:
    Timing::Tick time;
};
//------------------------------------------------------------------------------
class AnimModifyBlendWeight : public GraphicsEntityMessage
{
    __DeclareClass(AnimModifyBlendWeight);
    __DeclareMsgId;
public:
    AnimModifyBlendWeight() :
        trackindex(0),
        blendweight(1.0f)
    { };
public:
    void SetTrackIndex(IndexT val)
    {
        n_assert(!this->handled);
        this->trackindex = val;
    };
    IndexT GetTrackIndex() const
    {
        return this->trackindex;
    };
private:
    IndexT trackindex;
public:
    void SetBlendWeight(float val)
    {
        n_assert(!this->handled);
        this->blendweight = val;
    };
    float GetBlendWeight() const
    {
        return this->blendweight;
    };
private:
    float blendweight;
};
//------------------------------------------------------------------------------
class AnimModifyTimeFactor : public GraphicsEntityMessage
{
    __DeclareClass(AnimModifyTimeFactor);
    __DeclareMsgId;
public:
    AnimModifyTimeFactor() :
        trackindex(0),
        timefactor(1.0)
    { };
public:
    void SetTrackIndex(IndexT val)
    {
        n_assert(!this->handled);
        this->trackindex = val;
    };
    IndexT GetTrackIndex() const
    {
        return this->trackindex;
    };
private:
    IndexT trackindex;
public:
    void SetTimeFactor(float val)
    {
        n_assert(!this->handled);
        this->timefactor = val;
    };
    float GetTimeFactor() const
    {
        return this->timefactor;
    };
private:
    float timefactor;
};
//------------------------------------------------------------------------------
class AnimIsClipPlaying : public GraphicsEntityMessage
{
    __DeclareClass(AnimIsClipPlaying);
    __DeclareMsgId;
public:
    AnimIsClipPlaying() 
    { };
public:
    void SetClipName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->clipname = val;
    };
    const Util::StringAtom& GetClipName() const
    {
        return this->clipname;
    };
private:
    Util::StringAtom clipname;
public:
    void SetPlaying(bool val)
    {
        n_assert(!this->handled);
        this->playing = val;
    };
    bool GetPlaying() const
    {
        n_assert(this->handled);
        return this->playing;
    };
private:
    bool playing;
};
//------------------------------------------------------------------------------
class ApplySkinList : public GraphicsEntityMessage
{
    __DeclareClass(ApplySkinList);
    __DeclareMsgId;
public:
    ApplySkinList() 
    { };
public:
    void SetSkinList(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->skinlist = val;
    };
    const Util::StringAtom& GetSkinList() const
    {
        return this->skinlist;
    };
private:
    Util::StringAtom skinlist;
};
//------------------------------------------------------------------------------
class FetchSkinList : public GraphicsEntityMessage
{
    __DeclareClass(FetchSkinList);
    __DeclareMsgId;
public:
    FetchSkinList() 
    { };
public:
    void SetSkins(const Util::Array<Util::StringAtom>& val)
    {
        n_assert(!this->handled);
        this->skins = val;
    };
    const Util::Array<Util::StringAtom>& GetSkins() const
    {
        n_assert(this->handled);
        return this->skins;
    };
private:
    Util::Array<Util::StringAtom> skins;
};
//------------------------------------------------------------------------------
class ShowSkin : public GraphicsEntityMessage
{
    __DeclareClass(ShowSkin);
    __DeclareMsgId;
public:
    ShowSkin() 
    { };
public:
    void SetSkin(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->skin = val;
    };
    const Util::StringAtom& GetSkin() const
    {
        return this->skin;
    };
private:
    Util::StringAtom skin;
};
//------------------------------------------------------------------------------
class HideSkin : public GraphicsEntityMessage
{
    __DeclareClass(HideSkin);
    __DeclareMsgId;
public:
    HideSkin() 
    { };
public:
    void SetSkin(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->skin = val;
    };
    const Util::StringAtom& GetSkin() const
    {
        return this->skin;
    };
private:
    Util::StringAtom skin;
};
//------------------------------------------------------------------------------
class SetVariation : public GraphicsEntityMessage
{
    __DeclareClass(SetVariation);
    __DeclareMsgId;
public:
    SetVariation() 
    { };
public:
    void SetVariationName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->variationname = val;
    };
    const Util::StringAtom& GetVariationName() const
    {
        return this->variationname;
    };
private:
    Util::StringAtom variationname;
};
//------------------------------------------------------------------------------
class SetTimeFactor : public Messaging::Message
{
    __DeclareClass(SetTimeFactor);
    __DeclareMsgId;
public:
    SetTimeFactor() 
    { };
public:
    void SetFactor(float val)
    {
        n_assert(!this->handled);
        this->factor = val;
    };
    float GetFactor() const
    {
        return this->factor;
    };
private:
    float factor;
};
//------------------------------------------------------------------------------
class UnbindAttachment : public GraphicsEntityMessage
{
    __DeclareClass(UnbindAttachment);
    __DeclareMsgId;
public:
    UnbindAttachment() :
        cleartype(Graphics::AttachmentServer::ClearNone)
    { };
public:
    void SetEntityToAttach(const Ptr<Graphics::GraphicsEntity>& val)
    {
        n_assert(!this->handled);
        this->entitytoattach = val;
    };
    const Ptr<Graphics::GraphicsEntity>& GetEntityToAttach() const
    {
        return this->entitytoattach;
    };
private:
    Ptr<Graphics::GraphicsEntity> entitytoattach;
public:
    void SetJoint(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->joint = val;
    };
    const Util::StringAtom& GetJoint() const
    {
        return this->joint;
    };
private:
    Util::StringAtom joint;
public:
    void SetClearType(const Graphics::AttachmentServer::ClearType& val)
    {
        n_assert(!this->handled);
        this->cleartype = val;
    };
    const Graphics::AttachmentServer::ClearType& GetClearType() const
    {
        return this->cleartype;
    };
private:
    Graphics::AttachmentServer::ClearType cleartype;
};
//------------------------------------------------------------------------------
class BindAttachment : public UnbindAttachment
{
    __DeclareClass(BindAttachment);
    __DeclareMsgId;
public:
    BindAttachment() 
    { };
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
    void SetRotation(const Graphics::AttachmentServer::AttachmentRotation& val)
    {
        n_assert(!this->handled);
        this->rotation = val;
    };
    const Graphics::AttachmentServer::AttachmentRotation& GetRotation() const
    {
        return this->rotation;
    };
private:
    Graphics::AttachmentServer::AttachmentRotation rotation;
public:
    void SetOffset(const Math::matrix44& val)
    {
        n_assert(!this->handled);
        this->offset = val;
    };
    const Math::matrix44& GetOffset() const
    {
        return this->offset;
    };
private:
    Math::matrix44 offset;
};
//------------------------------------------------------------------------------
class SwitchAttachmentToNewJoint : public BindAttachment
{
    __DeclareClass(SwitchAttachmentToNewJoint);
    __DeclareMsgId;
public:
    SwitchAttachmentToNewJoint() 
    { };
public:
    void SetNewJoint(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->newjoint = val;
    };
    const Util::StringAtom& GetNewJoint() const
    {
        return this->newjoint;
    };
private:
    Util::StringAtom newjoint;
};
//------------------------------------------------------------------------------
class BaseAnimEventMessage : public GraphicsEntityMessage
{
    __DeclareClass(BaseAnimEventMessage);
    __DeclareMsgId;
public:
    BaseAnimEventMessage() 
    { };
};
//------------------------------------------------------------------------------
class RegisterAnimEventHandler : public Messaging::Message
{
    __DeclareClass(RegisterAnimEventHandler);
    __DeclareMsgId;
public:
    RegisterAnimEventHandler() 
    { };
public:
    void SetAnimEventHandler(const Ptr<Animation::AnimEventHandlerBase>& val)
    {
        n_assert(!this->handled);
        this->animeventhandler = val;
    };
    const Ptr<Animation::AnimEventHandlerBase>& GetAnimEventHandler() const
    {
        return this->animeventhandler;
    };
private:
    Ptr<Animation::AnimEventHandlerBase> animeventhandler;
};
//------------------------------------------------------------------------------
class UnregisterAnimEventHandler : public Messaging::Message
{
    __DeclareClass(UnregisterAnimEventHandler);
    __DeclareMsgId;
public:
    UnregisterAnimEventHandler() 
    { };
public:
    void SetCategoryName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->categoryname = val;
    };
    const Util::StringAtom& GetCategoryName() const
    {
        return this->categoryname;
    };
private:
    Util::StringAtom categoryname;
};
//------------------------------------------------------------------------------
class PreloadMousePointerTextures : public Messaging::Message
{
    __DeclareClass(PreloadMousePointerTextures);
    __DeclareMsgId;
public:
    PreloadMousePointerTextures() 
    { };
public:
    void SetResourceIds(const Util::Array<Resources::ResourceId>& val)
    {
        n_assert(!this->handled);
        this->resourceids = val;
    };
    const Util::Array<Resources::ResourceId>& GetResourceIds() const
    {
        return this->resourceids;
    };
private:
    Util::Array<Resources::ResourceId> resourceids;
};
//------------------------------------------------------------------------------
class UpdateMousePointers : public Messaging::Message
{
    __DeclareClass(UpdateMousePointers);
    __DeclareMsgId;
public:
    UpdateMousePointers() 
    { };
public:
    void SetPointers(const Util::Array<CoreGraphics::MousePointer>& val)
    {
        n_assert(!this->handled);
        this->pointers = val;
    };
    const Util::Array<CoreGraphics::MousePointer>& GetPointers() const
    {
        return this->pointers;
    };
private:
    Util::Array<CoreGraphics::MousePointer> pointers;
};
//------------------------------------------------------------------------------
class HoldSharedResources : public Messaging::Message
{
    __DeclareClass(HoldSharedResources);
    __DeclareMsgId;
public:
    HoldSharedResources() 
    { };
};
//------------------------------------------------------------------------------
class ReleaseSharedResources : public Messaging::Message
{
    __DeclareClass(ReleaseSharedResources);
    __DeclareMsgId;
public:
    ReleaseSharedResources() 
    { };
};
//------------------------------------------------------------------------------
class SetAnimatorTime : public GraphicsEntityMessage
{
    __DeclareClass(SetAnimatorTime);
    __DeclareMsgId;
public:
    SetAnimatorTime() 
    { };
public:
    void SetNodeInstanceName(const Util::StringAtom& val)
    {
        n_assert(!this->handled);
        this->nodeinstancename = val;
    };
    const Util::StringAtom& GetNodeInstanceName() const
    {
        return this->nodeinstancename;
    };
private:
    Util::StringAtom nodeinstancename;
public:
    void SetTime(const Timing::Time& val)
    {
        n_assert(!this->handled);
        this->time = val;
    };
    const Timing::Time& GetTime() const
    {
        return this->time;
    };
private:
    Timing::Time time;
};
//------------------------------------------------------------------------------
class UpdateCameraEntity : public GraphicsEntityMessage
{
    __DeclareClass(UpdateCameraEntity);
    __DeclareMsgId;
public:
    UpdateCameraEntity() 
    { };
public:
    void SetCameraSettings(const Graphics::CameraSettings& val)
    {
        n_assert(!this->handled);
        this->camerasettings = val;
    };
    const Graphics::CameraSettings& GetCameraSettings() const
    {
        return this->camerasettings;
    };
private:
    Graphics::CameraSettings camerasettings;
};
//------------------------------------------------------------------------------
class SetShadowPointOfInterest : public Messaging::Message
{
    __DeclareClass(SetShadowPointOfInterest);
    __DeclareMsgId;
public:
    SetShadowPointOfInterest() 
    { };
public:
    void SetPoi(const Math::point& val)
    {
        n_assert(!this->handled);
        this->poi = val;
    };
    const Math::point& GetPoi() const
    {
        return this->poi;
    };
private:
    Math::point poi;
};
//------------------------------------------------------------------------------
class SetParticleSystemPlaying : public GraphicsEntityMessage
{
    __DeclareClass(SetParticleSystemPlaying);
    __DeclareMsgId;
public:
    SetParticleSystemPlaying() 
    { };
public:
    void SetNode(const Util::String& val)
    {
        n_assert(!this->handled);
        this->node = val;
    };
    const Util::String& GetNode() const
    {
        return this->node;
    };
private:
    Util::String node;
public:
    void SetPlaying(bool val)
    {
        n_assert(!this->handled);
        this->playing = val;
    };
    bool GetPlaying() const
    {
        return this->playing;
    };
private:
    bool playing;
};
//------------------------------------------------------------------------------
class ToggleParticleSystemPlaying : public GraphicsEntityMessage
{
    __DeclareClass(ToggleParticleSystemPlaying);
    __DeclareMsgId;
public:
    ToggleParticleSystemPlaying() 
    { };
public:
    void SetNode(const Util::String& val)
    {
        n_assert(!this->handled);
        this->node = val;
    };
    const Util::String& GetNode() const
    {
        return this->node;
    };
private:
    Util::String node;
};
//------------------------------------------------------------------------------
class ShowSystemCursor : public Messaging::Message
{
    __DeclareClass(ShowSystemCursor);
    __DeclareMsgId;
public:
    ShowSystemCursor() 
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
} // namespace Graphics
//------------------------------------------------------------------------------
