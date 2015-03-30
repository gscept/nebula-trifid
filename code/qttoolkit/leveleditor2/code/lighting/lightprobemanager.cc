//------------------------------------------------------------------------------
//  lightprobemanager.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lightprobemanager.h"
#include "leveleditor2entitymanager.h"
#include "frame/frameserver.h"
#include "graphics/graphicsserver.h"

using namespace Graphics;
namespace LevelEditor2
{
__ImplementSingleton(LevelEditor2::LightProbeManager);
__ImplementClass(LevelEditor2::LightProbeManager, 'LPMN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
LightProbeManager::LightProbeManager() :
	lightProbeFrameShader(0),
	isOpen(false),
	inBeginUpdate(false)
{
	__ConstructSingleton;
	this->ui.setupUi(&this->dialog);

	// hide progress, only show when we are building
	this->ui.lightBuildProgress->setVisible(false);
	this->ui.lightBuildStatusField->setVisible(false);

	// set default dir
	this->outputFolder = "work:textures/test";
	this->naming = "#LEVEL#-#NAME#_#TYPE#";

	// bind buttons with slots
	connect(this->ui.buildLights, SIGNAL(clicked()), this, SLOT(Build()));
	connect(this->ui.createProbe, SIGNAL(clicked()), this, SLOT(CreateLightProbe()));
}

//------------------------------------------------------------------------------
/**
*/
LightProbeManager::~LightProbeManager()
{
	__DestructSingleton;	
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::Open()
{
	n_assert(!this->isOpen);

	// create frame shader
	this->lightProbeFrameShader = Frame::FrameServer::Instance()->LookupFrameShader("reflection");
	
	this->reflectionCamera = CameraEntity::Create();
	Ptr<Stage> stage = GraphicsServer::Instance()->GetStageByName("DefaultStage");
	stage->AttachEntity(this->reflectionCamera.cast<GraphicsEntity>());

	this->reflectionView = GraphicsServer::Instance()->CreateView(Graphics::View::RTTI, "Reflection");
	this->reflectionView->SetStage(stage);
	this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::Close()
{
	n_assert(this->isOpen);

	// destroy frame shader
	this->lightProbeFrameShader->Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::Show()
{
	this->dialog.show();
	this->dialog.raise();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::Hide()
{
	this->dialog.hide();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::BeginProbeUpdate()
{
	n_assert(this->reflectionView.isvalid());
	n_assert(this->reflectionCamera.isvalid());
	n_assert(!this->inBeginUpdate);
	this->reflectionView->SetCameraEntity(this->reflectionCamera);
	this->inBeginUpdate = true;

	// hide all probes
	IndexT i;
	for (i = 0; i < this->lightProbes.Size(); i++)
	{
		const Ptr<EnvironmentProbeCapturer>& probe = this->lightProbes[i];
		probe->SetVisible(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::EndProbeUpdate()
{
	n_assert(this->inBeginUpdate);
	this->reflectionView->SetCameraEntity(0);
	this->inBeginUpdate = false;

	// show all probes again
	IndexT i;
	for (i = 0; i < this->lightProbes.Size(); i++)
	{
		const Ptr<EnvironmentProbeCapturer>& probe = this->lightProbes[i];
		probe->SetVisible(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::RegisterProbe(const Ptr<EnvironmentProbeCapturer>& probe)
{
	n_assert(this->lightProbes.FindIndex(probe) == InvalidIndex);
	this->lightProbes.Append(probe);
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::UnregisterProbe(const Ptr<EnvironmentProbeCapturer>& probe)
{
	IndexT i = this->lightProbes.FindIndex(probe);
	n_assert(i != InvalidIndex);
	this->lightProbes.EraseIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::CreateLightProbe()
{
	Util::String errorMessage;
	EntityGuid entityId;
	if (!ActionManager::Instance()->CreateEntity(Probe, "LightProbe", "LightProbe", entityId, errorMessage))
	{
		n_error("Could not create LightProbe. This should never happen. Message is: %s", errorMessage.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeManager::Build()
{
	IndexT i;

	this->ui.lightBuildProgress->setVisible(true);
	QApplication::processEvents();

	this->ui.lightBuildProgress->setMaximum(this->lightProbes.Size());
	this->ui.lightBuildProgress->setValue(0);

	this->ui.lightBuildStatusField->setVisible(true);
	for (i = 0; i < this->lightProbes.Size(); i++)
	{
		const Ptr<EnvironmentProbeCapturer>& probe = this->lightProbes[i];

		Util::String status;
		status.Format("Building light probe '%s', ", probe->GetName().AsCharPtr());
		status += "%p%";
		this->ui.lightBuildProgress->setFormat(status.AsCharPtr());
		QApplication::processEvents();

		// render probe
		probe->Render(this->ui.lightBuildStatusField); 

		// update progress
		this->ui.lightBuildProgress->setValue(this->ui.lightBuildProgress->value() + 1);
		QApplication::processEvents();
	}

	// this will show the status as a 100% before closing
	this->ui.lightBuildProgress->setValue(this->ui.lightBuildProgress->value() + 1);
	QApplication::processEvents();

	this->ui.lightBuildStatusField->setVisible(false);
	this->ui.lightBuildProgress->setVisible(false);
}

} // namespace LevelEditor2