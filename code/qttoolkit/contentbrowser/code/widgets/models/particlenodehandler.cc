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

using namespace Particles;
using namespace ToolkitUtil;
using namespace IO;
using namespace ContentBrowser;
using namespace Math;
using namespace Materials;
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
ParticleNodeHandler::Setup(const String& material, const Util::String& resource)
{
	// copy attrs
	this->attrs = attrs;

	// clear reference maps
	this->textureImgMap.clear();
	this->textureTextMap.clear();
	this->textureLabelMap.clear();
	this->variableLabelMap.clear();
	this->variableSliderMap.clear();
	this->variableBoolMap.clear();
	this->variableVectorFieldMap.clear();
	this->variableVectorMap.clear();
	this->lowerLimitFloatMap.clear();
	this->upperLimitFloatMap.clear();
	this->lowerLimitIntMap.clear();
	this->upperLimitIntMap.clear();

	// copy resource
	this->resource = resource;

	// stop updating stuff
	this->isUpdatingValues = true;

	// get attribute
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();
	Ptr<ModelConstants> constants = this->itemHandler->GetConstants();

	// set primitive group index
	this->primGroupIndex = constants->GetParticleNode(this->nodeName).primitiveGroupIndex;

	String mesh = attributes->GetEmitterMesh(this->nodePath);
	this->attrs = attributes->GetEmitterAttrs(this->nodePath);
	if (mesh.IsEmpty())
	{
		this->ui->emitterMesh->setText("single point emitter mesh");
		this->ui->singlePointEmitter->setChecked(true);
		this->ui->primitiveGroupIndex->setEnabled(false);
		this->emitterMesh = ParticleServer::Instance()->GetDefaultEmitterMesh();
	}
	else
	{
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, mesh, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->ui->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
	}
	
	// fill emitter mesh
	this->ui->emitterMesh->setText(attributes->GetEmitterMesh(this->nodePath).AsCharPtr());

	// connect browser button
	connect(this->ui->browseEmitterMesh, SIGNAL(pressed()), this, SLOT(BrowseMesh()));

	// get layout
	QVBoxLayout* mainLayout = static_cast<QVBoxLayout*>(this->ui->variableFrame->layout());
	
	// setup UI
	this->MakeMaterialUI(mainLayout, this->ui->nodeName, this->ui->materialBox, this->ui->materialHelp, material);

	// get particle system node instance from state node
	this->particleStateNode = this->stateNode.downcast<ParticleSystemNodeInstance>();

	// setup envelope frames
	this->ui->colorRedFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Red), EmitterAttrs::Red);
	this->ui->colorGreenFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Green), EmitterAttrs::Green);
	this->ui->colorBlueFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Blue), EmitterAttrs::Blue);
	this->ui->colorAlphaFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Alpha), EmitterAttrs::Alpha);
	this->ui->frequencyFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::EmissionFrequency), EmitterAttrs::EmissionFrequency);
	this->ui->lifetimeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::LifeTime), EmitterAttrs::LifeTime);
	this->ui->initVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::StartVelocity), EmitterAttrs::StartVelocity);
	this->ui->rotationVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::RotationVelocity), EmitterAttrs::RotationVelocity);
	this->ui->sizeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Size), EmitterAttrs::Size);
	this->ui->spreadMinFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMin), EmitterAttrs::SpreadMin);
	this->ui->spreadMaxFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMax), EmitterAttrs::SpreadMax);
	this->ui->airResistanceFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::AirResistance), EmitterAttrs::AirResistance);
	this->ui->velocityFactorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::VelocityFactor), EmitterAttrs::VelocityFactor);
	this->ui->massFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Mass), EmitterAttrs::Mass);
	this->ui->timeManipulatorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::TimeManipulator), EmitterAttrs::TimeManipulator);

	// connect signals from frames
	connect(this->ui->colorRedFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->colorGreenFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->colorBlueFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->colorAlphaFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->frequencyFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->lifetimeFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->initVelocityFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->rotationVelocityFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->sizeFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->spreadMinFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->spreadMaxFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->airResistanceFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->velocityFactorFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->massFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));
	connect(this->ui->timeManipulatorFrame, SIGNAL(ValueChanged(Particles::EmitterAttrs::EnvelopeAttr)), this, SLOT(EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr)));

	// setup attributes
	this->ui->textureTiles->setValue(this->attrs.GetFloat(EmitterAttrs::TextureTile));
	this->ui->phasesPerSecond->setValue(this->attrs.GetFloat(EmitterAttrs::PhasesPerSecond));
	this->ui->emissionDuration->setValue(this->attrs.GetFloat(EmitterAttrs::EmissionDuration));
	this->ui->initRotationAngleMin->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMin));
	this->ui->initRotationAngleMax->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMax));
	this->ui->sizeRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::SizeRandomize));
	this->ui->startDelay->setValue(this->attrs.GetFloat(EmitterAttrs::StartDelay));
	this->ui->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->ui->initVectorRand->setValue(this->attrs.GetFloat(EmitterAttrs::VelocityRandomize));
	this->ui->rotationRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::RotationRandomize));
	this->ui->stretch->setValue(this->attrs.GetFloat(EmitterAttrs::ParticleStretch));
	this->ui->activityDistance->setValue(this->attrs.GetFloat(EmitterAttrs::ActivityDistance));
	this->ui->gravity->setValue(this->attrs.GetFloat(EmitterAttrs::Gravity));
	this->ui->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->ui->looping->setChecked(this->attrs.GetBool(EmitterAttrs::Looping));
	this->ui->randomizeRotation->setChecked(this->attrs.GetBool(EmitterAttrs::RandomizeRotation));
	this->ui->stretchToStart->setChecked(this->attrs.GetBool(EmitterAttrs::StretchToStart));
	this->ui->fadeOut->setChecked(this->attrs.GetBool(EmitterAttrs::ViewAngleFade));
	this->ui->viewAligned->setChecked(this->attrs.GetBool(EmitterAttrs::Billboard));
	this->ui->oldestFirst->setChecked(this->attrs.GetBool(EmitterAttrs::RenderOldestFirst));
	this->ui->stretchDetail->setValue(this->attrs.GetInt(EmitterAttrs::StretchDetail));
	this->ui->animationPhases->setValue(this->attrs.GetInt(EmitterAttrs::AnimPhases));

	float4 windDir = this->attrs.GetFloat4(EmitterAttrs::WindDirection);
	this->ui->windDirX->setValue(windDir.x());
	this->ui->windDirY->setValue(windDir.y());
	this->ui->windDirZ->setValue(windDir.z());

	this->ui->primitiveGroupIndex->setValue(this->primGroupIndex);
	this->ui->emitterMesh->setText(mesh.AsCharPtr());

	// setup connections of attributes
	connect(this->ui->textureTiles, SIGNAL(valueChanged(double)), this, SLOT(TextureTileChanged(double)));
	connect(this->ui->phasesPerSecond, SIGNAL(valueChanged(double)), this, SLOT(PhasesPerSecondChanged(double)));
	connect(this->ui->emissionDuration, SIGNAL(valueChanged(double)), this, SLOT(EmissionDurationChanged(double)));
	connect(this->ui->initRotationAngleMin, SIGNAL(valueChanged(double)), this, SLOT(StartRotationMinChanged(double)));
	connect(this->ui->initRotationAngleMax, SIGNAL(valueChanged(double)), this, SLOT(StartRotationMaxChanged(double)));
	connect(this->ui->startDelay, SIGNAL(valueChanged(double)), this, SLOT(StartDelayChanged(double)));
	connect(this->ui->precalcAtStart, SIGNAL(valueChanged(double)), this, SLOT(PrecalcTimeChanged(double)));
	connect(this->ui->initVectorRand, SIGNAL(valueChanged(double)), this, SLOT(VelocityRandomizeChanged(double)));
	connect(this->ui->rotationRandomSeed, SIGNAL(valueChanged(double)), this, SLOT(RotationRandomizeChanged(double)));
	connect(this->ui->stretch, SIGNAL(valueChanged(double)), this, SLOT(ParticleStretchChanged(double)));
	connect(this->ui->activityDistance, SIGNAL(valueChanged(double)), this, SLOT(ActivityDistanceChanged(double)));
	connect(this->ui->gravity, SIGNAL(valueChanged(double)), this, SLOT(GravityChanged(double)));
	connect(this->ui->precalcAtStart, SIGNAL(valueChanged(double)), this, SLOT(PrecalcTimeChanged(double)));
	connect(this->ui->looping, SIGNAL(toggled(bool)), this, SLOT(LoopingChanged(bool)));
	connect(this->ui->randomizeRotation, SIGNAL(toggled(bool)), this, SLOT(RandomizeRotationChanged(bool)));
	connect(this->ui->stretchToStart, SIGNAL(toggled(bool)), this, SLOT(StretchToStartChanged(bool)));
	connect(this->ui->fadeOut, SIGNAL(toggled(bool)), this, SLOT(ViewAngleFadeChanged(bool)));
	connect(this->ui->viewAligned, SIGNAL(toggled(bool)), this, SLOT(BillboardChanged(bool)));
	connect(this->ui->oldestFirst, SIGNAL(toggled(bool)), this, SLOT(RenderOldestFirst(bool)));
	connect(this->ui->sizeRandomSeed, SIGNAL(valueChanged(double)), this, SLOT(SizeRandomizeChanged(double)));
	connect(this->ui->stretchDetail, SIGNAL(valueChanged(int)), this, SLOT(StretchDetailChanged(int)));
	connect(this->ui->animationPhases, SIGNAL(valueChanged(int)), this, SLOT(AnimPhasesChanged(int)));
	connect(this->ui->windDirX, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));
	connect(this->ui->windDirY, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));
	connect(this->ui->windDirZ, SIGNAL(valueChanged(double)), this, SLOT(WindDirectionChanged()));

	connect(this->ui->primitiveGroupIndex, SIGNAL(valueChanged(int)), this, SLOT(PrimitiveGroupIndexChanged(int)));
	connect(this->ui->singlePointEmitter, SIGNAL(toggled(bool)), this, SLOT(SinglePointEmitterPressed()));
	connect(this->ui->deleteNodeButton, SIGNAL(clicked()), this, SLOT(OnDeleteParticleNode()));

	// start updates again
	this->isUpdatingValues = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::SoftRefresh(const Util::String& material, const Util::String& resource)
{
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// avoid emitting signals from UI
	this->isUpdatingValues = true;

	String mesh = attributes->GetEmitterMesh(this->nodePath);
	this->attrs = attributes->GetEmitterAttrs(this->nodePath);
	if (mesh.IsEmpty())
	{
		this->ui->emitterMesh->setText("single point emitter mesh");
		this->ui->singlePointEmitter->setChecked(true);
		this->ui->primitiveGroupIndex->setEnabled(false);
		this->emitterMesh = ParticleServer::Instance()->GetDefaultEmitterMesh();
	}
	else
	{
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, mesh, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->ui->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
	}

	// fill emitter mesh
	this->ui->emitterMesh->setText(attributes->GetEmitterMesh(this->nodePath).AsCharPtr());

	this->ui->colorRedFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Red), EmitterAttrs::Red);
	this->ui->colorGreenFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Green), EmitterAttrs::Green);
	this->ui->colorBlueFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Blue), EmitterAttrs::Blue);
	this->ui->colorAlphaFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Alpha), EmitterAttrs::Alpha);
	this->ui->frequencyFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::EmissionFrequency), EmitterAttrs::EmissionFrequency);
	this->ui->lifetimeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::LifeTime), EmitterAttrs::LifeTime);
	this->ui->initVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::StartVelocity), EmitterAttrs::StartVelocity);
	this->ui->rotationVelocityFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::RotationVelocity), EmitterAttrs::RotationVelocity);
	this->ui->sizeFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Size), EmitterAttrs::Size);
	this->ui->spreadMinFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMin), EmitterAttrs::SpreadMin);
	this->ui->spreadMaxFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::SpreadMax), EmitterAttrs::SpreadMax);
	this->ui->airResistanceFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::AirResistance), EmitterAttrs::AirResistance);
	this->ui->velocityFactorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::VelocityFactor), EmitterAttrs::VelocityFactor);
	this->ui->massFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::Mass), EmitterAttrs::Mass);
	this->ui->timeManipulatorFrame->Setup(this->attrs.GetEnvelope(EmitterAttrs::TimeManipulator), EmitterAttrs::TimeManipulator);

	this->ui->textureTiles->setValue(this->attrs.GetFloat(EmitterAttrs::TextureTile));
	this->ui->phasesPerSecond->setValue(this->attrs.GetFloat(EmitterAttrs::PhasesPerSecond));
	this->ui->emissionDuration->setValue(this->attrs.GetFloat(EmitterAttrs::EmissionDuration));
	this->ui->initRotationAngleMin->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMin));
	this->ui->initRotationAngleMax->setValue(this->attrs.GetFloat(EmitterAttrs::StartRotationMax));
	this->ui->sizeRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::SizeRandomize));
	this->ui->startDelay->setValue(this->attrs.GetFloat(EmitterAttrs::StartDelay));
	this->ui->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->ui->initVectorRand->setValue(this->attrs.GetFloat(EmitterAttrs::VelocityRandomize));
	this->ui->rotationRandomSeed->setValue(this->attrs.GetFloat(EmitterAttrs::RotationRandomize));
	this->ui->stretch->setValue(this->attrs.GetFloat(EmitterAttrs::ParticleStretch));
	this->ui->activityDistance->setValue(this->attrs.GetFloat(EmitterAttrs::ActivityDistance));
	this->ui->gravity->setValue(this->attrs.GetFloat(EmitterAttrs::Gravity));
	this->ui->precalcAtStart->setValue(this->attrs.GetFloat(EmitterAttrs::PrecalcTime));
	this->ui->looping->setChecked(this->attrs.GetBool(EmitterAttrs::Looping));
	this->ui->randomizeRotation->setChecked(this->attrs.GetBool(EmitterAttrs::RandomizeRotation));
	this->ui->stretchToStart->setChecked(this->attrs.GetBool(EmitterAttrs::StretchToStart));
	this->ui->fadeOut->setChecked(this->attrs.GetBool(EmitterAttrs::ViewAngleFade));
	this->ui->viewAligned->setChecked(this->attrs.GetBool(EmitterAttrs::Billboard));
	this->ui->oldestFirst->setChecked(this->attrs.GetBool(EmitterAttrs::RenderOldestFirst));
	this->ui->stretchDetail->setValue(this->attrs.GetInt(EmitterAttrs::StretchDetail));
	this->ui->animationPhases->setValue(this->attrs.GetInt(EmitterAttrs::AnimPhases));

	float4 windDir = this->attrs.GetFloat4(EmitterAttrs::WindDirection);
	this->ui->windDirX->setValue(windDir.x());
	this->ui->windDirY->setValue(windDir.y());
	this->ui->windDirZ->setValue(windDir.z());

	this->ui->primitiveGroupIndex->setValue(this->primGroupIndex);
	this->ui->emitterMesh->setText(mesh.AsCharPtr());

	// enable signals again
	this->isUpdatingValues = false;

	// set attrs in particle system
	this->particleStateNode->GetParticleSystemInstance()->UpdateEmitter(this->attrs, this->primGroupIndex, this->emitterMesh);

	ModelNodeHandler::SoftRefresh(material, resource);
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
		this->ui->emitterMesh->setText(value.AsCharPtr());

		// load mesh
		this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, value, NULL, true).downcast<Resources::ManagedMesh>();
		this->emitterMesh = this->managedEmitterMesh->GetMesh();
		this->ui->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);

		// update mesh
		Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

		// set mesh
		attributes->SetEmitterMesh(this->nodePath, value);

		// apply updates
		this->Apply();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::PrimitiveGroupIndexChanged(int i)
{
	// update mesh
	Ptr<ModelConstants> constants = this->itemHandler->GetConstants();
	ToolkitUtil::ModelConstants::ParticleNode node = constants->GetParticleNode(this->nodeName);
	node.primitiveGroupIndex = this->ui->primitiveGroupIndex->value();
	this->primGroupIndex = node.primitiveGroupIndex;
	constants->DeleteParticleNode(this->nodeName);
	constants->AddParticleNode(this->nodeName, node);

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
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// set attributes
	attributes->SetEmitterAttrs(this->nodePath, this->attrs);

	// set attrs in particle system
	this->particleStateNode->GetParticleSystemInstance()->UpdateEmitter(this->attrs, this->primGroupIndex, this->emitterMesh);

	// apply modifications
	this->itemHandler->OnModelModified();
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
ParticleNodeHandler::EmissionDurationChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::EmissionDuration, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::ActivityDistanceChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::ActivityDistance, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::StartRotationMinChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::StartRotationMin, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::StartRotationMaxChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::StartRotationMax, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::GravityChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::Gravity, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::ParticleStretchChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::ParticleStretch, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::VelocityRandomizeChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::VelocityRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::RotationRandomizeChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::RotationRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::SizeRandomizeChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::SizeRandomize, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::PrecalcTimeChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::PrecalcTime, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::StartDelayChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::StartDelay, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::TextureTileChanged( double f )
{
	this->attrs.SetFloat(EmitterAttrs::TextureTile, f);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::PhasesPerSecondChanged( double f )
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
	double x = this->ui->windDirX->value();
	double y = this->ui->windDirY->value();
	double z = this->ui->windDirZ->value();
	this->attrs.SetFloat4(EmitterAttrs::WindDirection, float4(x, y, z, 0));

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::LoopingChanged( bool b )
{
	this->attrs.SetBool(EmitterAttrs::Looping, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::RandomizeRotationChanged( bool b )
{
	this->attrs.SetBool(EmitterAttrs::RandomizeRotation, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::StretchToStartChanged( bool b )
{
	this->attrs.SetBool(EmitterAttrs::StretchToStart, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::RenderOldestFirst( bool b )
{
	this->attrs.SetBool(EmitterAttrs::RenderOldestFirst, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::ViewAngleFadeChanged( bool b )
{
	this->attrs.SetBool(EmitterAttrs::ViewAngleFade, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::BillboardChanged( bool b )
{
	this->attrs.SetBool(EmitterAttrs::Billboard, b);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::StretchDetailChanged( int i )
{
	this->attrs.SetInt(EmitterAttrs::StretchDetail, i);

	// apply modifications
	this->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleNodeHandler::AnimPhasesChanged( int i )
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
	bool checked = this->ui->singlePointEmitter->isChecked();
	if (checked)
	{
		this->ui->emitterMesh->setEnabled(false);
		this->ui->browseEmitterMesh->setEnabled(false);
		this->ui->primitiveGroupIndex->setEnabled(false);

		// update mesh
		Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

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
		this->ui->emitterMesh->setEnabled(true);
		this->ui->browseEmitterMesh->setEnabled(true);
		this->ui->primitiveGroupIndex->setEnabled(true);

		// update mesh
		Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

		// set mesh
		attributes->SetEmitterMesh(this->nodePath, this->ui->emitterMesh->text().toUtf8().constData());

		// load mesh
		String resource = this->ui->emitterMesh->text().toUtf8().constData();
		if (!resource.IsEmpty())
		{
			this->managedEmitterMesh = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Mesh::RTTI, resource, NULL, true).downcast<Resources::ManagedMesh>();
			this->emitterMesh = this->managedEmitterMesh->GetMesh();
			this->ui->primitiveGroupIndex->setMaximum(this->emitterMesh->GetNumPrimitiveGroups() - 1);
		}
	}	

	// apply changes
	this->Apply();

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleNodeHandler::OnDeleteParticleNode()
{
	this->itemHandler->RemoveParticleNode(this->nodePath, this->nodeName);
}

} // namespace Widgets