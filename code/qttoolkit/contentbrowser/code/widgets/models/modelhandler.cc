//------------------------------------------------------------------------------
//  modelitemhandler.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelhandler.h"
#include "previewer/previewstate.h"
#include "contentbrowserapp.h"
#include "n3util/n3xmlextractor.h"
#include "io/ioserver.h"
#include "ui_modelnodeinfowidget.h"
#include "graphics/graphicsprotocol.h"
#include "modelnodeframe.h"
#include "particlenodeframe.h"
#include "n3util/n3xmlexporter.h"
#include "graphics/graphicsinterface.h"
#include <QBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include "characternodeframe.h"
#include "modelutil/modelbuilder.h"
#include "modelnodehandler.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"
#include "messaging/messagecallbackhandler.h"


using namespace ContentBrowser;
using namespace ToolkitUtil;
using namespace Util;
using namespace IO;
using namespace Graphics;
using namespace Actions;
using namespace Particles;

namespace Widgets
{
__ImplementClass(Widgets::ModelHandler, 'MOIH', Widgets::BaseHandler);

//------------------------------------------------------------------------------
/**
*/
ModelHandler::ModelHandler() : 
	attributes(0),
	constants(0),
	physics(0),
	characterFrame(0)
{
	this->action = ModelModifyAction::Create();
	this->action->SetModelHandler(this);
}

//------------------------------------------------------------------------------
/**
*/
ModelHandler::~ModelHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::Cleanup()
{
    // cleanup action
    this->action->Cleanup();
    this->action = 0;

    // remove intermediate directory
    IoServer::Instance()->DeleteDirectory("int:");
}

//------------------------------------------------------------------------------
/**
*/
bool
ModelHandler::Preview()
{
	// get preview state
	Ptr<PreviewState> previewState = ContentBrowserApp::Instance()->GetPreviewState();

	// create original resource uri
	String resource;
	resource.Format("mdl:%s/%s.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// if intermediate dir doesn't exist, create it
	IoServer::Instance()->CreateDirectory(URI("int:models/" + this->category));

	// create temporary resource uri
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// create copy of existing resource with an extension, if file exists this will just replace it
	IoServer::Instance()->CopyFile(resource, tempResource);

	// create original physics resource uri
	String phResource;
	phResource.Format("phys:%s/%s.np3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// if intermediate dir doesn't exist, create it
	IoServer::Instance()->CreateDirectory(URI("int:physics/" + this->category));

	// create temporary resource uri
	String tempPhysicsResource;
	tempPhysicsResource.Format("int:physics/%s/%s_temp.np3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// create copy of existing resource with an extension, if file exists this will just replace it
	IoServer::Instance()->CopyFile(phResource, tempPhysicsResource);

	// preview the model
	if (!previewState->SetModel(tempResource))
	{
		// remove temp files if we cannot properly load them
		IoServer::Instance()->DeleteFile(tempResource);
		return false;
	}

	if (!previewState->SetPhysics(tempPhysicsResource))	
	{ 
		IoServer::Instance()->DeleteFile(tempPhysicsResource);
		QMessageBox::critical(NULL, "Failed to open physics", "Failed to open physics resource");

	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::Setup()
{
	n_assert(this->ui);

	// enable the UI again
	this->ui->frame->setDisabled(false);

	// call base class
	BaseHandler::Setup();	

	// format internal model name
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// set category and model of action
	this->action->SetCategory(this->category);
	this->action->SetModel(this->model);

	// get model data, force a reload
	this->attributes = ModelDatabase::Instance()->LookupAttributes(this->category + "/" + this->model, true);
	this->constants = ModelDatabase::Instance()->LookupConstants(this->category + "/" + this->model, true);
	this->physics = ModelDatabase::Instance()->LookupPhysics(this->category + "/" + this->model, true);

	// open files
	String attrPath, constPath, physPath;
	attrPath.Format("src:models/%s/%s.attributes", this->category.AsCharPtr(), this->model.AsCharPtr());
	constPath.Format("src:models/%s/%s.constants", this->category.AsCharPtr(), this->model.AsCharPtr());
	physPath.Format("src:models/%s/%s.physics", this->category.AsCharPtr(), this->model.AsCharPtr());

	// open file
	if (IoServer::Instance()->FileExists(attrPath))
    {
		Ptr<Stream> file = IoServer::Instance()->CreateStream(attrPath);
		file->Open();
		void* data = file->Map();
		SizeT size = file->GetSize();
        String attrVersion;
		attrVersion.Set((const char*)data, size);
		file->Unmap();
		file->Close();

		file = IoServer::Instance()->CreateStream(constPath);
		file->Open();
		data = file->Map();
		size = file->GetSize();
		String constsVersion;
		constsVersion.Set((const char*)data, size);
		file->Unmap();
		file->Close();

		file = IoServer::Instance()->CreateStream(physPath);
		file->Open();
		data = file->Map();
		size = file->GetSize();
		String physVersion;
		physVersion.Set((const char*)data, size);
		file->Unmap();
		file->Close();

		// add version, then use this
		this->action->AddVersion(attrVersion, constsVersion, physVersion);
    }
    else
    {
        QMessageBox box;
        QString message;
        message.sprintf("The model '%s' doesn't exist!", attrPath.AsCharPtr());
        box.setText(message);
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Close);
        box.setDefaultButton(QMessageBox::Close);
        box.exec();

        // discard and return
        this->DiscardNoCancel();
        return;
    }

	this->SetupTabs();

	// connect reload buttons to actions
	connect(this->ui->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
	connect(this->ui->saveAsButton, SIGNAL(clicked()), this, SLOT(SaveAs()));
	connect(this->ui->addParticleNode, SIGNAL(clicked()), this, SLOT(AddParticleNode()));
}

//------------------------------------------------------------------------------
/**
*/
bool 
ModelHandler::Discard()
{
	// before switching models, check if our action is at the final version
	if (!this->action->IsFinal())
	{
		// if we don't, we should prompt the user to save
		QMessageBox msgBox;
		msgBox.setText("The model has been modified.");
		msgBox.setInformativeText("Before you close this model, do you wish to save first?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);

		// execute
		int ret = msgBox.exec();

		if (ret == QMessageBox::Save)
		{
			this->Save();
		}
		else if (ret == QMessageBox::Cancel)
		{
			// abort action
			return false;
		}
	}

	// clear frames
	IndexT i;
	for (i = 0; i < this->nodeFrames.Size(); i++)
	{
		this->nodeFrames[i]->Discard();
	}
	for (i = 0; i < particleFrames.Size(); i++)
	{
		this->particleFrames[i]->Discard();
	}
	this->nodeFrames.Clear();
	this->particleFrames.Clear();

	if (this->characterFrame)
	{
		this->characterFrame->Discard();
		this->characterFrame = 0;
	}

    // clear attributes
    this->attributes->Clear();
    this->constants->Clear();
    this->physics->Clear();

	// clear model stuff
	this->attributes = 0;
	this->constants = 0;
	this->physics = 0;

	// create temporary resource uri
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary xml file
	tempResource.Format("int:models/%s/%s_temp.attributes", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary xml file
	tempResource.Format("int:models/%s/%s_temp.physics", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary physics model file
	tempResource.Format("int:physics/%s/%s_temp.np3",this->category.AsCharPtr(), this->model.AsCharPtr() );

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// discard current action
	this->action->Discard();

	// clear actions from application
	ContentBrowserApp::Instance()->ClearActions();

	// disconnect stuff
	disconnect(this->ui->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
	disconnect(this->ui->saveAsButton, SIGNAL(clicked()), this, SLOT(SaveAs()));
	disconnect(this->ui->addParticleNode, SIGNAL(clicked()), this, SLOT(AddParticleNode()));

	return BaseHandler::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::DiscardNoCancel()
{
	// before switching models, check if our action is at the final version
	if (!this->action->IsFinal())
	{
		// if we don't, we should prompt the user to save
		QMessageBox msgBox;
		msgBox.setText("The model has been reimported and must be updated.");
		msgBox.setInformativeText("Do you want to save your changes before updating?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msgBox.setDefaultButton(QMessageBox::Save);

		// execute
		int ret = msgBox.exec();

		if (ret == QMessageBox::Save)
		{
			this->Save();
		}
	}

	// clear frames
	IndexT i;
	for (i = 0; i < this->nodeFrames.Size(); i++)
	{
		this->nodeFrames[i]->Discard();
	}
	for (i = 0; i < particleFrames.Size(); i++)
	{
		this->particleFrames[i]->Discard();
	}
	this->nodeFrames.Clear();
	this->particleFrames.Clear();

	if (this->characterFrame)
	{
		this->characterFrame->Discard();
		this->characterFrame = 0;
	}

    // clear attributes
    this->attributes->Clear();
    this->constants->Clear();
    this->physics->Clear();

	// clear model stuff
	this->attributes = 0;
	this->constants = 0;
	this->physics = 0;

	// create temporary resource uri
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary xml file
	tempResource.Format("int:models/%s/%s_temp.attributes", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary xml file
	tempResource.Format("int:models/%s/%s_temp.physics", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// also remove temporary physics model file
	tempResource.Format("int:physics/%s/%s_temp.np3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// delete temporary file
	IoServer::Instance()->DeleteFile(tempResource);

	// discard current action
	this->action->Discard();

	// clear actions
	ContentBrowserApp::Instance()->ClearActions();

	// set setup bool to false
	this->isSetup = false;

	// disconnect stuff
	disconnect(this->ui->saveButton, SIGNAL(clicked()), this, SLOT(Save()));
	disconnect(this->ui->saveAsButton, SIGNAL(clicked()), this, SLOT(SaveAs()));
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::Mute()
{
	this->ui->frame->setDisabled(true);
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::HardRefresh()
{
	// format internal model name
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// go through node frames and unset their variable states
	IndexT i;
	for (i = 0; i < this->nodeFrames.Size(); i++)
	{
		this->nodeFrames[i]->GetHandler()->UnsetState();
	}

	for (i = 0; i < this->particleFrames.Size(); i++)
	{
		this->particleFrames[i]->GetHandler()->UnsetState();
	}

	// now change the model, should be safe now
	this->MakeModel();

	// get shape nodes
	const Array<ToolkitUtil::ModelConstants::ShapeNode>& shapeNodes = this->constants->GetShapeNodes();
	const Array<ToolkitUtil::ModelConstants::ParticleNode>& particleNodes = this->constants->GetParticleNodes();
	const Array<ToolkitUtil::ModelConstants::Skin>& skinNodes = this->constants->GetSkins();

	// create grid layout for node frame
	QTabWidget* nodeWidget = this->ui->nodeWidget;

	// go through all normal shapes, update corresponding frames, or create frames if none exist (remember, this is done after undo/redo, so the possibility is prevalent)
	for (i = 0; i < shapeNodes.Size(); i++)
	{
		const ToolkitUtil::ModelConstants::ShapeNode& shapeNode = shapeNodes[i];
		bool createNewFrame = true;

		IndexT j;
		for (j = 0; j < this->nodeFrames.Size(); j++)
		{
			// get node frame
			Ptr<ModelNodeHandler> nodeHandler = this->nodeFrames[j]->GetHandler();
			if (nodeHandler->GetPath() == shapeNode.path)
			{
				// get state
				const State& state = this->attributes->GetState(nodeHandler->GetPath());

				// prepare for update
				this->nodeFrames[j]->setUpdatesEnabled(false);

				// refresh handler
				nodeHandler->HardRefresh(state.material, tempResource);

				// prepare for update
				this->nodeFrames[j]->setUpdatesEnabled(true);

				// frame found, dont make
				createNewFrame = false;

				// we are done with this node
				break;
			}
		}

		// if we never found a frame, we have to make a new one
		if (createNewFrame)
		{
			// create new frame
			ModelNodeFrame* nodeFrame = new ModelNodeFrame;
			this->nodeFrames.Append(nodeFrame);

			// get state
			const State& state = this->attributes->GetState(shapeNode.path);

			// setup handler
			nodeFrame->GetHandler()->SetModelHandler(this);
			nodeFrame->GetHandler()->SetType(shapeNode.type);
			nodeFrame->GetHandler()->SetName(shapeNode.name);
			nodeFrame->GetHandler()->SetPath(shapeNode.path);
			nodeFrame->GetHandler()->Setup(state.material, tempResource);

			// add frame to tab box
			nodeWidget->addTab(nodeFrame, shapeNode.name.AsCharPtr());
		}
	}

	// go through skin nodes
	for (i = 0; i < skinNodes.Size(); i++)
	{
		const ToolkitUtil::ModelConstants::Skin& skin = skinNodes[i];
		bool createNewFrame = true;

		IndexT j;
		for (j = 0; j < this->nodeFrames.Size(); j++)
		{
			// get node frame
			Ptr<ModelNodeHandler> nodeHandler = this->nodeFrames[j]->GetHandler();
			if (nodeHandler->GetPath() == skin.path)
			{
				// get state
				const State& state = this->attributes->GetState(nodeHandler->GetPath());

				// prepare for update
				this->nodeFrames[j]->setUpdatesEnabled(false);

				// refresh handler
				nodeHandler->HardRefresh(state.material, tempResource);

				// prepare for update
				this->nodeFrames[j]->setUpdatesEnabled(true);

				// frame found, dont make
				createNewFrame = false;

				// we are done with this node
				break;
			}
		}

		// if we never found a frame, we have to make a new one
		if (createNewFrame)
		{
			// create new frame
			ModelNodeFrame* nodeFrame = new ModelNodeFrame;
			this->nodeFrames.Append(nodeFrame);

			// get state
			const State& state = this->attributes->GetState(skin.path);

			// setup handler
			nodeFrame->GetHandler()->SetModelHandler(this);
			nodeFrame->GetHandler()->SetType(skin.type);
			nodeFrame->GetHandler()->SetName(skin.name);
			nodeFrame->GetHandler()->SetPath(skin.path);
			nodeFrame->GetHandler()->Setup(state.material, tempResource);

			// add frame to tab box
			nodeWidget->addTab(nodeFrame, skin.name.AsCharPtr());
		}
	}

	// now do the same for particles
	for (i = 0; i < particleNodes.Size(); i++)
	{
		const ToolkitUtil::ModelConstants::ParticleNode& particleNode = particleNodes[i];
		bool createNewFrame = true;

		IndexT j;
		for (j = 0; j < this->particleFrames.Size(); j++)
		{
			// get node frame
			Ptr<ParticleNodeHandler> nodeHandler = this->particleFrames[j]->GetHandler();
			if (nodeHandler->GetPath() == particleNode.path)
			{
				// get state
				const State& state = this->attributes->GetState(nodeHandler->GetPath());

				// prepare for update
				this->particleFrames[j]->setUpdatesEnabled(false);

				// refresh handler
				nodeHandler->HardRefresh(state.material, tempResource);

				// prepare for update
				this->particleFrames[j]->setUpdatesEnabled(true);

				// frame found, dont make
				createNewFrame = false;

				// we are done with this node
				break;
			}
		}

		// if we never found a frame, we have to make a new one
		if (createNewFrame)
		{
			// create new frame
			ParticleNodeFrame* nodeFrame = new ParticleNodeFrame;
			this->particleFrames.Append(nodeFrame);

			// get state
			const State& state = this->attributes->GetState(particleNode.path);

			// get attrs
			const Particles::EmitterAttrs& attrs = this->attributes->GetEmitterAttrs(particleNodes[i].path);

			// setup handler
			nodeFrame->GetHandler()->SetModelHandler(this);
			nodeFrame->GetHandler()->SetType(particleNode.type);
			nodeFrame->GetHandler()->SetName(particleNode.name);
			nodeFrame->GetHandler()->SetPath(particleNode.path);
			nodeFrame->GetHandler()->Setup(state.material, tempResource);

			// add frame to tab box
			nodeWidget->addTab(nodeFrame, particleNode.name.AsCharPtr());
		}
	}

	// go through node frames and remove those who are irrelevant
	for (i = 0; i < this->nodeFrames.Size(); i++)
	{
		// get node frame
		Ptr<ModelNodeHandler> nodeHandler = this->nodeFrames[i]->GetHandler();

		// get state
		const State& state = this->attributes->GetState(nodeHandler->GetPath());

		if (!this->attributes->HasState(nodeHandler->GetPath()))
		{
			// remove from tab widget
			nodeWidget->removeTab(nodeWidget->indexOf(this->nodeFrames[i]));
			delete this->nodeFrames[i];

			// this node is effectively gone, so remove it!
			nodeHandler->Discard();
			this->nodeFrames.EraseIndex(i--);
		}
	}

	// go through particles and remove unnecessary frames
	for (i = 0; i < this->particleFrames.Size(); i++)
	{
		// get node frame
		Ptr<ParticleNodeHandler> nodeHandler = this->particleFrames[i]->GetHandler();

		// only update this node if it still exists
		if (!this->attributes->HasState(nodeHandler->GetPath()))
		{
			// remove from tab widget
			nodeWidget->removeTab(nodeWidget->indexOf(this->particleFrames[i]));
			delete this->particleFrames[i];

			// this node is effectively gone, so remove it!
			nodeHandler->Discard();
			this->particleFrames.EraseIndex(i--);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::SoftRefresh()
{
	// format internal model name
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// go through node frames and remove those who are irrelevant
	IndexT i;
	for (i = 0; i < this->nodeFrames.Size(); i++)
	{
		// get node frame
		Ptr<ModelNodeHandler> nodeHandler = this->nodeFrames[i]->GetHandler();

		// get state
		const State& state = this->attributes->GetState(nodeHandler->GetPath());

		// prepare for update
		this->nodeFrames[i]->setUpdatesEnabled(false);

		// refresh handler
		nodeHandler->SoftRefresh(state.material, tempResource);

		// prepare for update
		this->nodeFrames[i]->setUpdatesEnabled(true);
	}

	// go through particles and remove unnecessary frames
	for (i = 0; i < this->particleFrames.Size(); i++)
	{
		// get node frame
		Ptr<ParticleNodeHandler> nodeHandler = this->particleFrames[i]->GetHandler();

		// get state
		const State& state = this->attributes->GetState(nodeHandler->GetPath());

		// prepare for update
		this->particleFrames[i]->setUpdatesEnabled(false);

		// refresh handler
		nodeHandler->SoftRefresh(state.material, tempResource);

		// prepare for update
		this->particleFrames[i]->setUpdatesEnabled(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::OnModelModified(bool structureChange)
{
	// create memory stream to which modifier will write
	Ptr<MemoryStream> stream = MemoryStream::Create();
	this->attributes->Save(stream.upcast<Stream>());
	stream->SetAccessMode(Stream::ReadAccess);
	stream->Open();
	void* data = stream->Map();
	SizeT length = stream->GetSize();
	String attrVersion;
	attrVersion.Set((const char*)data, length);
	stream->Unmap();
	stream->Close();

	stream = MemoryStream::Create();
	this->constants->Save(stream.upcast<Stream>());
	stream->SetAccessMode(Stream::ReadAccess);
	stream->Open();
	data = stream->Map();
	length = stream->GetSize();
	String constsVersion;
	constsVersion.Set((const char*)data, length);
	stream->Unmap();
	stream->Close();

	stream = MemoryStream::Create();
	this->physics->Save(stream.upcast<Stream>());
	stream->SetAccessMode(Stream::ReadAccess);
	stream->Open();
	data = stream->Map();
	length = stream->GetSize();
	String physVersion;
	physVersion.Set((const char*)data, length);
	stream->Unmap();
	stream->Close();

	// add constants
	this->action->AddVersion(attrVersion, constsVersion, physVersion, structureChange);

	// apply action
	this->action->DoAndMakeCurrent();

	// finally push action to stack
	ContentBrowserApp::Instance()->PushAction(this->action.upcast<BaseAction>());
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::AddParticleNode()
{
	// get model data, force a reload
	Ptr<ModelAttributes> attrs = ModelDatabase::Instance()->LookupAttributes(this->category + "/" + this->model);
	Ptr<ModelConstants> consts = ModelDatabase::Instance()->LookupConstants(this->category + "/" + this->model);
	Ptr<ModelPhysics> phys = ModelDatabase::Instance()->LookupPhysics(this->category + "/" + this->model);

	// we should now have an empty particle effect, so now we need a node
	String name;
	IndexT number = consts->GetParticleNodes().Size();
	name.Format("node_%d", number);
	while (consts->HasParticleNode(name)) name.Format("node_%d", number++);

	ModelConstants::ParticleNode node;	
	node.name = name;
	node.type = "particle";
	node.path = "root/" + name;
	node.primitiveGroupIndex = 0;

	// now add particle node to constants
	consts->AddParticleNode(name, node);

	// create state
	State state;
	state.material = "ParticleUnlit";
	state.textures.Append(Texture("DiffuseMap", "tex:system/nebulalogo"));
	state.textures.Append(Texture("NormalMap", "tex:system/nobump"));

	// set state in attributes for this node
	attrs->SetState(node.path, state);

	// create set of emitter attributes, setup defaults
	EmitterAttrs emitterAttrs;
	emitterAttrs.SetBool(EmitterAttrs::Looping, true);
	emitterAttrs.SetFloat(EmitterAttrs::EmissionDuration, 25.0f);
	emitterAttrs.SetFloat(EmitterAttrs::Gravity, -9.82f);
	emitterAttrs.SetFloat(EmitterAttrs::ActivityDistance, 100.0f);
	emitterAttrs.SetFloat(EmitterAttrs::StartRotationMin, 20.0f);
	emitterAttrs.SetFloat(EmitterAttrs::StartRotationMax, 60.0f);
	emitterAttrs.SetInt(EmitterAttrs::AnimPhases, 1);
	EnvelopeCurve lifeTime;
	lifeTime.Setup(1, 1, 1, 1, 0.33f, 0.66f, 0, 0, EnvelopeCurve::Sine);
	EnvelopeCurve emissionFrequency;
	emissionFrequency.Setup(25, 25, 25, 25, 0.33f, 0.66f, 1, 1, EnvelopeCurve::Sine);
	emissionFrequency.SetLimits(0, 25);
	EnvelopeCurve alpha;
	alpha.Setup(0, 0.5f, 0.5f, 0, 0.33f, 0.66f, 1, 0, EnvelopeCurve::Sine);
	EnvelopeCurve size;
	size.Setup(0, 0.5f, 0.5f, 0.0f, 0.33f, 0.66f, 1, 1, EnvelopeCurve::Sine);
	emitterAttrs.SetEnvelope(EmitterAttrs::LifeTime, lifeTime);
	emitterAttrs.SetEnvelope(EmitterAttrs::VelocityFactor, lifeTime);
	emitterAttrs.SetEnvelope(EmitterAttrs::EmissionFrequency, emissionFrequency);
	emitterAttrs.SetEnvelope(EmitterAttrs::Alpha, alpha);
	emitterAttrs.SetEnvelope(EmitterAttrs::Size, size);

	// set attributes
	attrs->SetEmitterAttrs(node.path, emitterAttrs);
	attrs->SetEmitterMesh(node.path, "");

	// save new changes and make a new model
	this->OnModelModified(true);
	this->HardRefresh();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::RemoveParticleNode(const Util::String path, const Util::String node)
{
	Ptr<ModelAttributes> attrs = ModelDatabase::Instance()->LookupAttributes(this->category + "/" + this->model);
	Ptr<ModelConstants> consts = ModelDatabase::Instance()->LookupConstants(this->category + "/" + this->model);
	Ptr<ModelPhysics> phys = ModelDatabase::Instance()->LookupPhysics(this->category + "/" + this->model);

	// simply delete the node
	attrs->DeleteState(path);
	attrs->DeleteEmitterAttrs(path);
	attrs->DeleteEmitterMesh(path);
	consts->DeleteParticleNode(node);

	// apply modifications
	this->OnModelModified(true);
	this->HardRefresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::Save()
{
	// firstly, tag current version to be final in the action
	this->action->TagAsFinal();

	// create resource string
	String resource;
	resource.Format("src:models/%s/%s.attributes", this->category.AsCharPtr(), this->model.AsCharPtr());
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(resource);
	stream->SetAccessMode(Stream::WriteAccess);
	stream->Open();
	String lastVersion = this->action->GetLastAttrVersion();
	stream->Write((void*)lastVersion.AsCharPtr(), lastVersion.Length());
	stream->Close();
	
	resource.Format("src:models/%s/%s.constants", this->category.AsCharPtr(), this->model.AsCharPtr());
	stream = IoServer::Instance()->CreateStream(resource);
	stream->SetAccessMode(Stream::WriteAccess);
	stream->Open();
	lastVersion = this->action->GetLastConstVersion();
	stream->Write((void*)lastVersion.AsCharPtr(), lastVersion.Length());
	stream->Close();

	resource.Format("src:models/%s/%s.physics", this->category.AsCharPtr(), this->model.AsCharPtr());
	stream = IoServer::Instance()->CreateStream(resource);
	stream->SetAccessMode(Stream::WriteAccess);
	stream->Open();
	lastVersion = this->action->GetLastPhysVersion();
	stream->Write((void*)lastVersion.AsCharPtr(), lastVersion.Length());
	stream->Close();

	// create model builder
	Ptr<ModelBuilder> modelBuilder = ModelBuilder::Create();

	// set data for model builder
	modelBuilder->SetAttributes(this->attributes);
	modelBuilder->SetConstants(this->constants);
	modelBuilder->SetPhysics(this->physics);

	// create original resource uri
	resource.Format("mdl:%s/%s.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// save model to temporary
	modelBuilder->SaveN3(resource, Platform::Win32);

	// send modification message to remote end
	Ptr<ReloadResourceIfExists> msg = ReloadResourceIfExists::Create();
	msg->SetResourceName(resource);
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(msg.upcast<Messaging::Message>());

	// save physics as well
	resource.Format("physics:%s/%s.np3", this->category.AsCharPtr(), this->model.AsCharPtr());
	modelBuilder->SaveN3Physics(resource, Platform::Win32);

	// also send physics update
	msg = ReloadResourceIfExists::Create();
	msg->SetResourceName(resource);
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::SaveAs()
{
	// perform a modify which will create a new action
	this->OnModelModified();

	// firstly, tag current version to be final in the action
	this->action->TagAsFinal();

	QFileDialog fileDialog(NULL, "Save Model As", IO::URI("mdl:").GetHostAndLocalPath().AsCharPtr(), "*.n3");
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);

	int result = fileDialog.exec();
	if (result == QDialog::Accepted)
	{
		// get path
		QString fullPath = fileDialog.selectedFiles()[0];
		String path(fullPath.toUtf8().constData());

		// get category
		String cat = path.ExtractLastDirName();
		String file = path.ExtractFileName();
		file.StripFileExtension();

		// check if we are overwriting ourselves
		if (cat == this->category && file == this->model)
		{
			// same file, use normal save instead
			this->Save();
			return;
		}
		// create resource string
		String newAttrResource;
		String newConstResource;
		String newPhyResource;

		// reformat resource
		newAttrResource.Format("src:models/%s/%s.attributes", cat.AsCharPtr(), file.AsCharPtr());
		newConstResource.Format("src:models/%s/%s.constants", cat.AsCharPtr(), file.AsCharPtr());
		newPhyResource.Format("src:models/%s/%s.physics", cat.AsCharPtr(), file.AsCharPtr());

		String attrResource;
		String constResource;
		String phyResource;

		// reformat resource
		attrResource.Format("src:models/%s/%s.attributes", this->category.AsCharPtr(), this->model.AsCharPtr());
		constResource.Format("src:models/%s/%s.constants", this->category.AsCharPtr(), this->model.AsCharPtr());
		phyResource.Format("src:models/%s/%s.physics", this->category.AsCharPtr(), this->model.AsCharPtr());

		// create string of name
		String name = cat + "/" + file;

		// copy original model to new
		IoServer::Instance()->CopyFile(attrResource, newAttrResource);
		IoServer::Instance()->CopyFile(constResource, newConstResource);
		IoServer::Instance()->CopyFile(phyResource, newPhyResource);	

		// create new attributes, constants and physics
		Ptr<ModelAttributes> attrs = ModelDatabase::Instance()->LookupAttributes(name, true);
		Ptr<ModelConstants> consts = ModelDatabase::Instance()->LookupConstants(name, true);
		Ptr<ModelPhysics> phys = ModelDatabase::Instance()->LookupPhysics(name, true);

		// also replace old xml by creating stream
		String lastVersion = this->action->GetLastAttrVersion();
		Ptr<Stream> stream = IoServer::Instance()->CreateStream(newAttrResource);
		stream->SetAccessMode(Stream::WriteAccess);
		stream->Open();
		stream->Write((void*)lastVersion.AsCharPtr(), lastVersion.Length());
		stream->Close();

		// create model builder
		Ptr<ModelBuilder> modelBuilder = ModelBuilder::Create();

		// clear attributes
		attrs->Clear();

		// save attributes again
		attrs->Load(stream);

		// set data for model builder
		modelBuilder->SetAttributes(attrs);
		modelBuilder->SetConstants(consts);
		modelBuilder->SetPhysics(phys);

		// create original resource uri
		String resource;
		resource.Format("mdl:%s/%s.n3", cat.AsCharPtr(), file.AsCharPtr());

		// save model to temporary
		modelBuilder->SaveN3(resource, Platform::Win32);

		// send modification message to remote end
		Ptr<ReloadResourceIfExists> msg = ReloadResourceIfExists::Create();
		msg->SetResourceName(resource);
		QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(msg.upcast<Messaging::Message>());

		// save physics as well
		resource.Format("physics:%s/%s.np3", cat.AsCharPtr(), file.AsCharPtr());
		modelBuilder->SaveN3Physics(resource, Platform::Win32);

		// also send physics update
		msg = ReloadResourceIfExists::Create();
		msg->SetResourceName(resource);
		QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(msg.upcast<Messaging::Message>());

		// emit that the model handler saved a new file
		String res;
		res.Format("mdl:cat/file");
		emit ModelSavedAs(res);
	}	
}

//------------------------------------------------------------------------------
/**
	This function will take the current constants, physics and attributes, compose a model and replace the intermediate one.
	Use this function whenever there is some substantial change to a model node, for example material switch or node addition/removal
*/
void
ModelHandler::MakeModel()
{
	// create model builder
	Ptr<ModelBuilder> modelBuilder = ModelBuilder::Create();

	// set data for model builder
	modelBuilder->SetAttributes(this->attributes);
	modelBuilder->SetConstants(this->constants);
	modelBuilder->SetPhysics(this->physics);

	// create original resource uri
	String resource;
	resource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// save model to temporary
	modelBuilder->SaveN3(resource, Platform::Win32);

	// reload model, then call OnModelReloaded when its done
	Ptr<ReloadResourceIfExists> msg = ReloadResourceIfExists::Create();
	msg->SetResourceName(resource);
	__StaticSend(GraphicsInterface, msg);
	this->OnModelReloaded(msg.upcast<Messaging::Message>());
	//__SingleFireCallback(Widgets::ModelHandler, OnModelReloaded, this, msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelHandler::OnModelReloaded( const Ptr<Messaging::Message>& msg )
{
	// if we have a character component, make sure to set all his skins as visible again
	if (0 != this->characterFrame)
	{
		const Ptr<CharacterNodeHandler>& charHandler = this->characterFrame->GetHandler();
		//charHandler->ReshowSkins();
	}

	Ptr<ReloadResourceIfExists> rmsg = msg.downcast<ReloadResourceIfExists>();
	Ptr<PreviewState> previewState = ContentBrowserApp::Instance()->GetPreviewState();
	String gfx = rmsg->GetResourceName();
	previewState->SetModel(gfx);
}

//------------------------------------------------------------------------------
/**
*/
void
ModelHandler::SetupTabs()
{
	// format internal model name
	String tempResource;
	tempResource.Format("int:models/%s/%s_temp.n3", this->category.AsCharPtr(), this->model.AsCharPtr());

	// create grid layout for node frame
	QTabWidget* nodeWidget = this->ui->nodeWidget;

	// enable add particle button
	this->ui->addParticleNode->setEnabled(true);

	// remove tabs
	while (nodeWidget->currentWidget())
	{
		QWidget* widget = nodeWidget->widget(0);
		delete widget;
		nodeWidget->removeTab(0);
	}

	// get list of shapes
	const Array<ModelConstants::ShapeNode>& shapes = this->constants->GetShapeNodes();

	// iterate over shapes and create frames
	IndexT i;
	for (i = 0; i < shapes.Size(); i++)
	{
		// create new frame
		ModelNodeFrame* nodeFrame = new ModelNodeFrame;
		this->nodeFrames.Append(nodeFrame);

		// get state
		const State& state = this->attributes->GetState(shapes[i].path);

		// setup handler
		nodeFrame->GetHandler()->SetModelHandler(this);
		nodeFrame->GetHandler()->SetType(shapes[i].type);
		nodeFrame->GetHandler()->SetName(shapes[i].name);
		nodeFrame->GetHandler()->SetPath(shapes[i].path);
		nodeFrame->GetHandler()->Setup(state.material, tempResource);

		// add frame to tab box
		nodeWidget->addTab(nodeFrame, shapes[i].name.AsCharPtr());
	}

	// get list of characters
	const Array<ModelConstants::CharacterNode>& characters = this->constants->GetCharacterNodes();

	// iterate over shapes and create frames
	for (i = 0; i < characters.Size(); i++)
	{
		CharacterNodeFrame* nodeFrame = new CharacterNodeFrame;
		this->characterFrame = nodeFrame;

		nodeFrame->GetHandler()->SetItemHandler(this);
		nodeFrame->GetHandler()->SetName(characters[i].name);
		nodeFrame->GetHandler()->Setup();

		// add frame to tab box
		nodeWidget->addTab(nodeFrame, characters[i].name.AsCharPtr());

		// disable add particle button
		this->ui->addParticleNode->setEnabled(false);
	}

	// get list of all skins
	const Array<ModelConstants::Skin>& skins = this->constants->GetSkins();

	// iterate over shapes and create frames
	for (i = 0; i < skins.Size(); i++)
	{
		// create new frame
		ModelNodeFrame* nodeFrame = new ModelNodeFrame;
		this->nodeFrames.Append(nodeFrame);

		// skip skin if it contains no fragments
		if (skins[i].skinFragments.IsEmpty())
		{
			continue;
		}

		// get state
		const State& state = this->attributes->GetState(skins[i].path);

		// skip skins without fragments
		if (skins[i].skinFragments.Size() == 0)
		{
			continue;
		}

		// setup handler
		nodeFrame->GetHandler()->SetModelHandler(this);
		nodeFrame->GetHandler()->SetType(skins[i].type);
		nodeFrame->GetHandler()->SetName(skins[i].name);
		nodeFrame->GetHandler()->SetPath(skins[i].path);
		nodeFrame->GetHandler()->Setup(state.material, tempResource);

		// add frame to tab box
		nodeWidget->addTab(nodeFrame, skins[i].name.AsCharPtr());
	}

	// get a list of all particles
	const Array<ModelConstants::ParticleNode>& particleNodes = this->constants->GetParticleNodes();
	for (i = 0; i < particleNodes.Size(); i++)
	{
		// create new frame
		ParticleNodeFrame* nodeFrame = new ParticleNodeFrame;
		this->particleFrames.Append(nodeFrame);

		// get state
		const State& state = this->attributes->GetState(particleNodes[i].path);

		// get attrs
		const Particles::EmitterAttrs& attrs = this->attributes->GetEmitterAttrs(particleNodes[i].path);

		// setup handler
		nodeFrame->GetHandler()->SetModelHandler(this);
		nodeFrame->GetHandler()->SetType(particleNodes[i].type);
		nodeFrame->GetHandler()->SetName(particleNodes[i].name);
		nodeFrame->GetHandler()->SetPath(particleNodes[i].path);
		nodeFrame->GetHandler()->Setup(state.material, tempResource);

		// add frame to tab box
		nodeWidget->addTab(nodeFrame, particleNodes[i].name.AsCharPtr());
	}
}

} // namespace Widgets