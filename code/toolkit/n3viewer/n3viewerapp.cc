//------------------------------------------------------------------------------
//  n3viewerapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n3viewerapp.h"
#include "system/nebulasettings.h"
#include "input/keyboard.h"
#include "debugrender/debugrender.h"

namespace Toolkit
{
using namespace IO;
using namespace Util;
using namespace App;
using namespace Remote;
using namespace Graphics;
using namespace Math;
using namespace System;
using namespace Input;

__ImplementSingleton(Toolkit::N3ViewerApp);

//------------------------------------------------------------------------------
/**
*/
N3ViewerApp::N3ViewerApp() :
	skinIterator(0),
	clipIterator(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
N3ViewerApp::~N3ViewerApp()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
N3ViewerApp::Open()
{
    if (ViewerApplication::Open())
    {
        // setup remote subsystem
        this->remoteInterface = RemoteInterface::Create();
        if (this->args.HasArg("-commandport"))
        {
            this->remoteInterface->SetPortNumber(this->args.GetInt("-commandport",2102));
        }
        this->remoteInterface->Open();
        this->remoteControlProxy = RemoteControlProxy::Create();
        this->remoteControlProxy->Open();
        this->remoteControlProxy->AttachCommandHandler(N3ViewerRemoteCommandHandler::Create());

        // setup the lights
        this->SetupLights();

        // parse command line args
        this->SetupSceneFromCmdLineArgs();

        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::Close()
{
    // discard scene objects
    this->DiscardModels();
    this->DiscardLights();

    // shutdown remote system
    this->remoteControlProxy->Close();
    this->remoteControlProxy = 0;
    this->remoteInterface->Close();
    this->remoteInterface = 0;

    ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::OnConfigureDisplay()
{
    // setup assigns, system resources and shaders need to be loaded
    // from the toolkit directory, while all other resources must
    // be loaded from the project directory
    String exportDir = AssignRegistry::Instance()->GetAssign("export").Tokenize(":/").Back();
    String homeShdPath, homeFramePath, homeMshPath, homeTexPath, homeAnimPath;
    homeShdPath.Format("home:%s/shaders", exportDir.AsCharPtr());
    homeFramePath.Format("home:%s/frame", exportDir.AsCharPtr());
    homeMshPath.Format("home:%s/meshes", exportDir.AsCharPtr());
    homeTexPath.Format("home:%s/textures", exportDir.AsCharPtr());
    homeAnimPath.Format("home:%s/anims", exportDir.AsCharPtr());
        
    AssignRegistry* assReg = AssignRegistry::Instance();
    assReg->SetAssign(Assign("shd", homeShdPath));
    assReg->SetAssign(Assign("frame", homeFramePath));
    assReg->SetAssign(Assign("sysmsh", homeMshPath));
    assReg->SetAssign(Assign("systex", homeTexPath));
    assReg->SetAssign(Assign("sysanim", homeAnimPath));

    String projDir = this->QueryProjectPathFromRegistry();
    assReg->SetAssign(Assign("root", projDir));

    this->display->Settings().SetDisplayModeSwitchEnabled(false);

    ViewerApplication::OnConfigureDisplay();
}

//------------------------------------------------------------------------------
/**
	Traverses skins and clips. Get the next skin by pressing E, previous by Q.
	Same goes for clips, and S stops the current animation.
*/
void
N3ViewerApp::OnProcessInput()
{
	const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	if (!skins.IsEmpty())
	{
		if (keyboard->KeyDown(Key::Q))
		{
			Util::StringAtom skin = this->skins[skinIterator];
			this->RemoveSkin(this->modelEntities[0]->GetResourceId(), skin);
			skinIterator--;
			skinIterator = max(skinIterator, 0);

			skin = this->skins[skinIterator];
			this->LoadSkin(this->modelEntities[0]->GetResourceId(), skin);
		}
		else if (keyboard->KeyDown(Key::E))
		{
			Util::StringAtom skin = this->skins[skinIterator];
			this->RemoveSkin(this->modelEntities[0]->GetResourceId(), skin);

			skinIterator++;
			skinIterator = min(skinIterator, this->skins.Size()-1);
			skin = this->skins[skinIterator];
			this->LoadSkin(this->modelEntities[0]->GetResourceId(), skin);
		}
		if (!clips.IsEmpty())
		{
			if (keyboard->KeyDown(Key::A))
			{
				clipIterator--;
				clipIterator = max(min(clipIterator, this->clips.Size()-1), 0);
				Util::StringAtom clip = this->clips[clipIterator];
				this->LoadAnimation(this->modelEntities[0]->GetResourceId(), clip);
			}
			else if (keyboard->KeyDown(Key::S))
			{
				this->StopAnimation(this->modelEntities[0]->GetResourceId());
			}
			else if (keyboard->KeyDown(Key::D))
			{
				clipIterator++;
				clipIterator = max(min(clipIterator, this->clips.Size()-1), 0);
				Util::StringAtom clip = this->clips[clipIterator];
				this->LoadAnimation(this->modelEntities[0]->GetResourceId(), clip);
			}
			else if (keyboard->KeyDown(Key::W))
			{
				this->LoadAnimation(this->modelEntities[0]->GetResourceId(), this->clips[clipIterator]);
			}
		}

	}

	if(keyboard->KeyPressed(Input::Key::Prior))
	{
		this->globalColor += float4(0.001,0.001,0.001,0);
		this->globalColor = float4::clamp(this->globalColor,float4(0,0,0,0),float4(1,1,1,1));
		this->globalLightEntity->SetColor(this->globalColor);
	}
	if(keyboard->KeyPressed(Input::Key::Next))
	{
		this->globalColor -= float4(0.001,0.001,0.001,0);
		this->globalColor = float4::clamp(this->globalColor,float4(0,0,0,0),float4(1,1,1,1));
		this->globalLightEntity->SetColor(this->globalColor);
	}

	
    // handle remote commands
    this->remoteControlProxy->HandlePendingRequests();
    this->remoteControlProxy->HandlePendingCommands();

    ViewerApplication::OnProcessInput();
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::OnUpdateFrame()
{
	this->HandleReceivedMessages();

	if (this->clips.Size() == 0)
	{
		_debug_text("No clips available", Math::float2(0.01f,0.01f), Math::float4(1,1,1,1));
	}
	else
	{
		Util::String clipText;
		clipText.Format("Currently playing: %s", this->clips[clipIterator]);
		_debug_text(clipText, Math::float2(0.01f,0.01f), Math::float4(1,1,1,1));
	}

	if (this->skins.Size() == 0)
	{
		_debug_text("No skins available", Math::float2(0.01f,0.03f), Math::float4(1,1,1,1));
	}
	else
	{
		Util::String skinText;
		skinText.Format("Currently showing: %s", this->skins[skinIterator]);
		_debug_text(skinText, Math::float2(0.01f,0.03f), Math::float4(1,1,1,1));
	}

	_debug_text("Press A and D to scroll between clips", Math::float2(0.01f, 0.05f), Math::float4(1,0,0,1));
	_debug_text("Press W and S to restart/stop the current clip", Math::float2(0.01f, 0.07f), Math::float4(1,0,0,1));
	_debug_text("Press Q and E to scroll between skins", Math::float2(0.01f, 0.09f), Math::float4(1,0,0,1));
	
	// have work-light (make global light follow camera)
	this->globalLightEntity->SetTransform(this->camera->GetTransform());
    ViewerApplication::OnUpdateFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::SetupSceneFromCmdLineArgs()
{
    // setup the initial camera position
    Math::point eyePos = this->args.GetFloat4("-eyepos", Math::point(0.0f, 0.0f, 10.0f));
    Math::point eyeCoi = this->args.GetFloat4("-eyecoi", Math::point(0.0f, 0.0f, 0.0f));
    Math::vector eyeUp = this->args.GetFloat4("-eyeupx", Math::vector(0.0f, 1.0f, 0.0f));
#ifndef FREECAM
    this->mayaCameraUtil.Setup(eyeCoi, eyePos, eyeUp);
#else
	this->freeCameraUtil.Setup(eyePos, eyePos - eyeCoi);
#endif

    // read model argument and load object
    if (this->args.HasArg("-view"))
    {
        this->LoadModel(this->args.GetString("-view"));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::SetupLights()
{
    n_assert(!this->globalLightEntity.isvalid());

    // setup the global light
    matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-70.0f));
    this->globalLightEntity = GlobalLightEntity::Create();
    this->globalLightEntity->SetTransform(lightTransform);
    this->globalLightEntity->SetColor(float4(1.0f, 1.0f, 1.0f, 1.0f));
    this->globalLightEntity->SetBackLightColor(float4(0.2f, 0.2f, 0.2f, 1.0f));
	this->globalColor = this->globalLightEntity->GetColor();
    //this->globalLightEntity->SetAmbientLightColor(float4(0.05f, 0.05f, 0.05f, 1.0f));
    this->globalLightEntity->SetCastShadows(false);
    this->stage->AttachEntity(this->globalLightEntity.cast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::DiscardLights()
{
    // discard the global light entity
    this->stage->RemoveEntity(this->globalLightEntity.cast<GraphicsEntity>());
    this->globalLightEntity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::LoadModel(const Resources::ResourceId& resId)
{
	this->skins.Clear();
	this->clips.Clear();

    Ptr<ModelEntity> modelEntity = ModelEntity::Create();
    modelEntity->SetResourceId(resId);
    this->stage->AttachEntity(modelEntity.cast<GraphicsEntity>());
    this->modelEntities.Append(modelEntity);

	Ptr<FetchClips> fetchClipsMessage = FetchClips::Create();
	modelEntity->SendMsg(fetchClipsMessage.downcast<GraphicsEntityMessage>());
	this->deferredMessages.Push(fetchClipsMessage.downcast<GraphicsEntityMessage>());

	Ptr<FetchSkinList> fetchSkinsMessage = FetchSkinList::Create();
	modelEntity->SendMsg(fetchSkinsMessage.downcast<GraphicsEntityMessage>());
	this->deferredMessages.Push(fetchSkinsMessage.downcast<GraphicsEntityMessage>());
}

//------------------------------------------------------------------------------
/**	
*/
void
N3ViewerApp::LoadSkin(const Resources::ResourceId& modelResId, const Resources::ResourceId& skinResId)
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        if (this->modelEntities[i]->GetResourceId() == modelResId)
        {
            // apply skin
	            Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
            showSkin->SetSkin(skinResId);
            this->modelEntities[i]->SendMsg(showSkin.cast<GraphicsEntityMessage>());
        }
    }
}

//------------------------------------------------------------------------------
/**	
*/
void
N3ViewerApp::RemoveSkin(const Resources::ResourceId& modelResId, const Resources::ResourceId& skinResId)
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        if (this->modelEntities[i]->GetResourceId() == modelResId)
        {
            // apply skin
            Ptr<Graphics::HideSkin> hideSkin = Graphics::HideSkin::Create();
            hideSkin->SetSkin(skinResId);
            this->modelEntities[i]->SendMsg(hideSkin.cast<GraphicsEntityMessage>());
        }
    }
}

//------------------------------------------------------------------------------
/**	
*/
void
N3ViewerApp::LoadAnimation(const Resources::ResourceId& modelResId, const Resources::ResourceId& animResId)
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        if (this->modelEntities[i]->GetResourceId() == modelResId)
        {
            // replace animation
            Ptr<Graphics::AnimStopAllTracks> stopAll = Graphics::AnimStopAllTracks::Create();
            stopAll->SetAllowFadeOut(false);
            this->modelEntities[i]->SendMsg(stopAll.cast<GraphicsEntityMessage>());
            Ptr<Graphics::AnimPlayClip> playClip = Graphics::AnimPlayClip::Create();
            playClip->SetBlendWeight(1);
            playClip->SetClipName(animResId);
            playClip->SetFadeInTime(0);
            playClip->SetFadeOutTime(0);
            playClip->SetLoopCount(0);
            this->modelEntities[i]->SendMsg(playClip.cast<GraphicsEntityMessage>());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
N3ViewerApp::StopAnimation( const Resources::ResourceId& modelResId )
{
	IndexT i;
	for (i = 0; i < this->modelEntities.Size(); i++)
	{
		if (this->modelEntities[i]->GetResourceId() == modelResId)
		{
			Ptr<Graphics::AnimStopAllTracks> stopAll = Graphics::AnimStopAllTracks::Create();
			stopAll->SetAllowFadeOut(false);
			this->modelEntities[i]->SendMsg(stopAll.cast<GraphicsEntityMessage>());
		}
	}
}

//------------------------------------------------------------------------------
/**	
*/
void
N3ViewerApp::LoadVariation(const Resources::ResourceId& modelResId, const Resources::ResourceId& varResId)
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        if (this->modelEntities[i]->GetResourceId() == modelResId)
        {
            // replace variation
            Ptr<Graphics::SetVariation> setVar = Graphics::SetVariation::Create();
            setVar->SetVariationName(varResId);
            this->modelEntities[i]->SendMsg(setVar.cast<GraphicsEntityMessage>());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerApp::DiscardModels()
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        this->stage->RemoveEntity(this->modelEntities[i].cast<GraphicsEntity>());
    }
    this->modelEntities.Clear();
}

//------------------------------------------------------------------------------
/**
*/
String
N3ViewerApp::QueryProjectPathFromRegistry() const
{
    String projDirectory;
    if (NebulaSettings::Exists("gscept","ToolkitShared", "workdir"))
    {
        projDirectory = NebulaSettings::ReadString("gscept","ToolkitShared", "workdir");
    }
    else
    {
        projDirectory = "home:";
    }
    return projDirectory;
}

//------------------------------------------------------------------------------
/**
*/
void 
N3ViewerApp::HandleReceivedMessages()
{
	if (!this->deferredMessages.IsEmpty() && this->deferredMessages.Peek()->Handled())
	{
		Ptr<GraphicsEntityMessage> topMessage = this->deferredMessages.Pop();
		if (topMessage->IsA(FetchSkinList::RTTI))
		{
			Ptr<FetchSkinList> fetchSkinsMessage = topMessage.downcast<FetchSkinList>();
			this->skins = fetchSkinsMessage->GetSkins();
			if (this->skins.Size() > 0)
			{
				this->LoadSkin(this->modelEntities[0]->GetResourceId(), this->skins[0]);
			}
		}
		else if (topMessage->IsA(FetchClips::RTTI))
		{
			Ptr<FetchClips> fetchClipsMessage = topMessage.downcast<FetchClips>();
			this->clips = fetchClipsMessage->GetClips();
		}
	}
}


} // namespace Toolkit