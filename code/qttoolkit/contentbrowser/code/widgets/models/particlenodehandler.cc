//------------------------------------------------------------------------------
//  particlenodehandler.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QFileDialog>
#include "particlenodehandler.h"
#include "modelutil/modelattributes.h"
#include "modelutil/modelconstants.h"
#include "io/ioserver.h"
#include "math/float4.h"
#include "math/float2.h"
#include "materials/materialdatabase.h"
#include "resources/managedtexture.h"
#include "resources/resourcemanager.h"
#include "particles/particleserver.h"
#include "previewer/previewstate.h"
#include "graphics/modelentity.h"
#include "models/nodes/statenodeinstance.h"
#include "contentbrowserapp.h"
#include "renderutil/nodelookuputil.h"
#include "mutableparticlesysteminstance.h"
#include "particles/particlesystemnodeinstance.h"

using namespace Particles;
using namespace ToolkitUtil;
using namespace IO;
using namespace ContentBrowser;
using namespace Graphics;
using namespace Math;
using namespace Materials;
using namespace Models;
using namespace Util;
namespace Widgets
{
__ImplementClass(Widgets::ParticleNodeHandler, 'PANH', ModelNodeHandler);

//------------------------------------------------------------------------------
/**
*/
ParticleNodeHandler::ParticleNodeHandler() :
	isUpdatingValues(false),
	emitterMesh(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ParticleNodeHandler::~ParticleNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::Setup(const Util::String& resource)
{
	// put shading UI into shading tab
	this->ui = new Ui::ModelNodeInfoWidget;
	this->ui->setupUi(this->particleUi->shadingFrame);
	ModelNodeHandler::Setup(resource);

	// copy attrs
	this->attrs = attrs;

	// copy resource
	this->resource = resource;

	// stop updating stuff
	this->isUpdatingValues = true;

	// get attribute
	Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();
	Ptr<ModelConstants> constants = this->modelHandler->GetConstants();

	// set primitive group index
	this->primGroupIndex = attributes->GetAppendixNode(this->nodeName).data.particle.primGroup;

	String mesh = attributes->GetEmitterMesh(this->nodePath);
	this->attrs = attributes->GetEmitterAttrs(this->nodePath);
	if (mesh.IsEmpty())
	{
		this->particleUi->emitterMesh->setText("single point emitter mesh");
		this->particleUi->singlePointEmitter->setChecked(true);
		this->particleUi->primitiveGroupIndex->setEnabled(false);
		this->emitterMesh = ParticleServer::Instance()->GetDefaultEmitterMesh();
	}
	else
	{
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, mesh, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->particleUi->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
	}
	
	// fill emitter mesh
	this->particleUi->emitterMesh->setText(attributes->GetEmitterMesh(this->nodePath).AsCharPtr());

	// connect browser button
	connect(this->particleUi->browseEmitterMesh, SIGNAL(pressed()), this, SLOT(BrowseMesh()));
	connect(this->particleUi->restartParticle, SIGNAL(pressed()), this, SLOT(Restart()));

    // get state node
    Ptr<PreviewState> previewState = ContentBrowserApp::Instance()->GetPreviewState();
    const Ptr<ModelEntity>& model = previewState->GetModel();
    Ptr<StateNodeInstance> stateNode = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(model, this->nodePath);
	
	// get particle system node instance from state node
	this->particleStateNode = stateNode.downcast<ParticleSystemNodeInstance>();

	// setup envelope frames
	this->particleUi->colorRedFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Red), EmitterAttrs::Red);
	this->particleUi->colorGreenFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Green), EmitterAttrs::Green);
	this->particleUi->colorBlueFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Blue), EmitterAttrs::Blue);
	this->particleUi->colorAlphaFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Alpha), EmitterAttrs::Alpha);
	this->particleUi->frequencyFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::EmissionFrequency), EmitterAttrs::EmissionFrequency);
	this->particleUi->lifetimeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::LifeTime), EmitterAttrs::LifeTime);
	this->particleUi->initVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::StartVelocity), EmitterAttrs::StartVelocity);
	this->particleUi->rotationVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::RotationVelocity), EmitterAttrs::RotationVelocity);
	this->particleUi->sizeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Size), EmitterAttrs::Size);
	this->particleUi->spreadMinFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMin), EmitterAttrs::SpreadMin);
	this->particleUi->spreadMaxFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMax), EmitterAttrs::SpreadMax);
	this->particleUi->airResistanceFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::AirResistance), EmitterAttrs::AirResistance);
	this->particleUi->velocityFactorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::VelocityFactor), EmitterAttrs::VelocityFactor);
	this->particleUi->massFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Mass), EmitterAttrs::Mass);
	this->particleUi->timeManipulatorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::TimeManipulator), EmitterAttrs::TimeManipulator);

	// connect signals from frames
	connect(this->particleUi->colorRedFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->colorGreenFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->colorBlueFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->colorAlphaFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->frequencyFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->lifetimeFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->initVelocityFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->rotationVelocityFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->sizeFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->spreadMinFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->spreadMaxFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->airResistanceFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->velocityFactorFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->massFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->particleUi->timeManipulatorFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));

	// setup attributes
	this->particleUi->textureTiles->setValue(this->attrs.GetFloat(EmitterAttrs::TextureTile));
	this->particleUi->phasesPerSecond->setValue(this->attrs.GetFloat(EmitterAttrs::PhasesPerSecond));
	this->particleUi->emissionDuration->setValue(this->attrs.GetFloat(EmitterAttrs::EmissionDuration));
	this->particleUi->initRotationAngleMin->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMin));
	this->particleUi->initRotationAngleMax->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMax));
	this->particleUi->sizeRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::SizeRandomize));
	this->particleUi->startDelay->setValue(this->attrs.GetFloat(EmitterAttrs::StartDelay));
	this->particleUi->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->particleUi->initVectorRand->setValue(this->attrs.GetFloat(EmitterAttrs::VelocityRandomize));
	this->particleUi->rotationRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::RotationRandomize));
	this->particleUi->stretch->setValue(this->attrs.GetFloat(EmitterAttrs::ParticleStretch));
	this->particleUi->activityDistance->setValue(this->attrs.GetFloat(EmitterAttrs::ActivityDistance));
	this->particleUi->gravity->setValue(this->attrs.GetFloat(EmitterAttrs::Gravity));
	this->particleUi->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->particleUi->looping->setChecked(this->attrs.GetBool(EmitterAttrs::Looping));
	this->particleUi->randomizeRotation->setChecked(this->attrs.GetBool(EmitterAttrs::RandomizeRotation));
	this->particleUi->stretchToStart->setChecked(this->attrs.GetBool(EmitterAttrs::StretchToStart));
	this->particleUi->fadeOut->setChecked(this->attrs.GetBool(EmitterAttrs::ViewAngleFade));
	this->particleUi->viewAligned->setChecked(this->attrs.GetBool(EmitterAttrs::Billboard));
	this->particleUi->oldestFirst->setChecked(this->attrs.GetBool(EmitterAttrs::RenderOldestFirst));
	this->particleUi->stretchDetail->setValue(this->attrs.GetInt(EmitterAttrs::StretchDetail));
	this->particleUi->animationPhases->setValue(this->attrs.GetInt(EmitterAttrs::AnimPhases));

	float4 windDir = this->attrs.GetFloat4(EmitterAttrs::WindDirection);
	this->particleUi->windDirX->setValue(windDir.x());
	this->particleUi->windDirY->setValue(windDir.y());
	this->particleUi->windDirZ->setValue(windDir.z());

	this->particleUi->primitiveGroupIndex->setValue(this->primGroupIndex);
	this->particleUi->emitterMesh->setText(mesh.AsCharPtr());

	// setup connections of attributes
	connect(this->particleUi->textureTiles, SIGNAL(valueChanged(double)), this, SLOT(TextureTileChanged(double)));
	connect(this->particleUi->phasesPerSecond, SIGNAL(valueChanged(double)), this, SLOT(PhasesPerSecondChanged(double)));
	connect(this->particleUi->emissionDuration, SIGNAL(valueChanged(double)), this, SLOT(EmissionDurationChanged(double)));
	connect(this->particleUi->initRotationAngleMin, SIGNAL(valueChanged(double)), this, SLOT(StartRotationMinChanged(double)));
	connect(this->particleUi->initRotationAngleMax, SIGNAL(valueChanged(double)), this, SLOT(StartRotationMaxChanged(double)));
	connect(this->particleUi->startDelay, SIGNAL(valueChanged(double)), this, SLOT(StartDelayChanged(double)));
	connect(this->particleUi->precalcAtStart, SIGNAL(valueChanged(double)), this, SLOT(PrecalcTimeChanged(double)));
	connect(this->particleUi->initVectorRand, SIGNAL(valueChanged(double)), this, SLOT(VelocityRandomizeChanged(double)));
	connect(this->particleUi->rotationRandomSeed, SIGNAL(valueChanged(double)), this, SLOT(RotationRandomizeChanged(double)));
	connect(this->particleUi->stretch, SIGNAL(valueChanged(double)), this, SLOT(ParticleStretchChanged(double)));
	connect(this->particleUi->activityDistance, SIGNAL(valueChanged(double)), this, SLOT(ActivityDistanceChanged(double)));
	connect(this->particleUi->gravity, SIGNAL(valueChanged(double)), this, SLOT(GravityChanged(double)));
	connect(this->particleUi->precalcAtStart, SIGNAL(valueChanged(double)), this, SLOT(PrecalcTimeChanged(double)));
	connect(this->particleUi->looping, SIGNAL(toggled(bool)), this, SLOT(LoopingChanged(bool)));
	connect(this->particleUi->randomizeRotation, SIGNAL(toggled(bool)), this, SLOT(RandomizeRotationChanged(bool)));
	connect(this->particleUi->stretchToStart, SIGNAL(toggled(bool)), this, SLOT(StretchToStartChanged(bool)));
	connect(this->particleUi->fadeOut, SIGNAL(toggled(bool)), this, SLOT(ViewAngleFadeChanged(bool)));
	connect(this->particleUi->viewAligned, SIGNAL(toggled(bool)), this, SLOT(BillboardChanged(bool)));
	connect(this->particleUi->oldestFirst, SIGNAL(toggled(bool)), this, SLOT(RenderOldestFirst(bool)));
	connect(this->particleUi->sizeRandomSeed, SIGNAL(valueChanged(double)), this, SLOT(SizeRandomizeChanged(double)));
	connect(this->particleUi->stretchDetail, SIGNAL(valueChanged(int)), this, SLOT(StretchDetailChanged(int)));
	connect(this->particleUi->animationPhases, SIGNAL(valueChanged(int)), this, SLOT(AnimPhasesChanged(int)));
	connect(this->particleUi->windDirX, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));
	connect(this->particleUi->windDirY, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));
	connect(this->particleUi->windDirZ, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));

	connect(this->particleUi->primitiveGroupIndex, SIGNAL(valueChanged(int)), this, SLOT(PrimitiveGroupIndexChanged(int)));
	connect(this->particleUi->singlePointEmitter, SIGNAL(toggled(bool)), this, SLOT(SinglePointEmitterPressed()));
	connect(this->particleUi->deleteNodeButton, SIGNAL(clicked()), this, SLOT(OnDeleteParticleNode()));

	// start updates again
	this->isUpdatingValues = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::Refresh()
{
	Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();

	// avoid emitting signals from UI
	this->isUpdatingValues = true;

	String mesh = attributes->GetEmitterMesh(this->nodePath);
	this->attrs = attributes->GetEmitterAttrs(this->nodePath);
	if (mesh.IsEmpty())
	{
		this->particleUi->emitterMesh->setText("single point emitter mesh");
		this->particleUi->singlePointEmitter->setChecked(true);
		this->particleUi->primitiveGroupIndex->setEnabled(false);
		this->emitterMesh = ParticleServer::Instance()->GetDefaultEmitterMesh();
	}
	else
	{
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, mesh, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->particleUi->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
	}

	// fill emitter mesh
	this->particleUi->emitterMesh->setText(attributes->GetEmitterMesh(this->nodePath).AsCharPtr());

	this->particleUi->colorRedFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Red), EmitterAttrs::Red);
	this->particleUi->colorGreenFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Green), EmitterAttrs::Green);
	this->particleUi->colorBlueFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Blue), EmitterAttrs::Blue);
	this->particleUi->colorAlphaFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Alpha), EmitterAttrs::Alpha);
	this->particleUi->frequencyFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::EmissionFrequency), EmitterAttrs::EmissionFrequency);
	this->particleUi->lifetimeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::LifeTime), EmitterAttrs::LifeTime);
	this->particleUi->initVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::StartVelocity), EmitterAttrs::StartVelocity);
	this->particleUi->rotationVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::RotationVelocity), EmitterAttrs::RotationVelocity);
	this->particleUi->sizeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Size), EmitterAttrs::Size);
	this->particleUi->spreadMinFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMin), EmitterAttrs::SpreadMin);
	this->particleUi->spreadMaxFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMax), EmitterAttrs::SpreadMax);
	this->particleUi->airResistanceFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::AirResistance), EmitterAttrs::AirResistance);
	this->particleUi->velocityFactorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::VelocityFactor), EmitterAttrs::VelocityFactor);
	this->particleUi->massFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Mass), EmitterAttrs::Mass);
	this->particleUi->timeManipulatorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::TimeManipulator), EmitterAttrs::TimeManipulator);

	this->particleUi->textureTiles->setValue(this->attrs.GetFloat(EmitterAttrs::TextureTile));
	this->particleUi->phasesPerSecond->setValue(this->attrs.GetFloat(EmitterAttrs::PhasesPerSecond));
	this->particleUi->emissionDuration->setValue(this->attrs.GetFloat(EmitterAttrs::EmissionDuration));
	this->particleUi->initRotationAngleMin->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMin));
	this->particleUi->initRotationAngleMax->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMax));
	this->particleUi->sizeRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::SizeRandomize));
	this->particleUi->startDelay->setValue(this->attrs.GetFloat(EmitterAttrs::StartDelay));
	this->particleUi->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->particleUi->initVectorRand->setValue(this->attrs.GetFloat(EmitterAttrs::VelocityRandomize));
	this->particleUi->rotationRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::RotationRandomize));
	this->particleUi->stretch->setValue(this->attrs.GetFloat(EmitterAttrs::ParticleStretch));
	this->particleUi->activityDistance->setValue(this->attrs.GetFloat(EmitterAttrs::ActivityDistance));
	this->particleUi->gravity->setValue(this->attrs.GetFloat(EmitterAttrs::Gravity));
	this->particleUi->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->particleUi->looping->setChecked(this->attrs.GetBool(EmitterAttrs::Looping));
	this->particleUi->randomizeRotation->setChecked(this->attrs.GetBool(EmitterAttrs::RandomizeRotation));
	this->particleUi->stretchToStart->setChecked(this->attrs.GetBool(EmitterAttrs::StretchToStart));
	this->particleUi->fadeOut->setChecked(this->attrs.GetBool(EmitterAttrs::ViewAngleFade));
	this->particleUi->viewAligned->setChecked(this->attrs.GetBool(EmitterAttrs::Billboard));
	this->particleUi->oldestFirst->setChecked(this->attrs.GetBool(EmitterAttrs::RenderOldestFirst));
	this->particleUi->stretchDetail->setValue(this->attrs.GetInt(EmitterAttrs::StretchDetail));
	this->particleUi->animationPhases->setValue(this->attrs.GetInt(EmitterAttrs::AnimPhases));

	float4 windDir = this->attrs.GetFloat4(EmitterAttrs::WindDirection);
	this->particleUi->windDirX->setValue(windDir.x());
	this->particleUi->windDirY->setValue(windDir.y());
	this->particleUi->windDirZ->setValue(windDir.z());

	this->particleUi->primitiveGroupIndex->setValue(this->primGroupIndex);
	this->particleUi->emitterMesh->setText(mesh.AsCharPtr());

	// enable signals again
	this->isUpdatingValues = false;

	Ptr<Particles::MutableParticleSystemInstance> particleSystem = this->particleStateNode->GetParticleSystemInstance().downcast<Particles::MutableParticleSystemInstance>();
	particleSystem->UpdateEmitter(this->attrs, this->primGroupIndex, this->emitterMesh);
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::Discard()
{
	if (this->managedEmitterMesh.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedEmitterMesh.upcast<Resources::ManagedResource>());
	}	

	this->managedEmitterMesh = 0;
	this->emitterMesh = 0;
	ModelNodeHandler::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::BrowseMesh()
{
	// create string for meshes
	IO::URI msh("parmsh:");

	// open file dialog
	QFileDialog dialog(QApplication::activeWindow(), tr("Pick mesh"), msh.GetHostAndLocalPath().AsCharPtr(), tr("*.nvx2"));

	// exec dialog
	int result = dialog.exec();
	if (result == QDialog::Accepted)
	{
		// convert to nebula string
		String mesh = dialog.selectedFiles()[0].toUtf8().constData();

		// get category
		String category = mesh.ExtractLastDirName();

		// get actual file
		String meshFile = mesh.ExtractFileName();

		// format text
		String value;
		value.Format("parmsh:%s/%s", category.AsCharPtr(), meshFile.AsCharPtr());

		// set text
		this->particleUi->emitterMesh->setText(value.AsCharPtr());

		// load mesh
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, value, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->particleUi->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);

		// update mesh
		Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();

		// set mesh
		attributes->SetEmitterMesh(this->nodePath, value);

		// apply updates
		this->Apply();
	}
}

