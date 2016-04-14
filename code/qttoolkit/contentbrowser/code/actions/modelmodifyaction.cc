//------------------------------------------------------------------------------
//  modelmodifyaction.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelmodifyaction.h"
#include "n3util/n3xmlexporter.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "io/stream.h"
#include "io/ioserver.h"
#include "modelutil/modelbuilder.h"
#include "modelutil/modelattributes.h"
#include "modelutil/modeldatabase.h"
#include "widgets/models/modelhandler.h"
#include "messaging/messagecallbackhandler.h"

using namespace ToolkitUtil;
using namespace Graphics;
using namespace Util;
using namespace IO;

namespace Actions
{
__ImplementClass(Actions::ModelModifyAction, 'MMDA', Actions::BaseAction);

//------------------------------------------------------------------------------
/**
*/
ModelModifyAction::ModelModifyAction()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ModelModifyAction::~ModelModifyAction()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelModifyAction::Cleanup()
{
    this->handler = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelModifyAction::Undo()
{
	BaseAction::Undo();
	this->handler->Refresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelModifyAction::Redo()
{
	BaseAction::Redo();
	this->handler->Refresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelModifyAction::Do()
{
	// get constants and attributes
	Ptr<ModelAttributes> attributes = ModelDatabase::Instance()->LookupAttributes(this->category + "/" + this->model);
	Ptr<ModelConstants> constants = ModelDatabase::Instance()->LookupConstants(this->category + "/" + this->model);
	Ptr<ModelPhysics> physics = ModelDatabase::Instance()->LookupPhysics(this->category + "/" + this->model);

	// save attrs
	const String& attrVersion = this->attrVersions[this->currentVersion];
	const String& constVersion = this->constVersions[this->currentVersion];
	const String& physVersion = this->physVerions[this->currentVersion];

	// load all 
	Ptr<MemoryStream> attrStream = MemoryStream::Create();
	attrStream->SetAccessMode(Stream::WriteAccess);
	attrStream->Open();
	attrStream->Write(attrVersion.AsCharPtr(), attrVersion.Length());
	attrStream->Close();

	Ptr<MemoryStream> constStream = MemoryStream::Create();
	constStream->SetAccessMode(Stream::WriteAccess);
	constStream->Open();
	constStream->Write(constVersion.AsCharPtr(), constVersion.Length());
	constStream->Close();

	Ptr<MemoryStream> physStream = MemoryStream::Create();
	physStream->SetAccessMode(Stream::WriteAccess);
	physStream->Open();
	physStream->Write(physVersion.AsCharPtr(), physVersion.Length());
	physStream->Close();

	// now clear attributes and load from stream
	attributes->Clear();
	attributes->Load(attrStream.upcast<Stream>());
	constants->Clear();
	constants->Load(constStream.upcast<Stream>());
	physics->Clear();
	physics->Load(physStream.upcast<Stream>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelModifyAction::Discard()
{
	// clear versions list
	this->attrVersions.Clear();
	this->constVersions.Clear();
	this->physVerions.Clear();

	// call base class
	BaseAction::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelModifyAction::AddVersion(const Util::String& attributes, const Util::String& constants, const Util::String& physics)
{
	// go through from current index to end of list and remove indices
	int i;
	for (i = this->currentVersion + 1; i < this->attrVersions.Size(); i++)
	{
		this->attrVersions.EraseIndex(i);
		this->constVersions.EraseIndex(i);
		this->physVerions.EraseIndex(i);	
	}

	// then add version to lists
	this->attrVersions.Append(attributes);
	this->constVersions.Append(constants);
	this->physVerions.Append(physics);
	this->numVersions++;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
ModelModifyAction::GetLastAttrVersion() const
{
	return this->attrVersions[this->currentVersion];
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
ModelModifyAction::GetLastConstVersion() const
{
	return this->constVersions[this->currentVersion];
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
ModelModifyAction::GetLastPhysVersion() const
{
	return this->physVerions[this->currentVersion];
}

} // namespace Actions