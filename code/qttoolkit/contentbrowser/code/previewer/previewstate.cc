//------------------------------------------------------------------------------
//  previewstate.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "previewstate.h"
#include "core/ptr.h"
#include "input/inputserver.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "basegamefeature/managers/factorymanager.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/managers/entitymanager.h"
#include "basegamefeature/basegameprotocol.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "physics/physicsserver.h"
#include "qtaddons/remoteinterface/qtremoteserver.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"
#include "debugrender/debugshaperenderer.h"
#include "physics/model/physicsmodel.h"
#include "physics/resource/managedphysicsmodel.h"
#include "debugrender/debugrender.h"
#include "posteffect/posteffectentity.h"
#include "contentbrowserapp.h"
#include "resources/resourcemanager.h"
#include "messaging/messagecallbackhandler.h"
#include "graphicsfeature/properties/mayacameraproperty.h"
#include "visibility/visibilitysystems/visibilityquadtree.h"
#include "visibility/visibilitysystems/visibilitysystembase.h"
#include "renderutil/nodelookuputil.h"
#include "coregraphics/streamtexturesaver.h"
#include "imgui/imgui.h"
#include "algorithm/algorithmprotocol.h"
#include "models/nodes/shapenode.h"

using namespace Util;
using namespace Graphics;
using namespace GraphicsFeature;
using namespace Physics;
using namespace Math;
using namespace Input;
using namespace PostEffect;
namespace ContentBrowser
{
__ImplementClass(ContentBrowser::PreviewState, 'PRST',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
PreviewState::PreviewState() :
	defaultCam(0),
	showPhysics(false),
	showControls(false),
	workLight(true),
	showWireframe(false),
	showAO(true),
	showSurface(false),
	showSky(true),
	showPassNumber(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PreviewState::~PreviewState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
PreviewState::OnFrame()
{
	this->HandleInput();

	// draw dynui
	Dynui::ImguiAddon::BeginFrame();
	bool lastFrameAO = this->showAO;
	bool lastFrameWorkLight = this->workLight;
	bool lastFrameWireframe = this->showWireframe;
	bool lastShowSurface = this->showSurface;
	bool lastShowSky = this->showSky;
	int lastShowPassNumber = this->showPassNumber;
	const char* textures[] = { "None", "Diffuse", "Specular", "Emissive", "Lighting" };
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport settings", NULL, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::Checkbox("Ambient occlusion", &this->showAO);
		ImGui::SameLine();
		ImGui::Checkbox("Worklight", &this->workLight);
		ImGui::SameLine();
		ImGui::Checkbox("Surface preview", &this->showSurface);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe", &this->showWireframe);
		ImGui::SameLine();
		ImGui::Checkbox("Physics", &this->showPhysics);
		ImGui::SameLine();
		ImGui::Checkbox("Show sky", &this->showSky);
	ImGui::End();
	
	ImGui::Begin("Pass debug", NULL, ImVec2(322, 0), 0.0f, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowPos(ImVec2(10, 40));
		if (ImGui::CollapsingHeader("Passes"))
		{
			ImGui::Combo("Show pass", &this->showPassNumber, textures, 5);

			if (this->showPassNumber > 0)
			{
				if (lastShowPassNumber != this->showPassNumber)
				{
					const char* rendertargets[] = { "", "AlbedoBuffer", "SpecularBuffer", "EmissiveBuffer", "LightBuffer" };
					this->showPass = Resources::ResourceManager::Instance()->LookupResource(rendertargets[this->showPassNumber]).downcast<CoreGraphics::Texture>();
				}
				if (this->showPass.isvalid())
				{
					ImGuiIO& io = ImGui::GetIO();

					// calculate ratio and render
					float ratio = io.DisplaySize.y / io.DisplaySize.x;
					ImGui::Image(this->showPass, ImVec2(320, 320 * ratio), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1,1,1,1), ImVec4(0,0,0,1));
				}
			}
			ImGui::SetWindowSize(ImVec2(322, 0));							// trigger a resize if we have the header open

		}
	ImGui::End();
	ImGui::PopStyleVar();

	ContentBrowserApp::Instance()->GetWindow()->OnFrame();
	Dynui::ImguiAddon::EndFrame();

	if (lastFrameAO != this->showAO)
	{
		Ptr<Algorithm::EnableAmbientOcclusion> msg = Algorithm::EnableAmbientOcclusion::Create();
		msg->SetEnabled(this->showAO);
		GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
	}

	if (lastFrameWireframe != this->showWireframe)
	{
		this->SetShowWireframe(this->showWireframe);
	}

	if (lastShowSurface != this->showSurface)
	{
		this->SetSurfacePreview(this->showSurface);
	}

	if (lastShowSky != this->showSky)
	{
		// set sky entity visibility
		PostEffectManager::Instance()->GetSkyEntity()->SetVisible(this->showSky);
	}

	// get post effect entity
	const Ptr<PostEffectEntity>& peEntity = ContentBrowserApp::Instance()->GetPostEffectEntity();

	// handle light transform depending on light mode
	if (this->workLight)
	{
#ifdef USE_POSTEFFECT_GLOBALLIGHTTRANSFORM
		peEntity->Params().light->SetLightTransform(this->defaultCam->GetMatrix44(Attr::Transform));
		peEntity->SetDirty(true);
#else
		GraphicsFeatureUnit::Instance()->GetGlobalLightEntity()->SetTransform(this->defaultCam->GetMatrix44(Attr::Transform));
#endif
	}
	else
	{
#ifdef USE_POSTEFFECT_GLOBALLIGHTTRANSFORM
		peEntity->Params().light->SetLightTransform(this->lightTransform);
		peEntity->SetDirty(true);
#else
		GraphicsFeatureUnit::Instance()->GetGlobalLightEntity()->SetTransform(this->lightTransform);
#endif
	}

	// update remote interface and client
	QtRemoteInterfaceAddon::QtRemoteServer::Instance()->OnFrame();
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->OnFrame();

	// renders physics
	if (this->showPhysics)
	{
		PhysicsServer::Instance()->GetScene()->RenderDebug();
	}

	// if we show wireframe, render it!
	if (this->showWireframe)
	{
		const Util::Array<Ptr<Models::ModelNode>>& modelNodes = this->modelEntity->GetModelInstance()->GetModel()->GetNodes();
		IndexT i;
		for (i = 0; i < modelNodes.Size(); i++)
		{
			// ugh, we need to find all nodes, get their mesh, and draw them
			const Ptr<Models::ModelNode>& node = modelNodes[i];
			if (node->IsA(Models::ShapeNode::RTTI))
			{
				const Ptr<Models::ShapeNode>& shapeNode = node.downcast<Models::ShapeNode>();
				Math::matrix44 trans = Math::matrix44::transformation(shapeNode->GetScalePivot(), shapeNode->GetRotation(), shapeNode->GetScale(), shapeNode->GetRotatePivot(), shapeNode->GetRotation(), shapeNode->GetPosition());
				Debug::DebugShapeRenderer::Instance()->DrawMesh(trans, shapeNode->GetManagedMesh()->GetMesh(), shapeNode->GetPrimitiveGroupIndex(), float4(0, 0, 0.75f, 0.8f), CoreGraphics::RenderShape::Wireframe);
			}
		}
	}

	return GameStateHandler::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewState::HandleInput()
{
	// get mouse input device
	const Ptr<Mouse>& mouse = InputServer::Instance()->GetDefaultMouse();
	const Ptr<Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();

	if (!this->workLight && mouse->ButtonPressed(MouseButton::LeftButton) && !keyboard->KeyPressed(Key::LeftMenu))
	{
		// get movement
		float2 movement = mouse->GetMovement();

		// create matrix for x-rotation
		matrix44 xrot = matrix44::rotationy(movement.x()/100);

		// create matrix for y-rotation
		matrix44 yrot = matrix44::rotationx(movement.y()/100);

		// combine
		matrix44 trans = matrix44::multiply(xrot, yrot);

		// now apply to light
		this->lightTransform = matrix44::multiply(this->lightTransform, trans);
	}

	if (keyboard->KeyPressed(Key::F))
	{
		bbox boundingBox = this->modelEntity->GetGlobalBoundingBox();
		Ptr<GraphicsFeature::MayaCameraProperty> cameraProperty = this->defaultCam->FindProperty(GraphicsFeature::MayaCameraProperty::RTTI).downcast<GraphicsFeature::MayaCameraProperty>();
		cameraProperty->SetCameraFocus(boundingBox.center(), boundingBox.diagonal_size());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::OnStateEnter(const Util::String& prevState)
{
	GameStateHandler::OnStateEnter(prevState);

	// get the default stage
	this->defaultStage = GraphicsServer::Instance()->GetDefaultView()->GetStage();

	// lookup frameshader used by a frame draw
	Ptr<Frame::FrameShader> frameShader = Frame::FrameServer::Instance()->LookupFrameShader(NEBULA3_DEFAULT_FRAMESHADER_NAME);

	// setup surface view target
	this->surfaceViewTarget = CoreGraphics::RenderTarget::Create();
	this->surfaceViewTarget->SetResolveTextureResourceId("ResourcePreviewTarget");
	this->surfaceViewTarget->SetWidth(150);
	this->surfaceViewTarget->SetHeight(150);
	this->surfaceViewTarget->SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::None);
	this->surfaceViewTarget->SetColorBufferFormat(CoreGraphics::PixelFormat::SRGBA8);
	this->surfaceViewTarget->Setup();

	// setup model stage
	Ptr<Visibility::VisibilityQuadtree> visSystem = Visibility::VisibilityQuadtree::Create();
	visSystem->SetQuadTreeSettings(4, Math::bbox());
	Util::Array<Ptr<Visibility::VisibilitySystemBase> > visSystems;
	visSystems.Append(visSystem.cast<Visibility::VisibilitySystemBase>());
	this->surfaceStage = Graphics::GraphicsServer::Instance()->CreateStage("SurfacePreviewStage", visSystems);
	this->surfaceView = Graphics::GraphicsServer::Instance()->CreateView(Graphics::View::RTTI, "SurfacePreviewView", InvalidIndex, false, false);
	this->surfaceView->SetStage(this->surfaceStage);
	this->surfaceView->SetFrameShader(frameShader);
	Math::rectangle<int> viewport;
	viewport.left = 0;
	viewport.top = 0;
	viewport.bottom = 150;
	viewport.right = 150;
	this->surfaceView->SetResolveRect(viewport);

	// setup camera and attach to stage
	this->surfaceCamera = CameraEntity::Create();
	Graphics::CameraSettings settings;
	settings.SetupPerspectiveFov(n_deg2rad(90.0f), 1, 0.1f, 1000);
	this->surfaceCamera->SetCameraSettings(settings);
	this->surfaceCamera->SetTransform(matrix44::lookatrh(point(1, 1, 1), point(0, 0, 0), vector::upvec()));
	this->surfaceStage->AttachEntity(this->surfaceCamera.cast<GraphicsEntity>());

	// setup light
	this->surfaceLight = GlobalLightEntity::Create();
	this->surfaceLight->SetVolumetric(false);
	this->surfaceLight->SetCastShadows(false);
	this->surfaceLight->SetColor(float4(10));
	this->surfaceStage->AttachEntity(this->surfaceLight.cast<GraphicsEntity>());

	// create surface placeholder model
	this->surfaceModelEntity = ModelEntity::Create();
	this->surfaceModelEntity->SetTransform(matrix44::translation(0, 0, 0));
	this->surfaceModelEntity->SetResourceId("mdl:system/shadingsphere.n3");
	this->surfaceModelEntity->SetLoadSynced(true);
	this->surfaceStage->AttachEntity(this->surfaceModelEntity.cast<GraphicsEntity>());

	// create placeholder model
	this->modelEntity = ModelEntity::Create();
	this->modelEntity->SetTransform(matrix44::translation(0, 0, 0));
	this->modelEntity->SetResourceId("mdl:system/placeholder.n3");
	this->modelEntity->SetLoadSynced(true);
	this->defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

	// ok, first step is to create a surface which we can fiddle with
	IO::IoServer::Instance()->CopyFile("sur:system/placeholder.sur", "sur:system/placeholdersurface.sur");

	// load placeholder surface
	this->placeholderSurface = Resources::ResourceManager::Instance()->CreateManagedResource(Materials::Surface::RTTI, "sur:system/placeholdersurface.sur", NULL, true).downcast<Materials::ManagedSurface>();
	this->SetSurface(this->placeholderSurface->GetSurface()->CreateInstance());

	// create camera
	this->defaultCam = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Camera", "Camera");
	this->defaultCam->SetFloat4(Attr::MayaCameraCenterOfInterest, Math::float4(0, 0, 0, 1));
	Math::matrix44 camTrans;
	camTrans.set_position(Math::float4(10, 10, 10, 1));
	this->defaultCam->SetMatrix44(Attr::Transform, camTrans);
	this->defaultCam->SetFloat4(Attr::MayaCameraCenterOfInterest, Math::float4(0, 0, 0, 1));
	BaseGameFeature::EntityManager::Instance()->AttachEntity(defaultCam);

	// disable gravity
	Physics::PhysicsServer::Instance()->GetScene()->SetGravity(Math::vector(0,0,0));

	// make sure ImGui doesn't steal input focus
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse = false;
	io.WantCaptureKeyboard = false;
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::OnStateLeave(const Util::String& nextState)
{
	// cleanup scene before quitting application
	this->defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
	this->modelEntity = 0;

	this->defaultStage = 0;
	this->defaultCam = 0;
	this->physicsModel = 0;
	this->physicsObjects.Clear();

	this->surfaceViewTarget->Discard();
	this->surfaceViewTarget = 0;
	this->surfaceStage->RemoveAllEntities();
	this->surfaceModelEntity = 0;
	this->surfaceLight = 0;
	this->surfaceCamera = 0;
	this->surfaceLight = 0;
	this->surfaceStage = 0;
	this->surfaceView = 0;

	// discard placeholder surface
	Resources::ResourceManager::Instance()->DiscardManagedResource(this->placeholderSurface.upcast<Resources::ManagedResource>());

	// remove temporary surface file
	IO::IoServer::Instance()->DeleteFile("sur:system/placeholdersurface.sur");

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
bool
PreviewState::SetModel(const Resources::ResourceId& resource)
{
	// create placeholder model
	this->defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
	this->modelEntity->SetTransform(matrix44::translation(0.0, 0.0, 0.0));
	this->modelEntity->SetResourceId(resource);
	this->defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

    // setup anim event tracking
    this->modelEntity->ConfigureAnimEventTracking(true, false);

	// fetch skins
	Ptr<FetchSkinList> fetchSkinsMessage = FetchSkinList::Create();
    __Send(this->modelEntity, fetchSkinsMessage);
	this->OnFetchedSkinList(fetchSkinsMessage.upcast<Messaging::Message>());

	return this->modelEntity->IsValid();
}

//------------------------------------------------------------------------------
/**
*/
bool
PreviewState::SetPhysics(const Resources::ResourceId& resource)
{
	for (int i = 0; i < physicsObjects.Size(); i++)
	{
		PhysicsServer::Instance()->GetScene()->Detach(this->physicsObjects[i]);
	}
	this->physicsObjects.Clear();
	if (this->physicsModel.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->physicsModel.cast<Resources::ManagedResource>());
	}
	this->physicsModel = 0;

	// create physics resource
	this->physicsModel = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI, resource).cast<ManagedPhysicsModel>();

	// if succeeded, create instance of model
	if (this->physicsModel.isvalid())
	{
		this->physicsObjects = physicsModel->GetModel()->CreateStaticInstance(matrix44::identity());
		for (int i = 0; i < physicsObjects.Size(); i++)
		{
			PhysicsServer::Instance()->GetScene()->Attach(physicsObjects[i]);
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SetSurface(const Ptr<Materials::SurfaceInstance>& instance)
{
	// thankfully, this model uses a hierarchy we know already
	Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(this->surfaceModelEntity, "pSphere3");
	node->SetSurfaceInstance(instance);
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::DiscardSurface()
{
	// thankfully, this model uses a hierarchy we know already
	Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(this->surfaceModelEntity, "pSphere3");
	node->SetSurfaceInstance(this->placeholderSurface->GetSurface()->CreateInstance());
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SetSurfacePreview(bool b)
{
	Ptr<Graphics::View> view = GraphicsServer::Instance()->GetDefaultView();
	if (b)
	{
		view->SetStage(this->surfaceStage);
	}
	else
	{
		view->SetStage(this->defaultStage);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SaveThumbnail(const Util::String& path, bool swapStage)
{
	// hmm, dont want UI to draw to ImGui
	ImGui::NewFrame();

	// render a single frame
	if (swapStage)
	{
		CoreGraphics::DisplayMode mode = CoreGraphics::DisplayDevice::Instance()->GetWindow(0)->GetDisplayMode();
		Graphics::CameraSettings settings;
		settings.SetupPerspectiveFov(n_deg2rad(60.0f), 1, 0.1f, 1000);

		this->surfaceCamera->SetTransform(this->defaultCam->GetMatrix44(Attr::Transform));
		this->surfaceView->SetStage(this->defaultStage);
		this->surfaceCamera->SetCameraSettings(settings);
	}

	// get frame shader so we can change the frame shader result
	const Ptr<Frame::FrameShader>& frameShader = this->surfaceView->GetFrameShader();
	Ptr<Frame::FramePassBase> lastPass = frameShader->GetAllFramePassBases().Back();
	Ptr<CoreGraphics::RenderTarget> oldRenderTarget = lastPass->GetRenderTarget();

	// render and reset old render target
	lastPass->SetRenderTarget(this->surfaceViewTarget);
	this->surfaceView->SetCameraEntity(this->surfaceCamera);
	this->surfaceView->OnFrame(NULL, 0, 0, false);
	this->surfaceView->SetCameraEntity(NULL);
	lastPass->SetRenderTarget(oldRenderTarget);
	if (swapStage)
	{
		Graphics::CameraSettings settings;
		settings.SetupPerspectiveFov(n_deg2rad(90.0f), 1, 0.1f, 1000);
		
		this->surfaceView->SetStage(this->surfaceStage);
		this->surfaceCamera->SetTransform(matrix44::lookatrh(point(1, 1, 1), point(0, 0, 0), vector::upvec()));
		this->surfaceCamera->SetCameraSettings(settings);
	}
	Ptr<CoreGraphics::Texture> tex = this->surfaceViewTarget->GetResolveTexture();
	Ptr<CoreGraphics::StreamTextureSaver> saver = CoreGraphics::StreamTextureSaver::Create();
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
	saver->SetFormat(CoreGraphics::ImageFileFormat::PNG);
	saver->SetMipLevel(0);
	saver->SetStream(stream);
	tex->SetSaver(saver.upcast<Resources::ResourceSaver>());
	n_assert(tex->Save());
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::OnFetchedSkinList(const Ptr<Messaging::Message>& msg)
{
	Ptr<Graphics::FetchSkinList> rMsg = msg.downcast<Graphics::FetchSkinList>();

	// if we have a skin, apply the first
	const Array<StringAtom>& skins = rMsg->GetSkins();
	IndexT i;
	for (i = 0; i < skins.Size(); i++)
	{
		Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
		showSkin->SetSkin(skins[i]);
		this->modelEntity->HandleMessage(showSkin.cast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SetShowWireframe(bool enable)
{
	this->modelEntity->SetVisible(!enable);
}

} // namespace ContentBrowser