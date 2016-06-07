//------------------------------------------------------------------------------
//  lightprobe.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "environmentprobecapturer.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "frame/frameserver.h"
#include "frame/frameposteffect.h"
#include "graphics/graphicsinterface.h"
#include "coregraphics/streamtexturesaver.h"
#include "io/ioserver.h"
#include "lightprobemanager.h"
#include "basegamefeatureunit.h"
#include "texutil/cubefilterer.h"
#include <QPlainTextEdit>
#include "level.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

using namespace IO;
using namespace Math;
using namespace Graphics;
using namespace CoreGraphics;
using namespace ToolkitUtil;
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::EnvironmentProbeCapturer, 'LGPR', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
EnvironmentProbeCapturer::EnvironmentProbeCapturer() :
	resolutionX(256),
	resolutionY(256),
	generateMipmaps(true),
	calculateIrradiance(true),
	calculateReflections(true),
	captureDepth(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentProbeCapturer::~EnvironmentProbeCapturer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::Discard()
{
	n_assert(this->entity.isvalid());
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::SetVisible(bool b)
{
	n_assert(this->entity.isvalid());
	Ptr<GraphicsFeature::SetGraphicsVisible> msg = GraphicsFeature::SetGraphicsVisible::Create();
	msg->SetVisible(b);
	__SendSync(this->entity, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::AddHideLink(const Ptr<Graphics::GraphicsEntity>& entity)
{
	n_assert(this->hideLinks.FindIndex(entity) == InvalidIndex);
	this->hideLinks.Append(entity);
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::RemoveHideLink(const Ptr<Graphics::GraphicsEntity>& entity)
{
	IndexT i = this->hideLinks.FindIndex(entity);
	n_assert(i != InvalidIndex);
	this->hideLinks.EraseIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
void
ProgressBarStatusHandler(const Util::String& message, void* messageHandler)
{
	QPlainTextEdit* progress = (QPlainTextEdit*)messageHandler;
	progress->setPlainText(message.AsCharPtr());
	QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
	This function is rather heavy, basically, it sets up a view, a camera, a frame shader, a render target cube.
	Attaches camera to stage. Then for each cube face, rotates the camera 
*/
void
EnvironmentProbeCapturer::Render(QPlainTextEdit* progressBar)
{
	// ignore updates if we're not going to calculate anything anyways
	if (!this->calculateIrradiance && !this->calculateReflections) return;

	// get view
	Ptr<View> view = LightProbeManager::Instance()->GetReflectionView();
	Ptr<Stage> stage = LightProbeManager::Instance()->GetReflectionView()->GetStage();

	// begin probe update
	LightProbeManager::Instance()->BeginProbeUpdate();

	// create the camera which we are going to use
	CameraSettings settings;
	Ptr<CameraEntity> cam = view->GetCameraEntity();
	
	// create frame shader which uses a simplified path compared to the standard one, then resize to fit our resolution
	Ptr<Frame::FrameShader> frameShader = LightProbeManager::Instance()->GetReflectionFrameShader();
	frameShader->OnWindowResized(this->resolutionX, this->resolutionY);

	// create render target cube
	Util::FixedArray<Ptr<RenderTarget>> colorFaceTargets;
	Util::FixedArray<Ptr<Texture>> colorFaceTextures;
	colorFaceTargets.Resize(6);
	colorFaceTextures.Resize(6);
	IndexT i;
	for (i = 0; i < 6; i++)
	{
		colorFaceTargets[i] = RenderTarget::Create();
		colorFaceTargets[i]->SetColorBufferFormat(CoreGraphics::PixelFormat::R16G16B16A16F);
		colorFaceTargets[i]->SetResolveTextureResourceId(Util::String::Sprintf("LightProbeReflection%d", i));
		colorFaceTargets[i]->SetWidth(this->resolutionX);
		colorFaceTargets[i]->SetHeight(this->resolutionY);
		colorFaceTargets[i]->Setup();
		colorFaceTextures[i] = colorFaceTargets[i]->GetResolveTexture();
	}
	
	Util::FixedArray<Ptr<RenderTarget>> depthFaceTargets;
	Util::FixedArray<Ptr<Texture>> depthFaceTextures;
	depthFaceTargets.Resize(6);
	depthFaceTextures.Resize(6);
	for (i = 0; i < 6; i++)
	{
		depthFaceTargets[i] = RenderTarget::Create();
		depthFaceTargets[i]->SetColorBufferFormat(CoreGraphics::PixelFormat::R32F);
		depthFaceTargets[i]->SetResolveTextureResourceId(Util::String::Sprintf("LightProbeDepthFace%d", i));
		depthFaceTargets[i]->SetWidth(this->resolutionX);
		depthFaceTargets[i]->SetHeight(this->resolutionY);
		depthFaceTargets[i]->Setup();
		depthFaceTextures[i] = depthFaceTargets[i]->GetResolveTexture();
	}

	// setup the view
	view->SetFrameShader(frameShader);

	// get last pass and set render target, remove previous render targets
	const Ptr<Frame::FramePassBase>& pass = frameShader->GetAllFramePassBases().Back();
	Ptr<Frame::FramePostEffect> lastPass = pass.downcast<Frame::FramePostEffect>();
	Ptr<RenderTarget> originalRt = lastPass->GetRenderTarget();

	// create base transform for camera
	matrix44 cameraTransform = matrix44::translation(this->position);

	// create list of visibilities for the models we are going to hide, then hide the linked objects
	Util::Array<bool> hideList;
	for (i = 0; i < this->hideLinks.Size(); i++)
	{
		hideList.Append(this->hideLinks[i]->IsVisible());
		this->hideLinks[i]->SetVisible(false);
	}

	// calculate transform matrices, order is:
	//	+X,		-X,		+Y,		-Y,		+Z,		-Z
	//	Right,	Left,	Up,		Down,	Back,	Front
	//
	//  Also rotate +X, -X and +Z 180 around Z because GL will read them upside down
	//
	matrix44 views[6];
	views[0] = matrix44::multiply(matrix44::rotationy(n_deg2rad(90)), matrix44::rotationz(n_deg2rad(180)));
	views[1] = matrix44::multiply(matrix44::rotationy(n_deg2rad(-90)), matrix44::rotationz(n_deg2rad(180)));
	views[2] = matrix44::rotationx(n_deg2rad(90));
	views[3] = matrix44::rotationx(n_deg2rad(-90));
	views[4] = matrix44::multiply(matrix44::rotationy(n_deg2rad(180)), matrix44::rotationz(n_deg2rad(180)));
	views[5] = matrix44::rotationz(n_deg2rad(180));

	IndexT frameIndex;
	for (frameIndex = 0; frameIndex < 6; frameIndex++)
	{
		// setup camera settings
		settings.SetupPerspectiveFov(n_deg2rad(90.0f), this->resolutionX / this->resolutionY, 0.1f, 1000);
		cam->SetCameraSettings(settings);

		// set rotation based on view
		matrix44 rotation = views[frameIndex];
		rotation.set_position(this->position);
		cam->SetTransform(rotation);

		// now render like we normally would
		lastPass->SetRenderTarget(colorFaceTargets[frameIndex]);
		view->OnFrame(NULL, 0, 0, false);
		//GraphicsInterface::Instance()->OnFrame();

		if (captureDepth)
		{
			const Ptr<CoreGraphics::RenderTarget>& depthBuffer = frameShader->GetRenderTargetByName("ZDepth");
			const Ptr<CoreGraphics::RenderTarget>& depthFace = depthFaceTargets[frameIndex];
			depthBuffer->Copy(depthFace);
		}
	}

	// reset frame shader
	lastPass->SetRenderTargetCube(NULL);
	lastPass->SetRenderTarget(originalRt);

	// show entities which were previously visible again
	for (i = 0; i < this->hideLinks.Size(); i++)
	{
		this->hideLinks[i]->SetVisible(hideList[i]);
	}

	// create assigns for work and export
	Util::String levelName = Level::Instance()->GetName().AsCharPtr();
	Util::String workFile;
	Util::String exportFile;
	Util::String workFolder = "work:assets/" + levelName;
	Util::String exportFolder = "tex:" + levelName;	

	// format file name, we could use name and type here too, but it feels wrong to do so
	workFolder.SubstituteString("#LEVEL#", levelName.AsCharPtr());
	exportFolder.SubstituteString("#LEVEL#", levelName.AsCharPtr());

	// create output dirs
	IoServer::Instance()->CreateDirectory(workFolder);
	IoServer::Instance()->CreateDirectory(exportFolder);

	// create cube filterer
	Ptr<CubeFilterer> filterer = CubeFilterer::Create();
	filterer->SetCubeFaces(colorFaceTextures);
	filterer->SetOutputSize(this->resolutionX);
	filterer->SetSpecularPower(2048);

	// if we do irradiance too, then lets do it!
	if (this->calculateIrradiance)
	{
		this->irradianceMapName = this->outputName;
		this->irradianceMapName.SubstituteString("#NAME#", this->name.AsCharPtr());
		this->irradianceMapName.SubstituteString("#LEVEL#", levelName.AsCharPtr());
		this->irradianceMapName.SubstituteString("#TYPE#", "irradiance");
		workFile.Format("%s/%s.dds", workFolder.AsCharPtr(), this->irradianceMapName.AsCharPtr());
		exportFile.Format("%s/%s.dds", exportFolder.AsCharPtr(), this->irradianceMapName.AsCharPtr());

		filterer->SetOutputFile(workFile);
		filterer->SetGenerateMips(false);
		filterer->Filter(true, progressBar, ProgressBarStatusHandler);

		// copy from work to export
		IoServer::Instance()->CopyFile(workFile, exportFile);
	}

	if (this->calculateReflections)
	{
		this->reflectionMapName = this->outputName;
		this->reflectionMapName.SubstituteString("#NAME#", this->name.AsCharPtr());
		this->reflectionMapName.SubstituteString("#LEVEL#", levelName.AsCharPtr());
		this->reflectionMapName.SubstituteString("#TYPE#", "reflection");
		workFile.Format("%s/%s.dds", workFolder.AsCharPtr(), this->reflectionMapName.AsCharPtr());
		exportFile.Format("%s/%s.dds", exportFolder.AsCharPtr(), this->reflectionMapName.AsCharPtr());

		filterer->SetOutputFile(workFile);
		filterer->SetGenerateMips(this->generateMipmaps);
		filterer->Filter(false, progressBar, ProgressBarStatusHandler);

		// copy from work to export
		IoServer::Instance()->CopyFile(workFile, exportFile);
	}

	if (this->captureDepth)
	{
		this->depthMapName = this->outputName;
		this->depthMapName.SubstituteString("#NAME#", this->name.AsCharPtr());
		this->depthMapName.SubstituteString("#LEVEL#", levelName.AsCharPtr());
		this->depthMapName.SubstituteString("#TYPE#", "depth");
		workFile.Format("%s/%s.dds", workFolder.AsCharPtr(), this->depthMapName.AsCharPtr());
		exportFile.Format("%s/%s.dds", exportFolder.AsCharPtr(), this->depthMapName.AsCharPtr());

		filterer->SetCubeFaces(depthFaceTextures);
		filterer->SetOutputFile(workFile);
		filterer->DepthCube(false, progressBar, ProgressBarStatusHandler);

		// copy from work to export
		IoServer::Instance()->CopyFile(workFile, exportFile);
	}

	for (i = 0; i < 6; i++)
	{
		colorFaceTextures[i] = 0;
		colorFaceTargets[i]->Discard();
		colorFaceTargets[i] = 0;

		depthFaceTextures[i] = 0;
		depthFaceTargets[i]->Discard();
		depthFaceTargets[i] = 0;
	}

	// end probe update
	LightProbeManager::Instance()->EndProbeUpdate();

	// finalize by updating the entity if it wants to be 
	if (this->entity->GetBool(Attr::ProbeAutoAssignMaps))
	{
		// remove tex:
		exportFolder.StripAssignPrefix();

		if (this->calculateReflections)
		{
			// update reflections
			Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
			msg->SetAttr(Attr::Attribute(Attr::ProbeReflectionMap, exportFolder + "/" + this->reflectionMapName));
			__SendSync(this->entity, msg);

			// reload the reflection map if it exists
			Ptr<ReloadResourceIfExists> reload = ReloadResourceIfExists::Create();
			reload->SetResourceName("tex:" + exportFolder + "/" + this->reflectionMapName + ".dds");
			__StaticSend(GraphicsInterface, reload);
		}		

		if (this->calculateIrradiance)
		{
			// update irradiance
			Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
			msg->SetAttr(Attr::Attribute(Attr::ProbeIrradianceMap, exportFolder + "/" + this->irradianceMapName));
			__SendSync(this->entity, msg);

			// reload irradiance
			Ptr<ReloadResourceIfExists> reload = ReloadResourceIfExists::Create();
			reload->SetResourceName("tex:" + exportFolder + "/" + this->irradianceMapName + ".dds");
			__StaticSend(GraphicsInterface, reload);
		}		

		if (this->captureDepth)
		{
			// update depth
			Ptr<BaseGameFeature::SetAttribute> msg = BaseGameFeature::SetAttribute::Create();
			msg->SetAttr(Attr::Attribute(Attr::ProbeDepthMap, exportFolder + "/" + this->depthMapName));
			__SendSync(this->entity, msg);

			// reload irradiance
			Ptr<ReloadResourceIfExists> reload = ReloadResourceIfExists::Create();
			reload->SetResourceName("tex:" + exportFolder + "/" + this->depthMapName + ".dds");
			__StaticSend(GraphicsInterface, reload);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::BeforeRender()
{
	Ptr<LevelEditor2::BeginProbeBuild> msg = LevelEditor2::BeginProbeBuild::Create();
	__SendSync(this->entity, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentProbeCapturer::AfterRender()
{
	Ptr<LevelEditor2::EndProbeBuild> msg = LevelEditor2::EndProbeBuild::Create();
	__SendSync(this->entity, msg);
}

} // namespace LevelEditor2