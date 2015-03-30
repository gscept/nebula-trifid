//------------------------------------------------------------------------------
//  lightprobe.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
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
#include "leveleditor2protocol.h"
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
	calculateReflections(true)
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
	frameShader->OnDisplayResized(this->resolutionX, this->resolutionY);

	// create render target cube
	this->probeMap = RenderTargetCube::Create();
	this->probeMap->SetColorBufferFormat(CoreGraphics::PixelFormat::A16B16G16R16F);
	this->probeMap->SetResolveTextureResourceId("LightProbeReflection");
	this->probeMap->SetWidth(this->resolutionX);
	this->probeMap->SetHeight(this->resolutionY);
	this->probeMap->Setup();

	// setup the view
	view->SetFrameShader(frameShader);

	// get last pass and set render target, remove previous render targets
	const Ptr<Frame::FramePassBase>& pass = frameShader->GetAllFramePassBases().Back();
	Ptr<Frame::FramePostEffect> lastPass = pass.downcast<Frame::FramePostEffect>();
	Ptr<RenderTarget> originalRt = lastPass->GetRenderTarget();

	// save reference to original object
	lastPass->SetRenderTargetCube(this->probeMap);
	lastPass->SetRenderTarget(NULL);

	// create base transform for camera
	matrix44 cameraTransform = matrix44::translation(this->position);

	// create list of visibilities for the models we are going to hide, then hide the linked objects
	Util::Array<bool> hideList;
	IndexT i;
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
	views[3] = matrix44::rotationx(n_deg2rad(-90)), matrix44::rotationz(n_deg2rad(180));
	views[4] = matrix44::multiply(matrix44::rotationy(n_deg2rad(180)), matrix44::rotationz(n_deg2rad(180)));
	views[5] = matrix44::rotationz(n_deg2rad(180));
	
	// calculate width, height and depth of capture zone
	float4 diagonalDiff = this->captureZone.pmax - this->captureZone.pmin;
	float sizes[6];
	sizes[0] = sizes[1] = diagonalDiff.x() / 2;
	sizes[2] = sizes[3] = diagonalDiff.y() / 2;
	sizes[4] = sizes[5] = diagonalDiff.z() / 2;

	IndexT frameIndex;
	for (frameIndex = 0; frameIndex < 6; frameIndex++)
	{
		// set cube face to render to
		this->probeMap->SetDrawFace(Texture::CubeFace(Texture::PosX + frameIndex));

		settings.SetupPerspectiveFov(n_deg2rad(90.0f), this->resolutionX / this->resolutionY, 0.1f, 1000);
		cam->SetCameraSettings(settings);

		// set rotation based on view
		matrix44 rotation = views[frameIndex];
		rotation.set_position(this->position);
		cam->SetTransform(rotation);

		// now render like we normally would
		GraphicsInterface::Instance()->OnFrame();
	}

	// reset frame shader
	lastPass->SetRenderTargetCube(NULL);
	lastPass->SetRenderTarget(originalRt);

	// show entities which were previously visible again
	for (i = 0; i < this->hideLinks.Size(); i++)
	{
		this->hideLinks[i]->SetVisible(hideList[i]);
	}

	// get texture
	const Ptr<Texture>& tex = this->probeMap->GetResolveTexture();

	// create assigns for work and export
	Util::String workFile;
	Util::String exportFile;
	Util::String workFolder = "work:textures/" + this->outputFolder;
	Util::String exportFolder = "tex:" + this->outputFolder;
	Util::String levelName = Level::Instance()->GetName().AsCharPtr();

	// format file name, we could use name and type here too, but it feels wrong to do so
	workFolder.SubstituteString("#LEVEL#", levelName.AsCharPtr());
	exportFolder.SubstituteString("#LEVEL#", levelName.AsCharPtr());

	// create output dirs
	IoServer::Instance()->CreateDirectory(workFolder);
	IoServer::Instance()->CreateDirectory(exportFolder);

	// create cube filterer
	Ptr<CubeFilterer> filterer = CubeFilterer::Create();
	filterer->SetCubemap(tex);
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
		workFile.Format("%s/%s.dds", workFolder.AsCharPtr(), name.AsCharPtr());
		exportFile.Format("%s/%s.dds", exportFolder.AsCharPtr(), this->reflectionMapName.AsCharPtr());

		filterer->SetOutputFile(workFile);
		filterer->SetGenerateMips(this->generateMipmaps);
		filterer->Filter(false, progressBar, ProgressBarStatusHandler);

		// copy from work to export
		IoServer::Instance()->CopyFile(workFile, exportFile);
	}

	// cleanup the mess
	this->probeMap->Discard();
	this->probeMap = 0;

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
	}
}

} // namespace LevelEditor2