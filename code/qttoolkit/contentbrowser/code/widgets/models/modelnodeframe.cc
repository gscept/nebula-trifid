//------------------------------------------------------------------------------
//  modelnodeframe.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelnodeframe.h"

namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::ModelNodeFrame()
{
	/// setup ui
	this->ui.setupUi(this);
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::~ModelNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::AddModelNode(const Util::String& type, const Util::String& name, const Util::String& path, const Util::String& res)
{
	Ptr<ModelNodeHandler> handler = ModelNodeHandler::Create();
	handler->SetModelHandler(this->modelHandler);
	handler->SetUI(&this->ui);
	handler->SetType(type);
	handler->SetName(name);
	handler->SetPath(path);
	handler->Setup(res);
	this->itemHandlers.Append(handler);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeFrame::Discard()
{
	this->modelHandler = 0;
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Discard();
	}
	this->itemHandlers.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::Refresh()
{
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Refresh();
	}
}

} // namespace Widgets