//------------------------------------------------------------------------------
/**
	Start particle if it has stopped
*/
void
ParticleNodeHandler::Restart()
{
	this->particleStateNode->GetParticleSystemInstance()->Restart();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::PrimitiveGroupIndexChanged(int i)
{
	// update mesh
	Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
	ToolkitUtil::ModelAttributes::AppendixNode node = attrs->GetAppendixNode(this->nodeName);
	node.data.particle.primGroup = this->particleUi->primitiveGroupIndex->value();
	this->primGroupIndex = node.data.particle.primGroup;
	attrs->DeleteAppendixNode(this->nodeName);
	attrs->AddAppendixNode(this->nodeName, node);

	// apply updates
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::Apply()
{
	if (this->isUpdatingValues) return;

	// get model constants
	Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();

	// set attributes
	attributes->SetEmitterAttrs(this->nodePath, this->attrs);

	// set attrs in particle system
	Ptr<Particles::MutableParticleSystemInstance> particleSystem = this->particleStateNode->GetParticleSystemInstance().downcast<Particles::MutableParticleSystemInstance>();
	particleSystem->UpdateEmitter(this->attrs, this->primGroupIndex, this->emitterMesh);

	// apply modifications
	this->modelHandler->OnModelModified();
}

//------------------------------------------------------------------------------

/**
*/
void 
ParticleNodeHandler::EnvelopeChanged(EmitterAttrs::EnvelopeAttr attr)
{
	// get frame
	ParticleEnvelopeWidget* widget = static_cast<ParticleEnvelopeWidget*>(this->sender());

	// set attribute
	this->attrs.SetEnvelope(attr, widget->GetEnvelope());

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::EmissionDurationChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::EmissionDuration, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::ActivityDistanceChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::ActivityDistance, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::StartRotationMinChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::StartRotationMin, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::StartRotationMaxChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::StartRotationMax, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::GravityChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::Gravity, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::ParticleStretchChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::ParticleStretch, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::VelocityRandomizeChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::VelocityRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::RotationRandomizeChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::RotationRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::SizeRandomizeChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::SizeRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::PrecalcTimeChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::PrecalcTime, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::StartDelayChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::StartDelay, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::TextureTileChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::TextureTile, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::PhasesPerSecondChanged(double f)
{
	this->attrs.SetFloat(EmitterAttrs::PhasesPerSecond, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::WindDirectionChanged()
{
	double x = this->particleUi->windDirX->value();
	double y = this->particleUi->windDirY->value();
	double z = this->particleUi->windDirZ->value();
	this->attrs.SetFloat4(EmitterAttrs::WindDirection, float4(x, y, z, 0));

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::LoopingChanged(bool b)
{
	this->attrs.SetBool(EmitterAttrs::Looping, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::RandomizeRotationChanged(bool b)
{
	this->attrs.SetBool(EmitterAttrs::RandomizeRotation, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::StretchToStartChanged(bool b)
{
	this->attrs.SetBool(EmitterAttrs::StretchToStart, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::RenderOldestFirst(bool b)
{
	this->attrs.SetBool(EmitterAttrs::RenderOldestFirst, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::ViewAngleFadeChanged(bool b)
{
	this->attrs.SetBool(EmitterAttrs::ViewAngleFade, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::BillboardChanged(bool b)
{
	this->attrs.SetBool(EmitterAttrs::Billboard, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::StretchDetailChanged(int i)
{
	this->attrs.SetInt(EmitterAttrs::StretchDetail, i);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::AnimPhasesChanged(int i)
{
	this->attrs.SetInt(EmitterAttrs::AnimPhases, i);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::SinglePointEmitterPressed()
{
	bool checked = this->particleUi->singlePointEmitter->isChecked();
	if (checked)
	{
		this->particleUi->emitterMesh->setEnabled(false);
		this->particleUi->browseEmitterMesh->setEnabled(false);
		this->particleUi->primitiveGroupIndex->setEnabled(false);

		// update mesh
		Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();

		// set mesh
		attributes->SetEmitterMesh(this->nodePath, "");

		// unload mesh
		if (this->managedEmitterMesh.isvalid())
		{
			Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedEmitterMesh.upcast<Resources::ManagedResource>());
		}		
		this->managedEmitterMesh = 0;
		this->emitterMesh = ParticleServer::Instance()->GetDefaultEmitterMesh();
	}
	else
	{
		this->particleUi->emitterMesh->setEnabled(true);
		this->particleUi->browseEmitterMesh->setEnabled(true);
		this->particleUi->primitiveGroupIndex->setEnabled(true);

		// update mesh
		Ptr<ModelAttributes> attributes = this->modelHandler->GetAttributes();

		// set mesh
		attributes->SetEmitterMesh(this->nodePath, this->particleUi->emitterMesh->text().toUtf8().constData());

		// load mesh
		String resource = this->particleUi->emitterMesh->text().toUtf8().constData();
		if (!resource.IsEmpty())
		{
			this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, resource, NULL, true).downcast<Resources::ManagedMesh>();
			this->emitterMesh = this->managedEmitterMesh->GetMesh();
			this->particleUi->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
		}
	}	

	// apply changes
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::OnDeleteParticleNode()
{
	this->modelHandler->RemoveParticleNode(this->nodePath, this->nodeName);
}

} // namespace Widgets